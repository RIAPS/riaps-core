/**
 * Message handler functions for the commands of discovery service. Uses capnp, messages are defined in disco.capnp
 *
 * The discovery service listens for incoming connections on ASYNC REP port (ZMQ ROUTER), on the following addres:
 * @code riaps::framework::Configuration::GetDiscoveryServiceIpc().
 *
 *
 * @author Istvan Madari
 */


#ifndef RIAPS_CORE_R_MSGHANDLER_H
#define RIAPS_CORE_R_MSGHANDLER_H

#include <const/r_const.h>
#include <groups/r_group.h>
#include <discoveryd/r_discovery_types.h>
#include <discoveryd/r_registration.h>
#include <discoveryd/r_msghandler.h>
#include <discoveryd/r_dhtdata.h>

#include <opendht.h>
#include <czmq.h>
#include <spdlog_setup/conf.h>



static const std::map<riaps::discovery::Kind, std::string> kindMap =
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
        DiscoveryMessageHandler(dht::DhtRunner& dhtNode,
                                zsock_t** pipe,
                                std::shared_ptr<spdlog::logger> logger = nullptr);
        bool init();
        void run();
        ~DiscoveryMessageHandler();
    private:
        bool HandleZombieUpdate (std::vector<DhtData>&& values);

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
        void HandleGroupJoin    (riaps::discovery::GroupJoinReq::Reader    & msgGroupJoin);

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

        void PushDhtValuesToDisco(std::vector<DhtData>&& values);

        std::future<bool> waitForDht();

        const std::tuple<const std::string, const std::string> buildInsertKeyValuePair(
                const std::string&             appName,
                const std::string&             msgType,
                const riaps::discovery::Kind&  kind,
                const riaps::discovery::Scope& scope,
                const std::string&             host,
                const uint16_t                 port);

        const std::pair<const std::string, const std::string> buildLookupKey(
                const std::string&             appName,
                const std::string&             msgType,
                const riaps::discovery::Kind&  kind,
                const riaps::discovery::Scope& scope,
                const std::string&             clientActorHost,
                const std::string& clientActorName,
                const std::string& clientInstanceName,
                const std::string& clientPortName);

        void RenewServices();
        void MaintainZombieList();
        int deregisterActor(const std::string& appName,
                            const std::string& actorName);

        std::string mac_address_;
        std::string host_address_;


        void DhtPut(const std::string& key, std::vector<uint8_t>& data);
        void DhtPut(dht::InfoHash& keyhash, std::vector<uint8_t>& data);
        void DhtGet(const std::string lookupKey, ClientDetails clientDetails, uint8_t callLevel);

        int64_t last_service_checkin_;
        int64_t last_zombie_check_;

        const uint16_t service_check_period_;
        const uint16_t zombie_check_period_;

        // Key where the DHT stores zombie nodes
        const std::string zombieglobalkey_;// = "/zombies";
        const std::string zombielocalkey_;

        dht::DhtRunner& dht_node_;

        std::shared_ptr<spdlog::logger> logger_;

        /**
         * ZMQ socket for DHT communication
         */
        zsock_t* dht_update_socket_;

        /**
         * ZMQ socket for actor communication
         */
        zsock_t*   riaps_socket_;

        zpoller_t* poller_;
        zsock_t*   pipe_;

        zactor_t* dht_tracker_;

        std::shared_ptr<zframe_t> rep_identity_;

        bool terminated_;

        // Stores pair sockets for actor communication
        std::map<std::string, std::shared_ptr<ActorDetails>> clients_;

        // Stores addresses of zombie services
        // A service is zombie, if the related socket is not able to respond, but it is still in the DHT
        // The int64 argument is a timestamp. Old zombies are removed from the set after 10 minutes.
        std::map<std::string, int64_t> zombie_services_;

        // Client subscriptions to messageTypes
        std::map<std::string, std::vector<std::unique_ptr<ClientDetails>>> client_subscriptions_;

        // Subscribe for group changes
        // AppName - future<>
        std::map<std::string, std::future<size_t>> group_listeners_;

        // Registered OpenDHT listeners. Every key can be registered only once.
        std::map<std::string, std::future<size_t>> registered_listeners_;

        // Registered services, with PID-s. We are using this local cache for renew services in the OpenDHT.
        // Checking the registered services in every 20th seconds.
        std::map<pid_t, std::vector<std::unique_ptr<ServiceCheckins>>> service_checkins_;

        struct RegisteredGroup {
            std::string groupKey;
            riaps::groups::GroupDetails services;
            pid_t actorPid;
            Timeout<std::chrono::minutes> timeout;
        };

        std::unordered_map<pid_t, std::vector<std::shared_ptr<RegisteredGroup>>> group_services_;

        bool has_security_;
        std::shared_ptr<dht::crypto::PrivateKey> private_key_;
    };
}

#endif //RIAPS_CORE_R_MSGHANDLER_H
