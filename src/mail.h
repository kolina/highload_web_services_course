#pragma once

#include <fastcgi2/handler.h>
#include <fastcgi2/request.h>

#include <mongocxx/client.hpp>
#include <mongocxx/uri.hpp>

#include <string>

namespace NMail {

    struct TLink {
        std::string link_name;
        std::string link_address;
        bool templated;
    };

    class TMails {
    public:
        TMails();

        void Dispatch(fastcgi::Request *request, fastcgi::HandlerContext *context);

    private:
        thread_local static mongocxx::client mongoClient;

        void ListMails(fastcgi::Request *request, fastcgi::HandlerContext *context, const std::string& user);
        void GetMail(fastcgi::Request *request, fastcgi::HandlerContext *context, const std::string& user, const std::string& message);
        void CreateMail(fastcgi::Request *request, fastcgi::HandlerContext *context, const std::string& user, const std::string& message);
    };

}
