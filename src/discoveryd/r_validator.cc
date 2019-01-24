//
// Created by istvan on 1/22/19.
//

#include <discoveryd/r_validator.h>
#include <spdlog/spdlog.h>

using namespace std;

constexpr auto IDENTIFY_MESSAGE = "Identify yourself!";

void DestroySocket(zsock_t* socket) {
    zsock_destroy(&socket);
}

DiscoveryValidator::DiscoveryValidator(bool has_security) {
    logger_ = spd::get(DISCO_LOGGER_NAME);
    has_security_ = has_security;
}

bool DiscoveryValidator::InitNoSecurity() {
    if (riaps::framework::Configuration::HasSecurity()) {
        return false;
    }
    return true;
}

bool DiscoveryValidator::InitWithSecurity(const std::string &ip_address,
                                          std::shared_ptr<dht::crypto::PrivateKey> &private_key) {
    if (!riaps::framework::Configuration::HasSecurity()) {
        return false;
    }

    last_delete_ = riaps::utils::Timeout<std::chrono::minutes>(1);
    private_key_ = private_key;

    auto zmq_address   = fmt::format("tcp://{}:!", ip_address);

    validator_socket_  = shared_ptr<zsock_t>(zsock_new_router(zmq_address.c_str()), [](zsock_t* z){
        zsock_destroy(&z);
    });
    validator_address_ = zsock_endpoint(validator_socket());
    auto poller = zpoller_new(validator_socket(), nullptr);
    poller_ = shared_ptr<zpoller_t>(poller, [](zpoller_t* p){
        zpoller_destroy(&p);
    });

    signature_ = private_key_->sign(ConvertToBlob(validator_address()));
    return true;
}

bool DiscoveryValidator::IsValid(const std::string &node_address) {
    if (!has_security_)
        return true;
    DeleteOldNodes();
    if (!IsUptodate(node_address)) {
        auto new_node = CreateNode(node_address);
        cluster_nodes_[node_address] = new_node;
        zpoller_add(poller_.get(), new_node->request.get());
    }

    auto current_node = cluster_nodes_[node_address];
    if (!current_node->validated || current_node->check_period.IsTimeout()) {
        Validate(node_address);
    }

    logger_->debug("{} is {}", node_address, cluster_nodes_[node_address]->validated);
    return cluster_nodes_[node_address]->validated;
}

std::shared_ptr<node_info> DiscoveryValidator::CreateNode(const std::string &node_address) {
    auto node = make_shared<node_info>();
    node->check_period = riaps::utils::Timeout<std::chrono::seconds>(10,30);
    node->validated = false;
    zsock_t *socket = zsock_new(ZMQ_DEALER);
    node->request = shared_ptr<zsock_t>(socket, [](zsock_t *z) {
        zsock_destroy(&z);
    });

    return node;
}

bool DiscoveryValidator::IsUptodate(const std::string &node_address) {
    if (cluster_nodes_.find(node_address) == cluster_nodes_.end()) return false;
    if (cluster_nodes_[node_address] == nullptr) return false;

    return true;
}

void DiscoveryValidator::ReplyAll() {
    if (!has_security_)
        return;

    void* which = zpoller_wait(poller_.get(), 500);
    if(which) {
        if (which == validator_socket()) {
            zmsg_t* msg = zmsg_recv(which);
            if (msg) {
                zframe_t* identity_frame = zmsg_pop(msg);
                if (identity_frame) {
                    auto command = zmsg_popstr(msg);
                    if (streq(command, IDENTIFY_MESSAGE)) {
                        logger_->debug("{} from {}", command, (char*)zframe_data(identity_frame));
                        char* s = const_cast<char*>(validator_address().c_str());
                        uint8_t* d = signature_.data();
                        zsock_t* sock = validator_socket();
                        zsock_send(sock, "fsb",
                                       identity_frame,
                                       s,
                                       d, signature_.size());
                        logger_->debug("Identified as {} ({})", s, validator_address());
                    }
                    zstr_free(&command);
                    zframe_destroy(&identity_frame);
                }
                zmsg_destroy(&msg);
            }
        }
        // returned validators
        else {
            char* ip_address;
            unsigned char* signature_bytes;
            size_t signature_size;
            auto rc = zsock_recv(which, "sb", &ip_address, &signature_bytes, &signature_size);
            if (rc == 0) {
                string ip_str = ip_address;
                if (cluster_nodes_.find(ip_str) != cluster_nodes_.end()) {
                    auto blob_signature = ConvertToBlob<unsigned char>(signature_bytes, signature_size);
                    if (private_key_->getPublicKey().checkSignature(ConvertToBlob(ip_str), blob_signature)) {
                        cluster_nodes_[ip_str]->validated = true;
                        logger_->debug("{} is validated from now", ip_str);
                        zsock_disconnect(cluster_nodes_[ip_str]->request.get(), ip_address);
                    }
                }
                zstr_free(&ip_address);
                delete [] signature_bytes;
            }
        }
    }
}

void DiscoveryValidator::Validate(const std::string &node_address) {
    auto node = cluster_nodes_[node_address];
    node->check_period.Reset();
    node->validated = false;
    zsock_connect(node->request.get(), node_address.c_str());
    string msg_cpy = IDENTIFY_MESSAGE;
    zsock_send(node->request.get(), "s", msg_cpy.c_str());
}

void DiscoveryValidator::DeleteOldNodes() {
    if (last_delete_.IsTimeout()) {
        for (auto it = cluster_nodes_.begin(); it != cluster_nodes_.end();) {
            if (it->second->check_period.IsTimeout()) {
                logger_->debug("Delete: {}", it->first);
                zpoller_remove(poller_.get(), it->second->request.get());
                cluster_nodes_.erase(it++);
            } else {
                ++it;
            }
        }
    }
    last_delete_.Reset();
}

DiscoveryValidator::~DiscoveryValidator() {
    logger_->debug("{}", __func__);
    poller_.reset();
}