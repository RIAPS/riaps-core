//
// Created by istvan on 5/11/17.
//

#include <componentmodel/r_devmapi.h>
#include <framework/rfw_configuration.h>

namespace riaps{
    namespace devm{
        DevmApi::DevmApi() : _socket(NULL) {

        }

        zsock_t* DevmApi::GetSocket() {
            return _socket;
        }

        bool DevmApi::RegisterActor(const std::string& actorName,
                                    const std::string& appName,
                                    const std::string& version) {
            bool result = true;

            /////
            /// Request
            capnp::MallocMessageBuilder message;
            auto root = message.initRoot<riaps::devm::DevmReq>();
            auto areqBuilder = root.initActorReg();

            areqBuilder.setActorName(actorName);
            areqBuilder.setAppName(appName);
            areqBuilder.setVersion("0");
            areqBuilder.setPid(::getpid());

            auto serializedMessage = capnp::messageToFlatArray(message);
            auto s = serializedMessage.asBytes().size();
            auto c = serializedMessage.asBytes().begin();

            zmsg_t* msg = zmsg_new();
            zmsg_pushmem(msg, c, s);

            std::string ipcAddress = riaps::framework::Configuration::GetDeviceManagerServiceIpc();
            zsock_t * client = zsock_new_req (ipcAddress.c_str());
            assert(client);

            zmsg_send(&msg, client);

            /////
            /// Response
            zmsg_t* msg_response = zmsg_recv(client);

            zframe_t* capnp_msgbody = zmsg_pop(msg_response);
            size_t    size = zframe_size(capnp_msgbody);
            byte*     data = zframe_data(capnp_msgbody);

            auto capnp_data = kj::arrayPtr(reinterpret_cast<const capnp::word*>(data), size / sizeof(capnp::word));

            capnp::FlatArrayMessageReader reader(capnp_data);
            auto devmrep= reader.getRoot<riaps::devm::DevmRep>();

            // Register actor
            if (devmrep.isActorReg()) {
                auto msg_response = devmrep.getActorReg();
                auto status = msg_response.getStatus();
                auto port = msg_response.getPort();

                if (status == Status::ERR){
                    std::cout << "Couldn't register actor: " << actorName << std::endl;
                    result = false;
                }
                else {
                    auto devmEndpoint = "tcp://localhost:" + std::to_string(port);
                    _socket = zsock_new_pair(devmEndpoint.c_str());
                    assert(_socket);
                }
            }

            /////
            /// Clean up
            zsock_disconnect(client, "%s", ipcAddress.c_str());
            zframe_destroy(&capnp_msgbody);
            zmsg_destroy(&msg_response);
            zsock_destroy(&client);

            return result;


        }

        bool DevmApi::RegisterDevice(const std::string &appName,
                                     const std::string &modelName,
                                     const std::string &typeName,
                                     const std::map<std::string, std::string> &args) {
            bool result = true;

            /////
            /// Request
            capnp::MallocMessageBuilder message;
            riaps::devm::DevmReq::Builder root = message.initRoot<riaps::devm::DevmReq>();
            auto dreqBuilder = root.initDeviceReg();

            dreqBuilder.setAppName(appName);
            dreqBuilder.setModelName(modelName);
            dreqBuilder.setTypeName(typeName);

            if (args.size()>0){
                auto dargs = dreqBuilder.initDeviceArgs(args.size());
                int i=0;
                for (auto it = args.begin(); it!=args.end(); it++,i++){
                    riaps::devm::DeviceArg::Builder darg = dargs[i];
                    darg.setName(it->first);
                    darg.setName(it->second);
                }
            }

            auto serializedMessage = capnp::messageToFlatArray(message);

            zmsg_t* msg = zmsg_new();
            zmsg_pushmem(msg, serializedMessage.asBytes().begin(), serializedMessage.asBytes().size());

            std::string ipcAddress = riaps::framework::Configuration::GetDeviceManagerServiceIpc();
            zsock_t * client = zsock_new_req (ipcAddress.c_str());
            assert(client);

            zmsg_send(&msg, client);

            /////
            /// Response
            zmsg_t* msg_response = zmsg_recv(client);

            zframe_t* capnp_msgbody = zmsg_pop(msg_response);
            size_t    size = zframe_size(capnp_msgbody);
            byte*     data = zframe_data(capnp_msgbody);

            auto capnp_data = kj::arrayPtr(reinterpret_cast<const capnp::word*>(data), size / sizeof(capnp::word));

            capnp::FlatArrayMessageReader reader(capnp_data);
            auto devmrep= reader.getRoot<riaps::devm::DevmRep>();

            // Register actor
            if (devmrep.isDeviceReg()) {
                auto msg_response = devmrep.getDeviceReg();
                auto status = msg_response.getStatus();

                if (status == Status::ERR){
                    std::cout << "Couldn't register device: " << modelName << std::endl;
                    result = false;
                }

            }

            /////
            /// Clean up
            zsock_disconnect(client, "%s", ipcAddress.c_str());
            zframe_destroy(&capnp_msgbody);
            zmsg_destroy(&msg_response);
            zsock_destroy(&client);

            return result;
        }

        bool DevmApi::UnregisterDevice(const std::string &appName, const std::string &modelName,
                                       const std::string &typeName) {

            bool result = true;

            /////
            /// Request
            capnp::MallocMessageBuilder message;
            riaps::devm::DevmReq::Builder root = message.initRoot<riaps::devm::DevmReq>();
            auto dreqBuilder = root.initDeviceUnreg();

            dreqBuilder.setAppName(appName);
            dreqBuilder.setModelName(modelName);
            dreqBuilder.setTypeName(typeName);



            auto serializedMessage = capnp::messageToFlatArray(message);

            zmsg_t* msg = zmsg_new();
            zmsg_pushmem(msg, serializedMessage.asBytes().begin(), serializedMessage.asBytes().size());

            std::string ipcAddress = riaps::framework::Configuration::GetDeviceManagerServiceIpc();
            zsock_t * client = zsock_new_req (ipcAddress.c_str());
            assert(client);

            zmsg_send(&msg, client);

            /////
            /// Response
            zmsg_t* msg_response = zmsg_recv(client);

            zframe_t* capnp_msgbody = zmsg_pop(msg_response);
            size_t    size = zframe_size(capnp_msgbody);
            byte*     data = zframe_data(capnp_msgbody);

            auto capnp_data = kj::arrayPtr(reinterpret_cast<const capnp::word*>(data), size / sizeof(capnp::word));

            capnp::FlatArrayMessageReader reader(capnp_data);
            auto devmrep= reader.getRoot<riaps::devm::DevmRep>();

            // Register actor
            if (devmrep.isDeviceUnreg()) {
                auto msg_response = devmrep.getDeviceUnreg();
                auto status = msg_response.getStatus();

                if (status == Status::ERR){
                    std::cout << "Couldn't unregister device: " << modelName << std::endl;
                    result = false;
                }

            }

            /////
            /// Clean up
            zsock_disconnect(client, "%s", ipcAddress.c_str());
            zframe_destroy(&capnp_msgbody);
            zmsg_destroy(&msg_response);
            zsock_destroy(&client);

            return result;

        }

        DevmApi::~DevmApi() {
            zsock_destroy(&_socket);
        }
    }
}

//
//zsock_t*
//RegisterApp(const std::string& appName,
//               const std::string& modelName,
//               const std::string& typeName,
//               const std::vector<std::pair<std::string, std::string>>& args) {
//    /////
//    /// Request
//    /////
//    capnp::MallocMessageBuilder message;
//    Dev::Builder dreqBuilder = message.initRoot<DiscoReq>();
//    ActorRegReq::Builder areqBuilder = dreqBuilder.initActorReg();
//
//    areqBuilder.setActorName(actorname);
//    areqBuilder.setAppName(appname);
//    areqBuilder.setVersion("0");
//    areqBuilder.setPid(::getpid());
//
//    auto serializedMessage = capnp::messageToFlatArray(message);
//
//    zmsg_t* msg = zmsg_new();
//    zmsg_pushmem(msg, serializedMessage.asBytes().begin(), serializedMessage.asBytes().size());
//
//
//    //std::string mac_address = GetMacAddressStripped();
//    std::string ipcAddress = riaps::framework::Configuration::GetDiscoveryServiceIpc();
//    zsock_t * client = zsock_new_req (ipcAddress.c_str());
//    assert(client);
//
//    zmsg_send(&msg, client);
//
//    /////
//    /// Response
//    /////
//    zmsg_t* msg_response = zmsg_recv(client);
//
//    zframe_t* capnp_msgbody = zmsg_pop(msg_response);
//    size_t    size = zframe_size(capnp_msgbody);
//    byte*     data = zframe_data(capnp_msgbody);
//
//    auto capnp_data = kj::arrayPtr(reinterpret_cast<const capnp::word*>(data), size / sizeof(capnp::word));
//
//    capnp::FlatArrayMessageReader reader(capnp_data);
//    auto msg_discoreq= reader.getRoot<DiscoRep>();
//
//    zsock_t* discovery_port = NULL;
//
//    // Register actor
//    if (msg_discoreq.isActorReg()) {
//        auto msg_response = msg_discoreq.getActorReg();
//        auto status = msg_response.getStatus();
//        auto port = msg_response.getPort();
//
//
//        // TODO: Check status
//
//        if (status == Status::ERR){
//            std::cout << "Couldn't register actor: " << actorname << std::endl;
//        }
//        else {
//            auto discovery_endpoint = "tcp://localhost:" + std::to_string(port);
//            discovery_port = zsock_new_pair(discovery_endpoint.c_str());
//            assert(discovery_port);
//        }
//    }
//
//    /////
//    /// Clean up
//    /////
//    zsock_disconnect(client, ipcAddress.c_str());
//    zframe_destroy(&capnp_msgbody);
//    zmsg_destroy(&msg_response);
//    zsock_destroy(&client);
//
//    return discovery_port;
//
//}
//
//zsock_t*
//RegisterDevice(const std::string& appName,
//               const std::string& modelName,
//               const std::string& typeName,
//               const std::vector<std::pair<std::string, std::string>>& args){
//
//
//    /////
//    /// Request
//    /////
//    capnp::MallocMessageBuilder message;
//    DevmReq::Builder root = message.initRoot<DevmReq>();
//    DeviceRegReq::Builder dreqBuilder = root.initDeviceReg();
//
//    dreqBuilder.setAppName(appName);
//    dreqBuilder.setModelName(modelName);
//    dreqBuilder.setTypeName(typeName);
//
//    if (args.size()>0) {
//        auto devArgs  = dreqBuilder.initDeviceArgs(args.size());
//
//        for (int i =0; i<args.size(); i++) {
//            DeviceArg::Builder devArg = devArgs[i];
//            devArg.setName(args[i].first);
//            devArg.setValue(args[i].second);
//        }
//    }
//
//    auto serializedMessage = capnp::messageToFlatArray(message);
//
//    zmsg_t* msg = zmsg_new();
//    zmsg_pushmem(msg, serializedMessage.asBytes().begin(), serializedMessage.asBytes().size());
//
//
//    //std::string mac_address = GetMacAddressStripped();
//    std::string ipcAddress = riaps::framework::Configuration::GetDeviceManagerServiceIpc();
//    zsock_t * client = zsock_new_req (ipcAddress.c_str());
//    assert(client);
//
//    zmsg_send(&msg, client);
//
//    /////
//    /// Response
//    /////
//    zmsg_t* msg_response = zmsg_recv(client);
//
//    zframe_t* capnp_msgbody = zmsg_pop(msg_response);
//    size_t    size = zframe_size(capnp_msgbody);
//    byte*     data = zframe_data(capnp_msgbody);
//
//    auto capnp_data = kj::arrayPtr(reinterpret_cast<const capnp::word*>(data), size / sizeof(capnp::word));
//
//    capnp::FlatArrayMessageReader reader(capnp_data);
//    auto msg_discoreq= reader.getRoot<DiscoRep>();
//
//    zsock_t* discovery_port = NULL;
//
//    // Register actor
//    if (msg_discoreq.isActorReg()) {
//        auto msg_response = msg_discoreq.getActorReg();
//        auto status = msg_response.getStatus();
//        auto port = msg_response.getPort();
//
//
//        // TODO: Check status
//
//        if (status == Status::ERR){
//            std::cout << "Couldn't register actor: " << actorname << std::endl;
//        }
//        else {
//            auto discovery_endpoint = "tcp://localhost:" + std::to_string(port);
//            discovery_port = zsock_new_pair(discovery_endpoint.c_str());
//            assert(discovery_port);
//        }
//    }
//
//    /////
//    /// Clean up
//    /////
//    zsock_disconnect(client, ipcAddress.c_str());
//    zframe_destroy(&capnp_msgbody);
//    zmsg_destroy(&msg_response);
//    zsock_destroy(&client);
//
//    return discovery_port;
//
//    //if (!msg_response){
//    //    std::cout << "No msg => interrupted" << std::endl;
//    //    return false;
//    //}
//
//
//    /*zmsg_t* msg = zmsg_new();
//    zmsg_addstr(msg, CMD_DISC_REGISTER_ACTOR);
//    zmsg_addstr(msg, actorname.c_str());
//    zsock_t * client = zsock_new_req (DISCOVERY_SERVICE_IPC);
//    assert(client);
//
//    // TODO check return value
//    zmsg_send(&msg, client);
//
//    char* msg_response = zstr_recv(client);
//
//    if (!msg_response){
//        std::cout << "No msg => interrupted" << std::endl;
//        return;
//    }
//    else{
//        free(msg_response);
//    }
//
//    zsock_destroy(&client);*/
//}