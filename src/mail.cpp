#include "mail.h"

#include <fastcgi2/stream.h>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>

#include <sstream>
#include <vector>
#include <string>
#include <utility>

namespace {

void getPathComponents(const fastcgi::Request * const request, std::vector<std::string>& path_components) {
        std::stringstream path_stream(request->getScriptName());
        std::string current_token;
        while (std::getline(path_stream, current_token, '/')) {
            path_components.push_back(std::move(current_token));
        }
    }

}

namespace NMail {

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
    rapidjson::Document response(rapidjson::kObjectType);
    rapidjson::Document::AllocatorType& allocator = response.GetAllocator();

    rapidjson::Value links(rapidjson::kObjectType);

    rapidjson::Value self(rapidjson::kObjectType);
    self.AddMember("href", request->getScriptName(), allocator);

    rapidjson::Value curies(rapidjson::kArrayType);
    rapidjson::Value curies_object(rapidjson::kObjectType);
    curies_object.AddMember("name", "ma", allocator);
    curies_object.AddMember("href", "http://example.com/docs/rels/{rel}", allocator);
    curies_object.AddMember("templated", true, allocator);
    curies.PushBack(curies_object, allocator);

    rapidjson::Value create(rapidjson::kObjectType);
    std::ostringstream oss_create;
    oss_create << "/" << user << "/create/{messageId}";
    create.AddMember("href", oss_create.str(), allocator);
    create.AddMember("templated", true, allocator);

    rapidjson::Value find(rapidjson::kObjectType);
    std::ostringstream oss_find;
    oss_find << "/" << user << "/messages/{messageId}";
    find.AddMember("href", oss_find.str(), allocator);
    find.AddMember("templated", true, allocator);

    links.AddMember("self", self, allocator);
    links.AddMember("curies", curies, allocator);
    links.AddMember("ma::create", create, allocator);
    links.AddMember("ma::find", find, allocator);

    rapidjson::Value embedded(rapidjson::kArrayType);

    response.AddMember("_links", links, allocator);
    response.AddMember("_embedded", embedded, allocator);

    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    response.Accept(writer);

    request->write(buffer.GetString(), buffer.GetSize());
    request->write("\n", 1);
}

void TMails::GetMail(fastcgi::Request *request, fastcgi::HandlerContext *context, const std::string& user, const std::string& message) {
    fastcgi::RequestStream stream(request);
    
    rapidjson::Document response(rapidjson::kObjectType);
    rapidjson::Document::AllocatorType& allocator = response.GetAllocator();

    rapidjson::Value links(rapidjson::kObjectType);

    rapidjson::Value self(rapidjson::kObjectType);
    self.AddMember("href", request->getScriptName(), allocator);

    rapidjson::Value curies(rapidjson::kArrayType);
    rapidjson::Value curies_object(rapidjson::kObjectType);
    curies_object.AddMember("name", "ma", allocator);
    curies_object.AddMember("href", "http://example.com/docs/rels/{rel}", allocator);
    curies_object.AddMember("templated", true, allocator);
    curies.PushBack(curies_object, allocator);

    links.AddMember("self", self, allocator);
    links.AddMember("curies", curies, allocator);

    response.AddMember("body", "The answer to all questions in universe is 42.", allocator);

    rapidjson::Value to(rapidjson::kArrayType);
    rapidjson::Value cc(rapidjson::kArrayType);

    response.AddMember("to", to, allocator);
    response.AddMember("cc", cc, allocator);

    response.AddMember("from", "", allocator);
    response.AddMember("theme", "Answer to all questions in universe.", allocator);

    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    response.Accept(writer);

    request->write(buffer.GetString(), buffer.GetSize());
    request->write("\n", 1);
}

void TMails::CreateMail(fastcgi::Request *request, fastcgi::HandlerContext *context, const std::string& user, const std::string& message) {
    std::string inputJson;

    request->requestBody().toString(inputJson);
    rapidjson::Document document;
    document.Parse(inputJson.c_str());
    if (!(document["to"].IsArray() &&
          document["to"].Size() > 0 && document["cc"].IsArray()))
    {
        request->setStatus(400);
        return;
    }
}

}
