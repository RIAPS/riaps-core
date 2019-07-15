#ifndef RIAPS_CORE_R_VALIDATOR_H
#define RIAPS_CORE_R_VALIDATOR_H

#include <utils/r_timeout.h>
#include <utils/r_utils.h>
#include <framework/rfw_configuration.h>

#include <czmq.h>
#include <opendht.h>
#include <spdlog/spdlog.h>

#include <string>
#include <vector>
#include <algorithm>
#include <memory>
#include <map>

namespace spd = spdlog;

namespace riaps::discovery {

    /**
     * Details of remote validator.
     */
    struct node_info {

        /**
         * Request channel to the remote validator.
         */
        std::shared_ptr<zsock_t> request;

        /**
         * Stores when the remote node was checked last time.
         */
        riaps::utils::Timeout<std::chrono::seconds> check_period;

        /**
         * Is the node validated or not (in #check_period).
         */
        bool validated;
    };

    void DestroySocket(zsock_t *socket);

    /**
     * Validates remote discovery services, based on their IP.
     */
    class DiscoveryValidator {
    public:
        DiscoveryValidator(bool has_security);

        /**
         * Initializes the validator.
         * @param ip_address IP address where the validator accepts incoming requests from other validators to validate itself.
         * @param private_key Messages of the validation process are encrypted and signed with this key and the corresponding public key.
         * @return True if the validator is successfully initialized.
         */
        bool InitWithSecurity(const std::string &ip_address, std::shared_ptr<dht::crypto::PrivateKey> &private_key);

        /**
         * The validator doesn't initiate the validation process, just check the ip address well-formedness.
         * @return True if the validator is successfully initialized.
         */
        bool InitNoSecurity();

        /**
         * validates the remote node.
         * @param node_address Remote node address, where the other validator runs.
         * @return True if the remote node successfully validated itself.
         */
        bool IsValid(const std::string &node_address);

        /**
         * Reply to all validation requests.
         */
        void ReplyAll();

        /**
         * Address of the port where the validator accepts requests.
         * @return IP address, port
         */
        const std::string &validator_address() {
            return validator_address_;
        }

        /**
         * @return The ZMQ socket of the validator.
         */
        zsock_t *validator_socket() {
            return validator_socket_.get();
        }

        ~DiscoveryValidator();

    private:

        /**
         * Starts the valiadation of the remote node.
         * @param node_address Remote validator address
         */
        void Validate(const std::string &node_address);

        /**
         * Creates a new remote node info object.
         * @param node_address Remote node address.
         * @return Info object about the remote node.
         */
        std::shared_ptr<node_info> CreateNode(const std::string &node_address);

        bool IsUptodate(const std::string &node_address);

        /**
         * Removes the node info where the check_period is timed out.
         */
        void DeleteOldNodes();


        /**
         * Checks if the given address string is a well-formed address.
         * @param zmq_address The address to be validated.
         * @return True if the #zmq_address is well-formed.
         */
        bool IsValidZmqAddress(const std::string &zmq_address);


        std::shared_ptr<spd::logger> logger_;
        std::shared_ptr<zpoller_t> poller_;
        std::shared_ptr<zsock_t> validator_socket_;
        std::string validator_address_;
        std::shared_ptr<dht::crypto::PrivateKey> private_key_;

        /**
         * Last run of the cache maintain function: DeleteOldNodes()
         */
        riaps::utils::Timeout<std::chrono::minutes> last_delete_;

        /**
         * Cache of the checked nodes.
         */
        std::map<std::string, std::shared_ptr<node_info>> cluster_nodes_;

        /**
         * Signature to be used in the validation process.
         */
        dht::Blob signature_;
        bool has_security_;
    };
}

#endif //RIAPS_CORE_R_VALIDATOR_H
