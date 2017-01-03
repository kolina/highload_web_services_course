#include "mail.h"

#include <fastcgi2/stream.h>

#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/basic/document.hpp>

namespace {

    inline void getPathComponents(const fastcgi::Request * const request, std::vector<std::string>& path_components) {
        std::stringstream path_stream(request->getScriptName());
        std::string current_token;
        while (std::getline(path_stream, current_token, '/')) {
            path_components.push_back(std::move(current_token));
        }
    }

    inline void addLinks(bsoncxx::builder::basic::sub_document& doc, std::initializer_list<NMail::TLink>&& links) {
        bsoncxx::builder::basic::document links_object{};
        links_object.append(bsoncxx::builder::basic::kvp("curies",
                                                         [](bsoncxx::builder::basic::sub_array sub_arr) {
                                                            sub_arr.append([](bsoncxx::builder::basic::sub_document sub_doc) {
                                                                              sub_doc.append(bsoncxx::builder::basic::kvp("name", "ma"),
                                                                                             bsoncxx::builder::basic::kvp("href", "http://example.com/docs/rels/{rel}"),
                                                                                             bsoncxx::builder::basic::kvp("templated", true));
                                                                           });
                                                         })
                           );

        for (auto&& link_obj : links) {
            bsoncxx::builder::basic::document sub_doc{};
            sub_doc.append(bsoncxx::builder::basic::kvp("href", link_obj.link_address));
            if (link_obj.templated)
                sub_doc.append(bsoncxx::builder::basic::kvp("templated", true));
            links_object.append(bsoncxx::builder::basic::kvp(link_obj.link_name, std::move(sub_doc)));
        }

        doc.append(bsoncxx::builder::basic::kvp("_links", std::move(links_object)));
    }

    inline std::string getStringFromElement(const bsoncxx::document::view& element, const std::string& key) {
        return element[key].get_utf8().value.to_string();
    }

    inline void constructMessage(bsoncxx::builder::basic::sub_document& doc, const std::string& user, const bsoncxx::document::view& element) {
        addLinks(doc, {{"self", "/" + user + "/messages/" + getStringFromElement(element, "_id"), false},
                       {"ma:thread", "/" + user + "/threads/" + getStringFromElement(element, "thread_id"), false}});
        doc.append(bsoncxx::builder::basic::kvp("theme", "Answer to all questions."),
                   bsoncxx::builder::basic::kvp("from", element["from"].get_utf8()),
                   bsoncxx::builder::basic::kvp("to", element["to"].get_array()),
                   bsoncxx::builder::basic::kvp("created_at", element["created_at"].get_int32()),
                   bsoncxx::builder::basic::kvp("body", "Answer to all questions in universe is 42."));
    }

}

namespace NMail {

    const std::string DATABASE = "mail_service";
    const std::string COLLECTION = "mails";
    const size_t ANSWER_LIMIT = 100;

    thread_local mongocxx::client TMails::mongoClient{mongocxx::uri{}};

    TMails::TMails() {}

    void TMails::Dispatch(fastcgi::Request *request, fastcgi::HandlerContext *context) {
        auto& method = request->getRequestMethod();

        request->setContentType("application/hal+json");

        std::vector<std::string> path_components;
        getPathComponents(request, path_components);
        if (method == "GET" && path_components.size() == 3 && path_components[2] == "messages") {
            this->ListMails(request, context, path_components[1]);
        }
        else if (method == "GET" && path_components.size() == 4 && path_components[2] == "messages") {
            this->GetMail(request, context, path_components[1], path_components[3]);
        }
        else if (method == "PUT" && path_components.size() == 4 && path_components[2] == "messages") {
            this->CreateMail(request, context, path_components[1], path_components[3]);
        }
        else
            request->setStatus(404);
    }

    void TMails::ListMails(fastcgi::Request *request, fastcgi::HandlerContext *context, const std::string& user) {
        auto query = bsoncxx::builder::stream::document{}
                << "$or" << bsoncxx::builder::stream::open_array
                << bsoncxx::builder::stream::open_document << "from" << user << bsoncxx::builder::stream::close_document
                << bsoncxx::builder::stream::open_document << "to" << user << bsoncxx::builder::stream::close_document
                << bsoncxx::builder::stream::close_array << bsoncxx::builder::stream::finalize;

        mongocxx::options::find options;
        auto sort = bsoncxx::builder::stream::document{} << "created_at" << -1 << bsoncxx::builder::stream::finalize;
        options.sort(sort.view());
        options.limit(ANSWER_LIMIT);

        auto cursor = mongoClient[DATABASE][COLLECTION].find(query.view(), options);

        bsoncxx::builder::basic::document response{};
        addLinks(response, {{"self", request->getScriptFilename(), false},
                            {"ma:create", request->getScriptName(), false},
                            {"ma:find", request->getScriptName() + "/{messageId}", true}});

        bsoncxx::builder::basic::array messages{};
        for (auto&& item: cursor) {
            bsoncxx::builder::basic::document message{};
            constructMessage(message, user, item);
            messages.append(std::move(message));
        }

        response.append(bsoncxx::builder::basic::kvp("_embedded",
                                                     [&messages](bsoncxx::builder::basic::sub_document sub_doc) {
                                                        sub_doc.append(bsoncxx::builder::basic::kvp("ma:message_item", std::move(messages)));
                                                     })
                       );

        fastcgi::RequestStream stream(request);
        stream << bsoncxx::to_json(response.view()) << "\n";
    }

    void TMails::GetMail(fastcgi::Request *request, fastcgi::HandlerContext *context, const std::string& user, const std::string& message) {
        auto query = bsoncxx::builder::stream::document{}
                << "$and" << bsoncxx::builder::stream::open_array
                << bsoncxx::builder::stream::open_document << "_id" << message << bsoncxx::builder::stream::close_document
                << bsoncxx::builder::stream::open_document << "$or" << bsoncxx::builder::stream::open_array
                                                                    << bsoncxx::builder::stream::open_document << "from" << user << bsoncxx::builder::stream::close_document
                                                                    << bsoncxx::builder::stream::open_document << "to" << user << bsoncxx::builder::stream::close_document
                                                                   << bsoncxx::builder::stream::close_array
                << bsoncxx::builder::stream::close_document
                << bsoncxx::builder::stream::close_array << bsoncxx::builder::stream::finalize;

        auto result = mongoClient[DATABASE][COLLECTION].find_one(query.view());
        bsoncxx::builder::basic::document response{};
        if (result)
            constructMessage(response, user, *result);
        else {
            response.append(bsoncxx::builder::basic::kvp("Error", "Message mot found in database."));
            request->setStatus(404);
        }

        fastcgi::RequestStream stream(request);
        stream << bsoncxx::to_json(response.view()) << "\n";
    }

    void TMails::CreateMail(fastcgi::Request *request, fastcgi::HandlerContext *context, const std::string& user, const std::string& message) {
        std::string body;
        request->requestBody().toString(body);

        bsoncxx::document::value toInsert = bsoncxx::from_json(body);
        bsoncxx::document::view toInsertView = toInsert.view();

        fastcgi::RequestStream stream(request);
        bsoncxx::builder::basic::document response{};

        if (!request->hasArg("threadId") || request->getArg("threadId") == "") {
            request->setStatus(400);
            response.append(bsoncxx::builder::basic::kvp("Error", "Parameter threadId is missing."));
            stream << bsoncxx::to_json(response.view()) << "\n";
            return;
        }

        if (toInsertView.find("to") == toInsertView.end() || toInsertView["to"].type() != bsoncxx::type::k_array) {
            request->setStatus(400);
            response.append(bsoncxx::builder::basic::kvp("Error", "Validating of 'to' field failed."));
            stream << bsoncxx::to_json(response.view()) << "\n";
            return;
        }

        bsoncxx::array::view to = toInsertView["to"].get_array();
        size_t length = std::distance(to.begin(), to.end());

        for (size_t i = 0; i < length; ++i) {
            if (to[i].type() != bsoncxx::type::k_utf8) {
                request->setStatus(400);
                response.append(bsoncxx::builder::basic::kvp("Error", "Validating of 'to' field failed."));
                stream << bsoncxx::to_json(response.view()) << "\n";
                return;
            }
            else if (to[i].get_utf8().value.to_string() == user) {
                request->setStatus(400);
                response.append(bsoncxx::builder::basic::kvp("Error", "'To' field contains sender."));
                stream << bsoncxx::to_json(response.view()) << "\n";
                return;
            }
        }

        auto insertQuery = bsoncxx::builder::stream::document{}
                           << "from" << user
                           << "_id" << message
                           << "thread_id" << request->getArg("threadId")
                           << "to" << to
                           << "created_at" << (std::int32_t)(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()))
                           << bsoncxx::builder::stream::finalize;

        auto insertedResult = mongoClient[DATABASE][COLLECTION].insert_one(insertQuery.view());
        if (!insertedResult) {
            request->setStatus(500);
            response.append(bsoncxx::builder::basic::kvp("Error", "Error during insert in database."));
            stream << bsoncxx::to_json(response.view()) << "\n";
            return;
        }
        constructMessage(response, user, insertQuery);
        stream << bsoncxx::to_json(insertQuery.view()) << "\n";
    }

}
