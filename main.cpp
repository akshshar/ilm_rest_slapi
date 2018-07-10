#include <iostream>

#include "usr_interrupt_handler.hpp"
#include "runtime_utils.hpp"

#include "microsvc_controller.hpp"

#include "ServiceLayerRoute.h"
#include <csignal>

using namespace web;
using namespace cfx;


using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientReaderWriter;
using grpc::ClientWriter;
using grpc::CompletionQueue;
using grpc::Status;
using service_layer::SLInitMsg;
using service_layer::SLVersion;
using service_layer::SLGlobal;
using namespace openr;

std::string 
getEnvVar(std::string const & key)
{
    char * val = std::getenv( key.c_str() );
    return val == NULL ? std::string("") : std::string(val);
}


IosxrslVrf* vrfhandler_signum;
std::shared_ptr<IosxrslRoute> route_signum;
AsyncNotifChannel* asynchandler_signum;
bool sighandle_initiated = false;

void 
signalHandler(int signum)
{

   if (!sighandle_initiated) {
       sighandle_initiated = true;

       // Clear out the last vrfRegMsg batch
       vrfhandler_signum->vrf_msg.clear_vrfregmsgs();

       // Create a fresh SLVrfRegMsg batch for cleanup
       vrfhandler_signum->vrfRegMsgAdd("default");

       vrfhandler_signum->unregisterVrf(AF_INET);
       vrfhandler_signum->unregisterVrf(AF_INET6);

       // Shutdown the Async Notification Channel  
       asynchandler_signum->Shutdown();

       //terminate program  
       exit(signum);  
    } 
}

int main(int argc, const char * argv[]) {
    InterruptHandler::hookSIGINT();


    auto server_ip = getEnvVar("SERVER_IP");
    auto server_port = getEnvVar("SERVER_PORT");

    if (server_ip == "" || server_port == "") {
        if (server_ip == "") {
            LOG(ERROR) << "SERVER_IP environment variable not set";
        }
        if (server_port == "") {
            LOG(ERROR) << "SERVER_PORT environment variable not set";
        }
        return 1;

    }

    std::string grpc_server = server_ip + ":" + server_port;
    auto channel = grpc::CreateChannel(
                              grpc_server, grpc::InsecureChannelCredentials());

 
    LOG(INFO) << "Connecting IOS-XR to gRPC server at " << grpc_server;

    AsyncNotifChannel asynchandler(channel);

    // Acquire the lock
    std::unique_lock<std::mutex> initlock(init_mutex);

    // Spawn reader thread that maintains our Notification Channel
    std::thread thread_ = std::thread(&AsyncNotifChannel::AsyncCompleteRpc, &asynchandler);


    service_layer::SLInitMsg init_msg;
    init_msg.set_majorver(service_layer::SL_MAJOR_VERSION);
    init_msg.set_minorver(service_layer::SL_MINOR_VERSION);
    init_msg.set_subver(service_layer::SL_SUB_VERSION);


    asynchandler.SendInitMsg(init_msg);

    // Wait on the mutex lock
    while (!init_success) {
        init_condVar.wait(initlock);
    }

    // Set up a new channel for vrf/route messages

    auto vrfhandler = IosxrslVrf(channel);

    // Create a new SLVrfRegMsg batch
    vrfhandler.vrfRegMsgAdd("default", 10, 500);

    // Register the SLVrfRegMsg batch for v4 and v6
    vrfhandler.registerVrf(AF_INET);
    vrfhandler.registerVrf(AF_INET6);

    std::shared_ptr<IosxrslRoute> iosxrsl_route;

 
    iosxrsl_route = std::make_unique<IosxrslRoute>(channel);

    iosxrsl_route->setVrfV4("default");

    asynchandler_signum = &asynchandler;
    vrfhandler_signum = &vrfhandler;
    route_signum = iosxrsl_route;
    LOG(INFO) << "Press control-c to quit";

    MicroserviceController server(iosxrsl_route);
    //server.iosxrsl_route = iosxrsl_route;

    server.setEndpoint("http://host_auto_ip4:6502/");

    try {
        // wait for server initialization...
        server.accept().wait();
        std::cout << "Modern C++ Microservice now listening for requests at: " << server.endpoint() << '\n';

        InterruptHandler::waitForUserInterrupt();

        server.shutdown().wait();
        signalHandler(1);
        thread_.join();
    }
    catch(std::exception & e) {
        std::cerr << "something went wrong! :(" << '\n';
        return 1;
    }
    catch(...) {
        RuntimeUtils::printStackTrace();
        return 1;
    }

    return 0;
}
