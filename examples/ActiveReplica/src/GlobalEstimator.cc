//
// Created by istvan on 11/11/16.
//


#include <GlobalEstimator.h>

namespace activereplica {
    namespace components {

        GlobalEstimator::GlobalEstimator(_component_conf &config, riaps::Actor &actor)
                : GlobalEstimatorBase(config, actor), _hasJoined(false) {
            //PrintParameters();

            SetDebugLevel(_logger, spdlog::level::level_enum::debug);

            _pending = 0;
            _idIndex = 1;
        }

        void GlobalEstimator::OnEstimate(messages::Estimate::Reader &message,
                                         riaps::ports::PortBase *port) {
            //PrintMessageOnPort(port);
            //std::cout << "GlobalEstimator::OnEstimate(): " << message.getMsg().cStr() << std::endl;
        }

        void GlobalEstimator::OnWakeup(riaps::ports::PortBase *port) {
            //PrintMessageOnPort(port);
            //_logger->debug("OnWakeUp()");

            riaps::groups::GroupId gid;
            gid.groupTypeId = GROUPTYPE_BACKUPGROUP;
            gid.groupName = "Group1";

            if (!_hasJoined) {
                _hasJoined = true;
                bool rc = this->JoinToGroup(gid);
                _logger->debug_if(rc, "Successfully joined to group {}:{}", gid.groupTypeId, gid.groupName);

            } else {
                capnp::MallocMessageBuilder builder;

                auto msgEstimate = builder.initRoot<activereplica::messages::QueryRequest>();

                // Set the query id, which is the idIndex bit.
                msgEstimate.setId(_idIndex.to_ulong());

                //add the id to the pending requests
                _pending |= _idIndex;

                // shift the _idIndex
                _idIndex <<= 1;
                if (_idIndex == 0) _idIndex =1;

                bool rc = SendGroupMessage(gid, builder, GROUPPORT_BACKUPGROUP_QUERY_OUT);
                _logger->warn_if(!rc, "Sending groupmessage failed");
                if (rc){
                    _logger->info("Request [{}] sent", msgEstimate.getId());
                }
            }

        }

        void GlobalEstimator::OnGroupMessage(const riaps::groups::GroupId &groupId,
                                             capnp::FlatArrayMessageReader &capnpreader,
                                             riaps::ports::PortBase *port) {
            if (groupId.groupTypeId == GROUPTYPE_BACKUPGROUP && groupId.groupName == "Group1") {
                if (port->GetPortName() == GROUPPORT_BACKUPGROUP_RESPONSE_IN) {

                    auto msgEstimate    = capnpreader.getRoot<activereplica::messages::Estimate>();
                    uint32_t requestId  = msgEstimate.getId();
                    std::bitset<QUERYID_LENGTH> requestBits(requestId);


                    // check if the message already arrived earlier
                    if ((_pending&requestBits).any()){
                        // The message is not processed before

                        // delete the pending flag
                        _pending ^=requestBits;
                        _logger->info("Response: {}", msgEstimate.getValue());

                    }


                }
            }

        }

        GlobalEstimator::~GlobalEstimator() {

        }
    }
}

riaps::ComponentBase *create_component(_component_conf &config, riaps::Actor &actor) {
    auto result = new activereplica::components::GlobalEstimator(config, actor);
    return result;
}

void destroy_component(riaps::ComponentBase *comp) {
    delete comp;
}