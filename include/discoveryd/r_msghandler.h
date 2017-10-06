//
// Created by istvan on 10/6/17.
//

#ifndef RIAPS_CORE_R_MSGHANDLER_H
#define RIAPS_CORE_R_MSGHANDLER_H

#include <groups/r_group.h>
#include <discoveryd/r_discovery_types.h>

#include <opendht.h>
#include <czmq.h>

#define REGULAR_MAINTAIN_PERIOD 3000 //msec

namespace riaps{
    class DiscoveryMessageHandler{
    public:
        DiscoveryMessageHandler(dht::DhtRunner& dhtNode, zsock_t* pipe);
        bool Init();
        void Run();
        ~DiscoveryMessageHandler();
    private:
        bool handleZombieUpdate(const std::vector<std::shared_ptr<dht::Value>> &values);
        void handlePipeMessage();
        void maintainRenewal();
        void maintainZombieList();


        int64_t _lastServiceCheckin;
        int64_t _lastZombieCheck;
        const uint16_t _serviceCheckPeriod;
        const uint16_t _zombieCheckPeriod;

        // Key where the DHT stores zombie nodes
        const std::string _zombieKey;// = "/zombies";

        dht::DhtRunner& _dhtNode;

        /**
         * ZMQ socket for DHT communication
         */
        zsock_t* _dhtUpdateSocket;

        /**
         * ZMQ socket for actor communication
         */
        zsock_t*   _riapsSocket;

        zpoller_t* _poller;
        zsock_t*   _pipe;

        bool _terminated;

        // Stores pair sockets for actor communication
        std::map<std::string, std::unique_ptr<actor_details_t>> _clients;

        // TODO: zombieServices is not thread safe, todo implement a threadsafe wrapper
        // Stores addresses of zombie services
        // A service is zombie, if the related socket is not able to respond, but it is still in the DHT
        // The int64 argument is a timestamp. Old zombies are removed from the set after 10 minutes.
        std::map<std::string, int64_t> _zombieServices;

        // Client subscriptions to messageTypes
        std::map<std::string, std::vector<std::unique_ptr<client_details_t>>> _clientSubscriptions;

        // Group subscriptions
        std::map<riaps::groups::GroupId, std::vector<std::unique_ptr<client_details_t>>> _groupSubscriptions;

        // Registered OpenDHT listeners. Every key can be registered only once.
        std::map<std::string, std::future<size_t>> _registeredListeners;

        // Registered services, with PID-s. We are using this local cache for renew services in the OpenDHT.
        // Checking the registered services in every 20th seconds.
        std::map<pid_t, std::vector<std::unique_ptr<service_checkins_t>>> _serviceCheckins;

    };
}

#endif //RIAPS_CORE_R_MSGHANDLER_H
