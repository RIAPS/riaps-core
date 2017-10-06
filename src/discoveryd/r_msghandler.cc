//
// Created by istvan on 10/6/17.
//

#include <discoveryd/r_msghandler.h>
#include <discoveryd/r_riaps_cmd_handler.h>
#include <framework/rfw_configuration.h>
#include <discoveryd/r_discoveryd_commands.h>

namespace riaps{
    DiscoveryMessageHandler::DiscoveryMessageHandler(dht::DhtRunner &dhtNode, zsock_t* pipe)
        : _dhtNode(dhtNode),
          _pipe(pipe),
          _serviceCheckPeriod(20000), // 20 sec in in msec.
          _zombieCheckPeriod(600000), // 10 min in msec
          _zombieKey("/zombies"),
          _terminated(false),
          _dhtUpdateSocket(nullptr){

    }

    bool DiscoveryMessageHandler::Init() {


        _dhtUpdateSocket = zsock_new_pull(DHT_ROUTER_CHANNEL);

        _riapsSocket = zsock_new_rep (riaps::framework::Configuration::GetDiscoveryServiceIpc().c_str());
        int lingerValue = 0;
        int sendtimeout = 0; // 0 - returns immediately with EAGAIN if the message cannot be sent
        zmq_setsockopt(_riapsSocket, ZMQ_LINGER, &lingerValue, sizeof(int));
        zmq_setsockopt(_riapsSocket, ZMQ_SNDTIMEO, &sendtimeout, sizeof(int));

        zpoller_new(_pipe, _riapsSocket, _dhtUpdateSocket, NULL);

        zsock_signal (_pipe, 0);

        _lastServiceCheckin = zclock_mono();
        _lastZombieCheck  = zclock_mono();

        // Get current zombies, and listen to new zombies
        _dhtNode.get(_zombieKey, handleZombieUpdate);

        // Subscribe for further zombies
        _dhtNode.listen(_zombieKey, handleZombieUpdate);
    }



    void DiscoveryMessageHandler::Run() {
        while (!_terminated){
            void *which = zpoller_wait(_poller, REGULAR_MAINTAIN_PERIOD);

            // Check services whether they are still alive.
            // Reregister the too old services (OpenDHT ValueType settings, it is 10 minutes by default)
            int64_t loopStartTime = zclock_mono();
            if ((loopStartTime-_lastServiceCheckin) > _serviceCheckPeriod){
                maintainRenewal();
            }

            // Check outdated zombies
            if ((loopStartTime-_lastZombieCheck) > _zombieCheckPeriod) {
                maintainZombieList();
            }

            // Handling messages from the caller (e.g.: $TERM$)
            if (which == pipe) {
                _terminated = handlePipeMessage((zsock_t*)which, _dhtNode);
            }
            else if (which == _dhtUpdateSocket){
                // Process the updated nodes
                zmsg_t* msgResponse = zmsg_recv(_dhtUpdateSocket);

                zframe_t* capnpMsgBody = zmsg_pop(msgResponse);
                size_t    size = zframe_size(capnpMsgBody);
                byte*     data = zframe_data(capnpMsgBody);


                try {
                    auto capnp_data = kj::arrayPtr(reinterpret_cast<const capnp::word *>(data), size / sizeof(capnp::word));

                    capnp::FlatArrayMessageReader reader(capnp_data);
                    auto msgDhtUpdate = reader.getRoot<riaps::discovery::DhtUpdate>();

                    // If update
                    if (msgDhtUpdate.isProviderUpdate()) {
                        riaps::discovery::ProviderListUpdate::Reader msgProviderUpdate = msgDhtUpdate.getProviderUpdate();
                        handleUpdate(msgProviderUpdate, _clientSubscriptions, _clients);

                    } else if (msgDhtUpdate.isProviderGet()) {
                        riaps::discovery::ProviderListGet::Reader msgProviderGet = msgDhtUpdate.getProviderGet();
                        handleGet(msgProviderGet, _clients);
                    } else if (msgDhtUpdate.isZombieList()) {
                        auto zombieList = msgDhtUpdate.getZombieList();
                        int s = zombieList.size();
                        for (int i =0; i< zombieList.size(); i++){
                            std::string currentZombie = zombieList[i];
                            _zombieServices[currentZombie] = zclock_mono();
                        }
                    }

                    zframe_destroy(&capnpMsgBody);
                    zmsg_destroy(&msgResponse);
                }
                catch(kj::Exception& e){
                    std::cout << "Couldn't deserialize message from DHT_ROUTER_SOCKET" << std::endl;
                    continue;

                }
            }

                // Handling messages from the RIAPS FW
                // Discovery service commands
            else if(which == _riapsSocket){
                _terminated = handleRiapsMessages((zsock_t*)which,
                                                 clients,
                                                 serviceCheckins,
                                                 clientSubscriptions,
                                                 registeredListeners,
                                                 host_address,
                                                 mac_address,
                                                 zombieServices,
                                                 dhtNode);
            }
            else {


                //auto outdateds = maintain_servicecache(service_checkins);


            }
        }
    }

    // Handle ZMQ messages, arriving on the zactor PIPE
    void DiscoveryMessageHandler::handlePipeMessage(){
        zmsg_t* msg = zmsg_recv(_pipe);
        if (!msg){
            std::cout << "No msg => interrupted" << std::endl;
            _terminated = true;
        }
        else {

            char *command = zmsg_popstr(msg);

            if (streq(command, "$TERM")) {
                std::cout << std::endl << "$TERMINATE arrived, discovery service is stopping..." << std::endl;
                _terminated = true;
            } else if (streq(command, CMD_JOIN)) {
                std::cout << "New peer on the network. Join()" << std::endl;
                bool has_more_msg = true;

                while (has_more_msg) {
                    char *newhost = zmsg_popstr(msg);
                    if (newhost) {
                        std::cout << "Join to: " << newhost << std::endl;
                        std::string str_newhost(newhost);
                        dhtJoinToCluster(str_newhost, RIAPS_DHT_NODE_PORT, dhtNode);
                        zstr_free(&newhost);
                    } else {
                        has_more_msg = false;
                    }
                }
            }

            if (command) {
                zstr_free(&command);
            }
            zmsg_destroy(&msg);
        }
    }

    void DiscoveryMessageHandler::maintainRenewal(){

        std::vector<pid_t> toBeRemoved;
        for (auto it= _serviceCheckins.begin(); it!=_serviceCheckins.end(); it++){
            // Check pid, mark the removable pids
            // std::cout << "checking PID " << it->first << std::endl;
            if (!kill(it->first,0)==0){
                toBeRemoved.push_back(it->first);
            }
        }

        // Remove killed PIDs
        for (auto it = toBeRemoved.begin(); it!=toBeRemoved.end(); it++){
            std::cout << "Remove services with PID: " << *it << std::endl;

            // Put the service address into the zombies list in DHT
            for (auto serviceIt  = _serviceCheckins[*it].begin();
                 serviceIt != _serviceCheckins[*it].end();
                 serviceIt++) {

                // host:port
                std::string serviceAddress = (*serviceIt)->value;
                std::vector<uint8_t> opendht_data(serviceAddress.begin(), serviceAddress.end());
                _dhtNode.put(_zombieKey, dht::Value(opendht_data));
            }

            _serviceCheckins.erase(*it);
        }

        // Renew too old services
        int64_t now = zclock_mono();
        for (auto pidIt= _serviceCheckins.begin(); pidIt!=_serviceCheckins.end(); pidIt++){
            for(auto serviceIt = pidIt->second.begin(); serviceIt!=pidIt->second.end(); serviceIt++){

                // Renew
                if (now - (*serviceIt)->createdTime > (*serviceIt)->timeout){
                    (*serviceIt)->createdTime = now;

                    // Reput key-value
                    std::vector<uint8_t> opendht_data((*serviceIt)->value.begin(), (*serviceIt)->value.end());
                    auto keyhash = dht::InfoHash::get((*serviceIt)->key);

                    _dhtNode.put(keyhash, dht::Value(opendht_data));
                }
            }
        }
    }

    void DiscoveryMessageHandler::maintainZombieList(){
        int64_t currentTime = zclock_mono();

        for (auto it = _zombieServices.begin(); it!=_zombieServices.end(); it++){
            // 10 min timeout
            if ((currentTime - it->second) > 60*10*1000) {
                std::cout << "Purge zombie from cache: " << it->first << std::endl;
                it = _zombieServices.erase(it);
            }
        }
    }

    DiscoveryMessageHandler::~DiscoveryMessageHandler() {
        zpoller_destroy(&_poller);
        zsock_destroy(&_dhtUpdateSocket);
        zsock_destroy(&_riapsSocket);
        sleep(1);

        for (auto it_client = _clients.begin(); it_client!=_clients.end(); it_client++){
            if (it_client->second->socket!=NULL) {
                zsock_destroy(&it_client->second->socket);
                it_client->second->socket=NULL;
            }
        }
        sleep(1);
    }
}