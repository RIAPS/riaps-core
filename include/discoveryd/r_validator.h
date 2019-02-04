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

struct node_info {
    std::shared_ptr<zsock_t>                    request;
    riaps::utils::Timeout<std::chrono::seconds> check_period;
    bool                                        validated;
};

void DestroySocket(zsock_t* socket);

class DiscoveryValidator {
public:
    DiscoveryValidator(bool has_security);
    bool InitWithSecurity(const std::string& ip_address, std::shared_ptr<dht::crypto::PrivateKey>& private_key);
    bool InitNoSecurity();

    bool IsValid(const std::string& node_address);
    void ReplyAll();

    const std::string& validator_address() {
        return validator_address_;
    }

    zsock_t* validator_socket() {
        return validator_socket_.get();
    }

    ~DiscoveryValidator();

private:
    void Validate(const std::string& node_address);
    std::shared_ptr<node_info> CreateNode(const std::string& node_address);
    bool IsUptodate(const std::string& node_address);
    void DeleteOldNodes();


    std::shared_ptr<spd::logger>                      logger_;
    std::shared_ptr<zpoller_t>                        poller_;
    std::shared_ptr<zsock_t>                          validator_socket_;
    std::string                                       validator_address_;
    std::shared_ptr<dht::crypto::PrivateKey>          private_key_;
    riaps::utils::Timeout<std::chrono::minutes>       last_delete_;
    std::map<std::string, std::shared_ptr<node_info>> cluster_nodes_;
    dht::Blob                                         signature_;
    bool                                              has_security_;
};

#endif //RIAPS_CORE_R_VALIDATOR_H
