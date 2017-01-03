#include "mail.h"

#include <fastcgi2/component.h>
#include <fastcgi2/component_factory.h>

class MailServiceFastCGI : virtual public fastcgi::Component, virtual public fastcgi::Handler
{
public:
    MailServiceFastCGI(fastcgi::ComponentContext *context) :
            fastcgi::Component(context)
    {

    }

    virtual void onLoad()
    {

    }

    virtual void onUnload()
    {

    }

    virtual void handleRequest(fastcgi::Request *request, fastcgi::HandlerContext *context)
    {

        Mails.Dispatch(request, context);
    }

private:

    NMail::TMails Mails;
};

FCGIDAEMON_REGISTER_FACTORIES_BEGIN()
    FCGIDAEMON_ADD_DEFAULT_FACTORY("MailServiceFastCGIFactory", MailServiceFastCGI)
FCGIDAEMON_REGISTER_FACTORIES_END()
