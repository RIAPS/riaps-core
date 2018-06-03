/**
 * Message handler functions for the commands of discovery service. Uses capnp message buffers,
 * the messages are defined in disco.capnp
 *
 * The discovery service listens for incoming connections on ASYNC REP port (ZMQ ROUTER), on the following addres:
 * @code riaps::framework::Configuration::GetDiscoveryServiceIpc().
 *
 *
 *
 * @author Istvan Madari
 */


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
        bool init();
        void run();
        ~DiscoveryMessageHandler();
    private:
        bool handleZombieUpdate (const std::vector<std::shared_ptr<dht::Value>> &values);

        /**
         * Handles messages from the zactor pipe (typically $TERM - terminate)
         */
        void handlePipeMessage  ();

        /**
         * Handles messages from the actors
         */
        void handleRiapsMessage ();

        /**
         * Handles actor registration, creates the PIPE ZMQ channel between the actor and the discovery service.
         * @param msgActorReq
         */
        void handleActorReg     (riaps::discovery::ActorRegReq::Reader     & msgActorReq);

        /**
         * Unregisters the actor, closes the ZMQ channel, removes all related data.
         * @param msgActorUnreg
         */
        void handleActorUnreg   (riaps::discovery::ActorUnregReq::Reader   & msgActorUnreg);

        /**
         * Registers a service in the DHT.
         * @param msgServiceReg
         */
        void handleServiceReg   (riaps::discovery::ServiceRegReq::Reader   & msgServiceReg);

        /**
         * Searches a service in the DHT, and subscribes to the new services (under the same key).
         * @param msgServiceLookup
         */
        void handleServiceLookup(riaps::discovery::ServiceLookupReq::Reader& msgServiceLookup);

        /**
         * Registers the component in a group and subscribes to notifications about new members.
         * @param msgGroupJoin
         */
        void handleGroupJoin    (riaps::discovery::GroupJoinReq::Reader    & msgGroupJoin);

        /**
         *
         * @param msgProviderGet
         * @param clients
         */
        void handleDhtGet(const riaps::discovery::ProviderListGet::Reader& msgProviderGet,
                          const std::map<std::string, std::shared_ptr<ActorDetails>>& clients);

        void handleDhtUpdate(const riaps::discovery::ProviderListUpdate::Reader&                          msgProviderUpdate,
                          const std::map<std::string, std::vector<std::unique_ptr<ClientDetails>>>& clientSubscriptions);

        void handleDhtGroupUpdate(const riaps::discovery::GroupUpdate::Reader& msgGroupUpdate);

        void pushDhtValuesToDisco(std::vector<std::shared_ptr<dht::Value>> values);

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



        void maintainRenewalDepricated();
        void maintainRenewal();
        void maintainZombieList();
        int deregisterActor(const std::string& appName,
                            const std::string& actorName);

        std::string m_macAddress;
        std::string m_hostAddress;


        void dhtPut(dht::InfoHash keyhash, const std::string key, std::vector<uint8_t> data, uint8_t callLevel);
        void dhtGet(const std::string lookupKey, ClientDetails clientDetails, uint8_t callLevel);

        int64_t m_lastServiceCheckin;
        int64_t m_lastZombieCheck;

        const uint16_t m_serviceCheckPeriod;
        const uint16_t m_zombieCheckPeriod;

        // Key where the DHT stores zombie nodes
        const std::string m_zombieKey;// = "/zombies";

        dht::DhtRunner& m_dhtNode;

        std::shared_ptr<spdlog::logger> m_logger;

        /**
         * ZMQ socket for DHT communication
         */
        zsock_t* m_dhtUpdateSocket;

        /**
         * ZMQ socket for actor communication
         */
        zsock_t*   m_riapsSocket;

        zpoller_t* m_poller;
        zsock_t*   m_pipe;

        std::shared_ptr<zframe_t> m_repIdentity;

        bool m_terminated;

        // Stores pair sockets for actor communication
        std::map<std::string, std::shared_ptr<ActorDetails>> m_clients;

        // Stores addresses of zombie services
        // A service is zombie, if the related socket is not able to respond, but it is still in the DHT
        // The int64 argument is a timestamp. Old zombies are removed from the set after 10 minutes.
        std::map<std::string, int64_t> m_zombieServices;

        // Client subscriptions to messageTypes
        std::map<std::string, std::vector<std::unique_ptr<ClientDetails>>> m_clientSubscriptions;

        // Subscribe for group changes
        // AppName - future<>
        std::map<std::string, std::future<size_t>> m_groupListeners;

        // Registered OpenDHT listeners. Every key can be registered only once.
        std::map<std::string, std::future<size_t>> m_registeredListeners;

        // Registered services, with PID-s. We are using this local cache for renew services in the OpenDHT.
        // Checking the registered services in every 20th seconds.
        std::map<pid_t, std::vector<std::unique_ptr<ServiceCheckins>>> m_serviceCheckins;

        struct RegisteredGroup {
            std::string groupKey;
            riaps::groups::GroupDetails services;
            pid_t actorPid;
            Timeout<std::ratio<60>> timeout; // std::ratio<60> -> minutes
        };

        std::unordered_map<pid_t, std::vector<std::shared_ptr<RegisteredGroup>>> m_groupServices;
    };
}

#endif //RIAPS_CORE_R_MSGHANDLER_H
