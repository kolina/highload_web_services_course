#pragma once

#include <fastcgi2/handler.h>
#include <fastcgi2/request.h>

#include <string>

namespace NMail {

class TMails {
public:
    TMails();

    void Dispatch(fastcgi::Request *request, fastcgi::HandlerContext *context);

private:
    void ListMails(fastcgi::Request *request, fastcgi::HandlerContext *context, const std::string& user);
    void GetMail(fastcgi::Request *request, fastcgi::HandlerContext *context, const std::string& user, const std::string& message);
    void CreateMail(fastcgi::Request *request, fastcgi::HandlerContext *context, const std::string& user, const std::string& message);
};

}
