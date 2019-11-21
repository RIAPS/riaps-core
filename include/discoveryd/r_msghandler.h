#ifndef RIAPS_CORE_R_MSGHANDLER_H
#define RIAPS_CORE_R_MSGHANDLER_H

#include <const/r_const.h>
#include <groups/r_group.h>
#include <discoveryd/r_discovery_types.h>
#include <discoveryd/r_msghandler.h>
#include <discoveryd/r_dhtdata.h>

#include <opendht.h>
#include <czmq.h>
#include <spdlog_setup/conf.h>

namespace spd = spdlog;

/**
 * Discovery service. Finds other discovery services on the network.
 * Handles actor registrations, ZMQ address:port registrations from the RIAPS components.
 * Returns registered addresses, sends notification about new service addresses.
 */
namespace riaps::discovery {

        /**
         * RIAPS port type - string representation.
         */
        static const std::unordered_map<riaps::discovery::Kind, std::string> kindMap =
                {{riaps::discovery::Kind::PUB, "pub"},
                 {riaps::discovery::Kind::SUB, "sub"},
                 {riaps::discovery::Kind::CLT, "clt"},
                 {riaps::discovery::Kind::SRV, "srv"},
                 {riaps::discovery::Kind::REQ, "req"},
                 {riaps::discovery::Kind::REP, "rep"},
                 {riaps::discovery::Kind::QRY, "qry"},
                 {riaps::discovery::Kind::ANS, "ans"},
                 {riaps::discovery::Kind::GPUB, "gpub"},
                 {riaps::discovery::Kind::GSUB, "gsub"}};

        /**
         * Encapsulates all the message handlers of the discovery service.
         */
        class DiscoveryMessageHandler {
        public:

            /**
             * @param dht_node DHT instance to be used for saving/querying data
             * @param pipe ZMQ pipe for external communication
             * @param logger spdlog instance <optional>
             */
            DiscoveryMessageHandler(dht::DhtRunner &dht_node,
                                    zsock_t **pipe,
                                    std::shared_ptr<spdlog::logger> logger = nullptr);

            /**
             * Creates ZMQ ports, collects zombie addresses, reads the LMDB instance for leftover ports.
             */
            bool Init();

            /**
             * Starts the message handler thread.
             */
            void Run();

            ~DiscoveryMessageHandler();

        private:
            /**
             * Handles messages from the DHT, when new zombie service is added.
             */
            bool HandleZombieUpdate(std::vector<DhtData> &&values);

            /**
             * Handles messages from the zactor pipe (typically $TERM - terminate)
             */
            void HandlePipeMessage();

            /**
             * Handles messages from the actors
             */
            void HandleRiapsMessage();

            /**
             * Handles actor registration, creates the PIPE ZMQ channel between the actor and the discovery service.
             * @param msg_actor_req
             */
            void HandleActorReg(riaps::discovery::ActorRegReq::Reader &msg_actor_req);

            /**
             * Unregisters the actor, closes the ZMQ channel, removes all related data.
             * @param msg_actor_unreg
             */
            void HandleActorUnreg(riaps::discovery::ActorUnregReq::Reader &msg_actor_unreg);

            /**
             * Registers a service in the DHT.
             * @param msg_service_reg
             */
            void HandleServiceReg(riaps::discovery::ServiceRegReq::Reader &msg_service_reg);

            /**
             * Searches a service in the DHT, and subscribes to the new services (under the same key).
             * @param msg_service_lookup
             */
            void HandleServiceLookup(riaps::discovery::ServiceLookupReq::Reader &msg_service_lookup);

            /**
             * Registers the component in a group and subscribes to notifications about new members.
             * @param msg_group_join
             */
            void HandleGroupJoin(riaps::discovery::GroupJoinReq::Reader &msg_group_join);

            /**
             * Handler of DHT::get() results.
             * @param msg_provider_get List of values returned by the DHT
             * @param clients Active clients where the results will be forwarded.
             */
            void HandleDhtGet(const riaps::discovery::ProviderListGet::Reader &msg_provider_get,
                              const std::map<std::string, std::shared_ptr<ActorDetails>> &clients);

            /**
             * Handler of DHT::listen() results.
             * @param msg_provider_update List of values returned by the DHT
             * @param client_subscriptions Active clients where the results will be forwarded.
             */
            void HandleDhtUpdate(const riaps::discovery::ProviderListUpdate::Reader &msg_provider_update,
                                 const std::map<std::string, std::vector<std::unique_ptr<ClientDetails>>> &client_subscriptions);


            /**
             * Handles when the DHT returns new group members.
             * @param msg_group_update
             */
            void HandleDhtGroupUpdate(const riaps::discovery::GroupUpdate::Reader &msg_group_update);

            /**
             * Forwards data from the DHT thread into the Discovery thread.
             * @param values The values to be forwarded.
             */
            void PushDhtValuesToDisco(std::vector<DhtData> &&values);

            /**
             * Waits until the DHT is not stable.
             * @return True, when the DHT is stable.
             */
            std::future<bool> WaitForDht();

            /**
             * Builds a DHT key for dht::put() operations.
             */
            const std::tuple<const std::string, const std::string> BuildInsertKeyValuePair(
                    const std::string &app_name,
                    const std::string &msg_type,
                    const riaps::discovery::Kind &kind,
                    const riaps::discovery::Scope &scope,
                    const std::string &host,
                    const uint16_t port);

            /**
             * Builds a DHT key for dht::get() operations.
             */
            const std::pair<const std::string, const std::string> BuildLookupKey(
                    const std::string &app_name,
                    const std::string &msg_type,
                    const riaps::discovery::Kind &kind,
                    const riaps::discovery::Scope &scope,
                    const std::string &client_actor_host,
                    const std::string &client_actor_name,
                    const std::string &client_instance_name,
                    const std::string &client_port_name);

            /**
             * Renews the registration of a service in the DHT. (Before the value expires in the DHT, by default it is 10 minutes).
             */
            void RenewServices();

            /**
             * Removes zombie addresses from the local chace after 15 minutes.
             */
            void MaintainZombieList();

            /**
             * Removes all info objects from the cache variables. DHT is untouched, let the data expired.
             */
            int DeregisterActor(const std::string &app_name,
                                const std::string &actor_name);

            /**
             * Host address. DHT keys for non-local RIAPS ports are partially derived from the host address.
             */
            std::string host_address_;

            /**
             * MAX address. DHT keys for local RIAPS ports are partially derived from the host address.
             */
            std::string mac_address_;



            /**
             * Saves a byte array into DHT.
             * @param key Data is saved under key.
             * @param data The data to be saved in bytes.
             */
            void DhtPut(const std::string &key, std::vector<uint8_t> &data);

            /**
             * Saves a byte array into DHT.
             * @param keyhash Hash of the key where the data will be saved.
             * @param data The data to be saved in bytes.
             */
            void DhtPut(dht::InfoHash &keyhash, std::vector<uint8_t> &data);

            /**
             * Queryes data from the DHT.
             * @param lookup_key Key
             * @param client_details Info object of the actor
             * @param call_level Current level of the call. If the call fails it calls itself recursively.
             */
            void DhtGet(const std::string lookup_key, ClientDetails client_details, uint8_t call_level);

            /**
             * Timestamp of the last renewal in the DHT.
             */
            int64_t last_service_renewal_;

            /**
             * Timestamp of the last renewal in the DHT.
             */
            int64_t last_zombie_renewal_;

            /**
             * Timestamp of the last service renewal in the DHT.
             */
            const uint16_t service_renewal_period_;

            /**
             * Timestamp of the last zombie service renewal in the DHT.
             */
            const uint16_t zombie_renewal_period_;

            /**
             * Key of the zombie addresses (not local address)
             */
            const std::string zombieglobalkey_;

            /**
             * Key of the zombie addresses (local addresses)
             */
            const std::string zombielocalkey_;

            /**
             * DHT node instance.
             */
            dht::DhtRunner &dht_node_;

            /**
             * Logger instance.
             */
            std::shared_ptr<spdlog::logger> logger_;

            /**
             * ZMQ socket for DHT communication
             */
            zsock_t *dht_update_socket_;

            /**
             * ZMQ socket for actor communication
             */
            zsock_t *riaps_socket_;

            /**
             * ZMQ poller for all incoming sockets of the discovery.
             */
            zpoller_t *poller_;

            /**
             * ZMQ socket for communication with the outter shell.
             */
            zsock_t *pipe_;

            /**
             * DHT tracker thread, checks the DHT stability.
             */
            zactor_t *dht_tracker_;

            /**
             * Latest ZMQ frame for the router-like communication.
             */
            std::shared_ptr<zframe_t> rep_identity_;

            /**
             * Message loop keeps running until true;
             */
            bool terminated_;

            /**
             * Stores pair sockets for actor communication
             */
            std::map<std::string, std::shared_ptr<ActorDetails>> clients_;


            /**
             * Stores addresses of zombie services.
             * A service is zombie, if the related socket is not able to respond, but it is still in the DHT
             * The int64 argument is a timestamp. Old zombies are removed from the set after 10 minutes.
             */
            std::unordered_map<std::string, int64_t> zombie_services_; // TODO: replace int64_t => riaps::TimeOut

            /**
             * Client subscriptions to messageTypes
             */
            std::map<std::string, std::vector<std::unique_ptr<ClientDetails>>> client_subscriptions_;

            /**
             * Subscribers for group changes
             * AppName - future<>
             */
            std::map<std::string, std::future<size_t>> group_listeners_;

            /**
             * Registered OpenDHT listeners. Every key can be registered only once.
             */
            std::map<std::string, std::future<size_t>> registered_listeners_;

            // Registered services, with PID-s. We are using this local cache for renew services in the OpenDHT.
            // Checking the registered services in every 20th seconds.
            std::map<pid_t, std::vector<std::unique_ptr<ServiceCheckins>>> service_checkins_;

            /**
             * Details of one group in the local cache.
             */
            struct RegisteredGroup {

                /**
                 * The key of the group in the DHT
                 */
                std::string groupKey;

                /**
                 * Registered ports of the groupinstance.
                 */
                riaps::groups::GroupDetails services;

                /**
                 * Actor PID.
                 */
                pid_t actorPid;

                /**
                 * Time of the insertion.
                 */
                Timeout<std::chrono::minutes> timeout;
            };

            /**
             * Cache of the locally running actors - groupinstance mapping.
             * The actor is identified by its pid.
             * TODO: Replace PID by something else. ActorName?
             */
            std::unordered_map<pid_t, std::vector<std::shared_ptr<RegisteredGroup>>> group_services_;


            /**
             * True if the security is turned on in riaps.conf.
             */
            bool has_security_;

            /**
             * The private key to be used DHT data encryption.
             */
            std::shared_ptr<dht::crypto::PrivateKey> private_key_;
        };
}

#endif //RIAPS_CORE_R_MSGHANDLER_H
