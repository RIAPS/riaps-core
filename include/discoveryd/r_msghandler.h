//
// Created by istvan on 10/6/17.
//

#ifndef RIAPS_CORE_R_MSGHANDLER_H
#define RIAPS_CORE_R_MSGHANDLER_H

#include <groups/r_group.h>
#include <discoveryd/r_discovery_types.h>
#include <discoveryd/r_registration.h>

#include <opendht.h>
#include <czmq.h>
#include <spdlog/spdlog.h>

#define REGULAR_MAINTAIN_PERIOD 3000 //msec
#define CMD_JOIN "JOIN"
#define RIAPS_DHT_NODE_PORT 4222

#define DHT_ROUTER_CHANNEL "ipc:///tmp/dhtrouterchannel"

static std::map<riaps::discovery::Kind, std::string> kindMap =
        {{riaps::discovery::Kind::PUB, "pub"},
         {riaps::discovery::Kind::SUB, "sub"},
         {riaps::discovery::Kind::CLT, "clt"},
         {riaps::discovery::Kind::SRV, "srv"},
         {riaps::discovery::Kind::REQ, "req"},
         {riaps::discovery::Kind::REP, "rep"},
         {riaps::discovery::Kind::QRY, "qry"},
         {riaps::discovery::Kind::ANS, "ans"}};

namespace spd = spdlog;

namespace riaps{
    class DiscoveryMessageHandler{
    public:
        DiscoveryMessageHandler(dht::DhtRunner& dhtNode, zsock_t** pipe, std::shared_ptr<spdlog::logger> logger = nullptr);
        bool Init();
        void Run();
        ~DiscoveryMessageHandler();
    private:
        bool handleZombieUpdate (const std::vector<std::shared_ptr<dht::Value>> &values);
        void handlePipeMessage  ();
        void handleRiapsMessage ();
        void handleActorReg     (riaps::discovery::ActorRegReq::Reader     & msgActorReq);
        void handleActorUnreg   (riaps::discovery::ActorUnregReq::Reader   & msgActorUnreg);
        void handleServiceReg   (riaps::discovery::ServiceRegReq::Reader   & msgServiceReg);
        void handleServiceLookup(riaps::discovery::ServiceLookupReq::Reader& msgServiceLookup);
        void handleGroupJoin    (riaps::discovery::GroupJoinReq::Reader    & msgGroupJoin);

        void handleDhtGet(const riaps::discovery::ProviderListGet::Reader& msgProviderGet,
                          const std::map<std::string, std::shared_ptr<actor_details_t>>& clients);

        void handleDhtUpdate(const riaps::discovery::ProviderListUpdate::Reader&                          msgProviderUpdate,
                          const std::map<std::string, std::vector<std::unique_ptr<client_details_t>>>& clientSubscriptions);

        void handleDhtGroupUpdate(const riaps::discovery::GroupUpdate::Reader& msgGroupUpdate);

        void PushDhtValuesToDisco(std::vector<std::shared_ptr<dht::Value>> values);

        std::tuple<std::string, std::string> buildInsertKeyValuePair(
                const std::string&             appName,
                const std::string&             msgType,
                const riaps::discovery::Kind&  kind,
                const riaps::discovery::Scope& scope,
                const std::string&             host,
                const uint16_t                 port);

        std::pair<std::string, std::string> buildLookupKey(
                const std::string&             appName,
                const std::string&             msgType,
                const riaps::discovery::Kind&  kind,
                const riaps::discovery::Scope& scope,
                const std::string&             clientActorHost,
                const std::string& clientActorName,
                const std::string& clientInstanceName,
                const std::string& clientPortName);



        void maintainRenewal();
        void maintainZombieList();
        int deregisterActor(const std::string& appName,
                            const std::string& actorName);

        std::string _macAddress;
        std::string _hostAddress;


        int64_t _lastServiceCheckin;
        int64_t _lastZombieCheck;

        const uint16_t _serviceCheckPeriod;
        const uint16_t _zombieCheckPeriod;

        // Key where the DHT stores zombie nodes
        const std::string _zombieKey;// = "/zombies";

        dht::DhtRunner& _dhtNode;

        std::shared_ptr<spdlog::logger> _logger;

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
        //std::map<std::string, std::unique_ptr<actor_details_t>> _clients;
        std::map<std::string, std::shared_ptr<actor_details_t>> _clients;

        // TODO: zombieServices is not thread safe, todo implement a threadsafe wrapper
        // Stores addresses of zombie services
        // A service is zombie, if the related socket is not able to respond, but it is still in the DHT
        // The int64 argument is a timestamp. Old zombies are removed from the set after 10 minutes.
        std::map<std::string, int64_t> _zombieServices;

        // Client subscriptions to messageTypes
        std::map<std::string, std::vector<std::unique_ptr<client_details_t>>> _clientSubscriptions;

        // Subscribe for group changes
        // AppName - future<>
        std::map<std::string, std::future<size_t>> _groupListeners;

        // Registered OpenDHT listeners. Every key can be registered only once.
        std::map<std::string, std::future<size_t>> _registeredListeners;

        // Registered services, with PID-s. We are using this local cache for renew services in the OpenDHT.
        // Checking the registered services in every 20th seconds.
        std::map<pid_t, std::vector<std::unique_ptr<service_checkins_t>>> _serviceCheckins;

    };
}

#endif //RIAPS_CORE_R_MSGHANDLER_H
