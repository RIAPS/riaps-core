//
// Created by istvan on 11/22/16.
//

#include "discoveryd/r_odht.h"
#include <czmq.h>

void dhtJoinToCluster(std::string& destination_host, int port, dht::DhtRunner& dhtrunner){
    dhtrunner.bootstrap(destination_host, std::to_string(port));
}

void handleGet(const ProviderListGet::Reader& msgProviderGet,
               const std::map<std::string, std::unique_ptr<actor_details>>& clients)
{
    auto msgGetResults = msgProviderGet.getResults();

    for (int idx = 0; idx < msgGetResults.size(); idx++) {
        std::string resultEndpoint = std::string(msgGetResults[idx].cStr());

        auto pos = resultEndpoint.find(':');
        if (pos == std::string::npos) {
            continue;
        }

        std::string host = resultEndpoint.substr(0, pos);
        std::string port = resultEndpoint.substr(pos + 1, std::string::npos);
        int portNum = -1;

        try {
            portNum = std::stoi(port);
        } catch (std::invalid_argument &e) {
            std::cout << "Cast error, string -> int, portnumber: " << port << std::endl;
            std::cout << e.what() << std::endl;
            continue;
        }
        catch (std::out_of_range &e) {
            std::cout << "Cast error, string -> int, portnumber: " << port << std::endl;
            std::cout << e.what() << std::endl;
            continue;
        }

        capnp::MallocMessageBuilder message;
        auto msg_discoupd = message.initRoot<DiscoUpd>();
        auto msg_client = msg_discoupd.initClient();
        auto msg_socket = msg_discoupd.initSocket();

        // Set up client
        msg_client.setActorHost(msgProviderGet.getClient().getActorHost());
        msg_client.setActorName(msgProviderGet.getClient().getActorName());
        msg_client.setInstanceName(msgProviderGet.getClient().getInstanceName());
        msg_client.setPortName(msgProviderGet.getClient().getPortName());

        msg_discoupd.setScope(msgProviderGet.getPath().getScope());

        msg_socket.setHost(host);
        msg_socket.setPort(portNum);

        auto serializedMessage = capnp::messageToFlatArray(message);

        zmsg_t *msg = zmsg_new();
        zmsg_pushmem(msg, serializedMessage.asBytes().begin(),
                     serializedMessage.asBytes().size());

        std::string clientKeyBase =  "/" + std::string(msgProviderGet.getPath().getAppName())
                                     + "/" + std::string(msgProviderGet.getClient().getActorName())
                                     + "/";

        zmsg_send(&msg, clients.at(clientKeyBase)->socket);

        std::cout << "Get results were sent to the client: " << clientKeyBase << std::endl;
    }
}

/// \brief Gets the provider updates from the OpenDHT (e.g.: new publisher registered) and sends a DiscoUpd message
///        to the interested clients.
/// \param msgProviderUpdate The capnp message from the OpenDHT Listen().
/// \param clientSubscriptions List of current key subscribtions.
/// \param clients  Holds the ZMQ sockets of the client actors.
void handleUpdate(const ProviderListUpdate::Reader& msgProviderUpdate,
                  const std::map<std::string, std::vector<std::unique_ptr<client_details>>>& clientSubscriptions,
                  const std::map<std::string, std::unique_ptr<actor_details>>& clients){

    std::string provider_key = std::string(msgProviderUpdate.getProviderpath().cStr());

    auto msg_newproviders = msgProviderUpdate.getNewvalues();

    // Look for services who may interested in the new provider
    if (clientSubscriptions.find(provider_key) != clientSubscriptions.end()) {
        for (auto &subscribedClient : clientSubscriptions.at(provider_key)) {
            for (int idx = 0; idx < msg_newproviders.size(); idx++) {
                std::string new_provider_endpoint = std::string(msg_newproviders[idx].cStr());

                auto pos = new_provider_endpoint.find(':');
                if (pos == std::string::npos) {
                    continue;
                }

                std::string host = new_provider_endpoint.substr(0, pos);
                std::string port = new_provider_endpoint.substr(pos + 1, std::string::npos);
                int portNum = -1;

                try {
                    portNum = std::stoi(port);
                } catch (std::invalid_argument &e) {
                    std::cout << "Cast error, string -> int, portnumber: " << port << std::endl;
                    std::cout << e.what() << std::endl;
                    continue;
                }
                catch (std::out_of_range &e) {
                    std::cout << "Cast error, string -> int, portnumber: " << port << std::endl;
                    std::cout << e.what() << std::endl;
                    continue;
                }


                std::string clientKeyBase = "/" + subscribedClient->app_name +
                                            "/" + subscribedClient->actor_name +
                                            "/";

                std::cout << "Search for registered actor: " + clientKeyBase << std::endl;

                // Python reference:
                // TODO: Figure out, de we really need for this. I don't think so...
                //if self.hostAddress != actorHost:
                //continue

                // If the client port saved before
                if (clients.find(clientKeyBase) != clients.end()) {
                    const actor_details *clientSocket = clients.at(clientKeyBase).get();

                    if (clientSocket->socket != NULL) {
                        capnp::MallocMessageBuilder message;
                        auto msg_discoupd = message.initRoot<DiscoUpd>();
                        auto msg_client = msg_discoupd.initClient();
                        auto msg_socket = msg_discoupd.initSocket();

                        // Set up client
                        msg_client.setActorHost(subscribedClient->actor_host);
                        msg_client.setActorName(subscribedClient->actor_name);
                        msg_client.setInstanceName(subscribedClient->instance_name);
                        msg_client.setPortName(subscribedClient->portname);

                        msg_discoupd.setScope(subscribedClient->isLocal ? Scope::LOCAL : Scope::GLOBAL);

                        msg_socket.setHost(host);
                        msg_socket.setPort(portNum);

                        auto serializedMessage = capnp::messageToFlatArray(message);

                        zmsg_t *msg = zmsg_new();
                        zmsg_pushmem(msg, serializedMessage.asBytes().begin(),
                                     serializedMessage.asBytes().size());

                        zmsg_send(&msg, clientSocket->socket);

                        std::cout << "Port update sent to the client: " << clientKeyBase << std::endl;
                    }
                }
            }
        }
    }
}