/**
 * Handlers of disco messages
 *
 * @author Istvan Madari <istvan.madari@vanderbilt.edu>
 */

#include <discoveryd/r_riaps_cmd_handler.h>
#include <discoveryd/r_odht.h>
#include <framework/rfw_network_interfaces.h>

#include <msgpack.hpp>


std::pair<std::string, std::string>
buildInsertKeyValuePair(std::string appName ,
                        std::string msgType ,
                        riaps::discovery::Kind        kind    ,
                        riaps::discovery::Scope       scope   ,
                        std::string host    ,
                        uint16_t    port    ) {
    std::string key;
    key =   "/" + appName
          + "/" + msgType
          + "/" + kindMap[kind];

    if (scope == riaps::discovery::Scope::LOCAL){
        // hostid
        //auto hostid = gethostid();

        std::string mac_address = riaps::framework::Network::GetMacAddressStripped();

        key += mac_address;
    }

    std::string value = host + ":" + std::to_string(port);

    return std::pair<std::string, std::string>(key, value);
}

std::pair<std::string, std::string>
buildLookupKey(std::string appName,
               std::string msgType,
               riaps::discovery::Kind kind,
               riaps::discovery::Scope scope,
               std::string clientActorHost,
               std::string clientActorName,
               std::string clientInstanceName,
               std::string clientPortName ) {


    std::string key;

    std::map<riaps::discovery::Kind, std::string> kindPairs = {
                          {riaps::discovery::Kind::SUB, kindMap[riaps::discovery::Kind::PUB]},
                          {riaps::discovery::Kind::CLT, kindMap[riaps::discovery::Kind::SRV]},
                          {riaps::discovery::Kind::REQ, kindMap[riaps::discovery::Kind::REP]},
                          {riaps::discovery::Kind::REP, kindMap[riaps::discovery::Kind::REQ]}};

    key =     "/" + appName
            + "/" + msgType
            + "/" + kindPairs[kind];

    //auto hostid = gethostid();

    std::string hostid = riaps::framework::Network::GetMacAddressStripped();

    if (scope == riaps::discovery::Scope::LOCAL){
        key += hostid;
    }

    std::string client =   '/' + appName
                         + '/' + clientActorName
                         + '/' + clientActorHost
                         + '/' + clientInstanceName
                         + '/' + clientPortName;

    if (scope == riaps::discovery::Scope::LOCAL) {
        client = client + ":" + hostid;
    }

    return {key, client};
}

// Hanlde ZMQ message scoming from the RIAPS actors/components
bool handleRiapsMessages(zsock_t* riapsSocket,
                         std::map<std::string, std::unique_ptr<actor_details_t>>& clients,
                         std::map<pid_t, std::vector<std::unique_ptr<service_checkins_t>>>& serviceCheckins,
                         std::map<std::string, std::vector<std::unique_ptr<client_details_t>>>& clientSubscriptions,
                         std::map<std::string, std::future<size_t>>& registeredListeners,
                         const std::string& hostAddress,
                         const std::string& macAddress,
                         const std::map<std::string, int64_t>& zombieList,
                         dht::DhtRunner& dhtNode
                         ) {

    else {



        if (msgDiscoReq.isGroupJoin()) {
            // Join to the group.
            auto msgGroupJoin     = msgDiscoReq.getGroupJoin();
            auto msgGroupServices = msgGroupJoin.getServices();
            std::string appName   = msgGroupJoin.getGroupId().getAppName();
            riaps::groups::GroupDetails groupDetails;
            groupDetails.appName = appName;
            groupDetails.groupId = {
                    msgGroupJoin.getGroupId().getGroupName(),
                    msgGroupJoin.getGroupId().getGroupType()
            };

            for (int i = 0; i<msgGroupServices.size(); i++){
                groupDetails.groupServices.push_back({
                                                        msgGroupServices[i].getMessageType(),
                                                        msgGroupServices[i].getAddress()
                                                     });
            }

            msgpack::sbuffer sbuf;
            msgpack::pack(sbuf, groupDetails);
            std::string key = "/groups/"+appName;
            dht::Blob b(sbuf.data(), sbuf.data()+sbuf.size());
            dhtNode.put(key, dht::Value(b));

            // Debug
            std::cout << "Component joined to group: "
                      << appName << "::"
                      << groupDetails.groupId.groupTypeId << "::"
                      << groupDetails.groupId.groupName   << std::endl;
            std::cout << "Group services: " << std::endl;
            for (auto& g : groupDetails.groupServices){
                    std::cout << "\t- " << g.address << " " << g.messageType << std::endl;
            }

            //Send response
            capnp::MallocMessageBuilder repMessage;
            auto msgDiscoRep     = repMessage.initRoot<riaps::discovery::DiscoRep>();
            auto msgGroupJoinRep = msgDiscoRep.initGroupJoin();

            msgGroupJoinRep.setStatus(riaps::discovery::Status::OK);

            auto serializedMessage = capnp::messageToFlatArray(repMessage);

            zmsg_t *msg = zmsg_new();
            zmsg_pushmem(msg, serializedMessage.asBytes().begin(), serializedMessage.asBytes().size());

            zmsg_send(&msg, riapsSocket);
        }

        zmsg_destroy(&riapsMessage);
    }
    return terminated;
}
