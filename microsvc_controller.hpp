#pragma once 

#include "basic_controller.hpp"
#include "ServiceLayerRoute.h"


using namespace cfx;
using namespace openr;


class MicroserviceController : public BasicController, Controller {
public:
    MicroserviceController(std::shared_ptr<IosxrslRoute> iosxrsl_route) : BasicController(iosxrsl_route) {}
    ~MicroserviceController() {}
    void handleGet(http_request message) override;
    void handlePut(http_request message) override;
    void handlePost(http_request message) override;
    void handlePatch(http_request message) override;
    void handleDelete(http_request message) override;
    void handleHead(http_request message) override;
    void handleOptions(http_request message) override;
    void handleTrace(http_request message) override;
    void handleConnect(http_request message) override;
    void handleMerge(http_request message) override;
    void initRestOpHandlers() override;    
    void ilm_route_add();
    void ilm_route_del();

private:
    static json::value responseNotImpl(const http::method & method);
};
