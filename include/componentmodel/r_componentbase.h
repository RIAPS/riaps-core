/*! \mainpage Welcome to the C++ component framework documentation

The component framework provides a base class (ComponentBase) for your components. Features of the RIAPS framework can be used by calling the provided APIs.
 The RIAPS services are grouped into four modules:
 -# Component API
 -# Resource and fault management
 -# Distributed Coordination
 -# Port API
 */


#ifndef RIAPS_R_COMPONENTBASE_H
#define RIAPS_R_COMPONENTBASE_H

#include <componentmodel/r_pyactor.h>
#include <componentmodel/r_discoverdapi.h>
#include <componentmodel/r_configuration.h>
#include <componentmodel/r_oneshottimer.h>
#include <componentmodel/r_messageparams.h>
#include <componentmodel/r_messagebuilder.h>
#include <groups/r_groupid.h>
#include <groups/r_group.h>
#include <messaging/disco.capnp.h>
#include <utils/r_utils.h>

#include <msgpack.hpp>
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <spdlog_setup/conf.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <queue>
#include <random>
#include <ctime>


#include <componentmodel/ports/r_publisherport.h>
#include <componentmodel/ports/r_subscriberport.h>
#include <componentmodel/ports/r_responseport.h>
#include <componentmodel/ports/r_requestport.h>
#include <componentmodel/ports/r_insideport.h>
#include <componentmodel/ports/r_answerport.h>
#include <componentmodel/ports/r_queryport.h>
#include <componentmodel/ports/r_periodictimer.h>

constexpr auto BILLION = 1000000000l;
constexpr auto TIMER_ACCURACY = (50*1000); // 50 microsec

namespace spd = spdlog;

namespace riaps {


    namespace groups{
        class Group;
    }



    /**
     * @brief Declares the component thread function. The component thread is implemented as a ZMQ actor (http://czmq.zeromq.org/czmq4-0:zactor).
     * @param pipe ZMQ pair socket to send/recv messages to/from the component thread.
     * @param args Outer shell object, handles the communication with the PyActor, provides virtual methods for the developer components.
     */
    void component_actor(zsock_t* pipe, void* args);

    /**
     * @brief Parent class of all C++ components. Provides the RIAPS API for the developer.
     */
    class ComponentBase {
        friend riaps::groups::Group;
    public:

        ComponentBase(const std::string &application_name,
                      const std::string &actor_name);

        /** @name API for the Python actor
         */
        ///@{
        /**
         * @brief Stops the component.
         *
         * Stops the component in the following order:
         *   -# Stops the timers
         *   -# Destroys the component thread
         *   -# Deletes ports
         */
        virtual void Terminate() final;
        virtual void Setup() final;
        virtual void Activate() final;

        /**
         * Forwards PortUpdate message to the component thread.
         * @param port_name The name of the port which has to connect to a new service.
         * @param host IP address of the new service.
         * @param port Port of the new service.
         */
        virtual void HandlePortUpdate(const std::string &port_name, const std::string &host, int port) final;
        virtual void HandleReinstate() final;
        virtual void HandleActivate();
        ///@}

        /**
         * @return PyActor object, contains the properties of the Python actor.
         */
        std::shared_ptr<PyActor> actor() const;



        /// @cond
        /// Friend declaration. Skipped in doxygen.
        friend void component_actor(zsock_t* pipe, void* args);
        /// @endcond

        /**
         * \defgroup B Component API
         * @{
         */

        /**
         * @return The component logger instance.
         */
        std::shared_ptr<spd::logger> component_logger() const;

        /**
         * @return The component unique ID.
         */
        const std::string ComponentUuid() const;

        /**
         * @return True if the security is turned on, otherwise false.
         */
        bool has_security() const;

        /**
         * @return ::ComponentConf object that contains the component configuration.
         */
        const ComponentConf& component_config() const;
        ///@}

        /**
         * \defgroup RFM Resource and fault management
         * @{
         */

        /**
         * Handle the case when the CPU limit is exceeded: notify each component. If the component has defined a handler, it will be called.
         */
        virtual void HandleCPULimit();

        /**
         * Handle the case when the memory limit is exceeded: notify each component. If the component has defined a handler, it will be called.
         */
        virtual void HandleMemLimit();

        /**
         * Handle the case when the file space limit is exceeded: notify each component. If the component has defined a handler, it will be called.
         */
        virtual void HandleSpcLimit();

        /**
         * Handle the case when the net usage limit is exceeded: notify each component. If the component has defined a handler, it will be called.
         */
        virtual void HandleNetLimit();

        /**
         * Handle the NIC state change message: notify components
         */
        virtual void HandleNICStateChange(const std::string& state);

        /**
         * Handle the peer state change message: notify components
         * @param state
         * @param uuid Component UUID
         */
        virtual void HandlePeerStateChange(const std::string& state, const std::string& uuid);
        ///@}



        /** @name Internal methods.
         */
        ///@{
        /**
         * @return PAIR socket of the component thread.
         */
        zactor_t* GetZmqPipe() const;

        /**
         * @brief Sends capnp message on port.
         *
         * Copies the capnp message buffer into a ZMQ message frame and sends the message on port, identified by port_name.
         * @param message The capnp buffer that holds the message
         * @param port_name The message is sent on this port. Available names are declared in the riaps model.
         * @return riaps::ports::PortError object, that contains the error code if any.
         */
        riaps::ports::PortError SendMessageOnPort(capnp::MallocMessageBuilder& message,
                                                  const std::string& port_name);

        /**
         * Sends message on answer port
         *
         * @return
         */
//        riaps::ports::PortError SendMessageOnPort(capnp::MallocMessageBuilder& message,
//                                                  const std::string& port_name,
//                                                  std::shared_ptr<riaps::MessageParams> params);

        /**
         * Sends a message on query port
         *
         */
        riaps::ports::PortError SendMessageOnPort(capnp::MallocMessageBuilder& message,
                                                  const std::string&           port_name,
                                                  std::string&                 request_id);

        ///@}

        /**
         * \defgroup DC Distributed Coordination
         * @{
         */

        /**
         * Sends a message to every members in the given group.
         * @param group_id Group instance id.
         * @param message The message to be sent in capnp buffer.
         * @return True if the send was successful. False otehrwise.
         */
//        template<class T>
//        bool SendGroupMessage(
//                const riaps::groups::GroupId& group_id,
//                MessageBuilder<T>& message);

        /**
         * Handler for group messages.
         * Implementation must immediately call recv on the group to obtain message.
         * @param group The group where the message arrived.
         */
        virtual void HandleGroupMessage(groups::Group* group);

        virtual void HandleMessageFromLeader(groups::Group* group);

        virtual void HandleMessageToLeader(groups::Group* group, std::string identity);

        virtual void HandleVoteRequest(riaps::groups::Group *group, std::string rfvid);

        virtual void HandleVoteResult(riaps::groups::Group *group, std::string rfvid, bool vote);


        /** @}*/

        virtual ~ComponentBase() = default;

    protected:
        /**
         * Sends a ZMQ message on the given inside port. This Send() is just for InsidePorts
         *
         * @param message ZMQ message structure to be sent
         * @param port_name The port, which sends the message.
         * @return True if the message was sent successfully.
         */
        bool SendMessageOnPort(zmsg_t** message, const std::string& port_name);

        /**
         * \addgroup DC
         * @{
         */
        /**
         * Fired when a message arrives on one the group ports.
         * @param group_id Group instance id.
         * @param capnpreader The received message in capnp buffer
         * @param port The port structure where the message was read form.
         */
        virtual void OnGroupMessage(const riaps::groups::GroupId& group_id,
                                    capnp::FlatArrayMessageReader& capnpreader,
                                    riaps::ports::PortBase* port);






        /**
         * Gives a snapshot about the members of a group, considering a timeout.
         * @param groupId Group instance id.
         * @param timeout A member is counted if heartbeat was received from it in the last "timeout" msec
         * @return The number of members.
         */
        uint16_t GetGroupMemberCount(const riaps::groups::GroupId &groupId,
                                     int64_t timeout = 1000 * 15 /*15 sec in msec*/);

        std::string GetLeaderId(const riaps::groups::GroupId& groupId);

        std::vector<riaps::groups::Group*> GetGroups();

        /**
         * Does a valid leader available in the group?
         * @param groupId
         * @return
         */
        bool IsLeaderAvailable(const riaps::groups::GroupId& groupId);

        /**
         * Is the current component the leader?
         * @param groupId
         * @return
         */
        bool IsLeader(const riaps::groups::GroupId& groupId);
        ///@}

        /**
         * \addtogroup B
         * @{
         */

        /**
         * The name of the component logger.
         * @return The name of the component logger.
         */
        const std::string component_logger_name();

        /**
         *
         * @tparam T Type of the RIAPS port.
         * @return Port instance with the given name, otherwise nullptr.
         */
        template<class T>
        T* GetPortAs(const std::string&);

        /**
         * Search a port with the given name.
         * @return NULL if there is no port with the given name.
         */
        ports::PortBase* GetPortByName(const std::string&);

        /**
         * The component name.
         * @return The component name.
         */
        const std::string component_name() const;

        /**
         * Search publisher port by name.
         *
         * @param portName The name of the publisher port to be retrieved.
         * @return NULL if the port with the name wasn't found or the port is not a publisher port.
         */
        ports::PublisherPort*  GetPublisherPortByName(const std::string& portName);

        /**
         * Search request port by name.
         *
         * @param portName The name of the request port to be retrieved.
         * @return NULL if the port with the name wasn't found or the port is not a request port.
         */
        ports::RequestPort*    GetRequestPortByName(const std::string& portName);

        /**
         * Search query port by name.
         *
         * @param portName The name of the request port to be retrieved.
         * @return NULL if the port with the name wasn't found or the port is not a request port.
         */
        ports::QueryPort*      GetQueryPortByName(const std::string& portName);

        /**
         * Search response port by name.
         *
         * @param portName The name of the response port to be retrieved.
         * @return NULL if the port with the name wasn't found or the port is not a response port.
         */
        ports::ResponsePort*   GetResponsePortByName(const std::string& portName);

        /**
         * Search subscriber port by name.
         *
         * @param portName The name of the subscriber port to be retrieved.
         * @return NULL if the port with the name wasn't found or the port is not a subscriber port.
         */
        ports::SubscriberPort* GetSubscriberPortByName(const std::string& portName);

        /**
         * Sets the debug level. The possible debug levels are:
         *  -# trace = 0,
         *  -# debug = 1,
         *  -# info = 2,
         *  -# warn = 3,
         *  -# err = 4,
         *  -# critical = 5,
         *  -# off = 6
         *
         *  @param component_level Debug level of the components.
         *  @param framework_level Debug level of the RIAPS framework.
         */
        void set_debug_level(spd::level::level_enum component_level,
                             spd::level::level_enum framework_level = spd::level::info);
        ///@}

        // Note: disable for now, we need more tests.
        //bool CreateOneShotTimer(const std::string& timerid, timespec& wakeuptime);
        //virtual void OnScheduledTimer(char* timerId, bool missed);
        virtual void OnScheduledTimer(uint64_t timerId);

        /**
         * Converts and forwards the incoming capnp message to the appropriate handler. Pure virtual function,
         * the inherited class must know the possible message types.
         *
         * @param capnpreader The capnp buffer holding the incoming message
         * @param port The port where the message arrived.
         */
        virtual void DispatchMessage(ports::PortBase* port) = 0;

        /**
         * Forwards the given ZMQ message to the appropriate handler. Used for inside ports only in device components.
         * @param zmsg
         * @param port
         */
        virtual void DispatchInsideMessage(zmsg_t* zmsg,
                                           ports::PortBase* port) = 0;

        timespec WaitUntil(const timespec& targetTimepoint);

        /**
         * \addtogroup DC
         * @{
         */

        /**
         * Is the current component the leader?
         * @param groupId
         * @return True if the current component is the leader in the given group, otherwise False.
         */
        bool IsLeader(const riaps::groups::Group* groupId);

        /**
         * The component joins to the given group.
         * @param groupId The id of the group to join.
         */
        bool JoinGroup(riaps::groups::GroupId&& groupId);

        /**
         * The component joins to the given group.
         * @param groupId The id of the group to join.
         */
        bool JoinGroup(riaps::groups::GroupId&  groupId);

        /**
         * The component leaves the given group.
         * @param groupId The id of the group to be left.
         */
        bool LeaveGroup(riaps::groups::GroupId&& groupId);

        /**
         * The component leaves the given group.
         * @param groupId The id of the group to be left.
         */
        bool LeaveGroup(riaps::groups::GroupId&  groupId);

        /**
         * Returns the current memberships of the component.
         */
        std::vector<riaps::groups::GroupId> GetGroupMemberships();

        /**
         * Returns the current memberships of the component by type.
         * @param groupType Group type
         */
        std::vector<riaps::groups::GroupId> GetGroupMembershipsByType(const std::string& groupType);

        /**
         * @return True if the component is member of the given group. False otherwise.
         */
        bool IsMemberOf(riaps::groups::GroupId& groupId);

        /**
         * Handler, vote request about the message.
         * @param groupId
         * @param proposeId
         * @param message
         */
        virtual void OnPropose (riaps::groups::GroupId& groupId, const std::string& proposeId, capnp::FlatArrayMessageReader& message);

        /**
         * Handler, vote request about the action at timePoint.
         * @param groupId
         * @param proposeId
         * @param actionId
         * @param timePoint
         */
        virtual void OnActionPropose (riaps::groups::GroupId& groupId,
                                      const std::string& proposeId,
                                      const std::string& actionId,
                                      const timespec& timePoint);

        /**
         * Handler. Message arrived to the leader.
         * @param groupId
         * @param message
         */
        virtual void OnMessageToLeader(const riaps::groups::GroupId& groupId, capnp::FlatArrayMessageReader& message);

        /**
         * Handler. Message arrived from the leader.
         * @param groupId
         * @param message
         */
        virtual void OnMessageFromLeader(const riaps::groups::GroupId& groupId, capnp::FlatArrayMessageReader& message);

        /**
         * Handler. Outcome of a vote is announced.
         * @param groupId
         * @param proposeId
         * @param accepted
         */
        virtual void OnAnnounce(const riaps::groups::GroupId& groupId, const std::string& proposeId, bool accepted);

        /**
         * Sends a request for vote about the _message_.
         * @param groupId
         * @param message
         * @return
         */
        std::string SendPropose(const riaps::groups::GroupId& groupId, capnp::MallocMessageBuilder& message);

        /**
         * Sends vote about proposeId.
         */
        bool SendVote(const riaps::groups::GroupId& groupId, const std::string& proposeId, bool accept);

        /**
         * Proposes an action to the leader.
         * @param group_id Unique ID of the group instance.
         * @param action_id The action to be performed. Developer-generated ID, each component is able to identify the appropriate function to be called.
         * @param abs_time The proposed timepoint, when the action should be executed.
         * @return The generated proposeId. The leader announces the results by this id.
         */
//        std::string ProposeAction(const riaps::groups::GroupId& group_id  ,
//                                  const std::string&            action_id ,
//                                  const timespec&               abs_time
//        );
        ///@}

        uint64_t ScheduleAbsTimer(const timespec &t, uint64_t wakeupOffset = 0 /*nanosec*/);

        /**
         * @param tp Absolute time when the action is executed
         * @param action Pointer to the function to be executed
         * @param wakeupOffset The timer is fired earlier by wakeupOffset.
         *                     The value is in nanosec and depends on the current platform.
         *                     On BBB it is 2000 microsec, this is the worst case.
         *                     If the timer fires too early, make sure to call the WaitUntil() in the handler.
         * @return Unique id of the scheduled timer
         */
        uint64_t ScheduleAction(const timespec &tp, std::function<void(const uint64_t)> action,
                                uint64_t wakeupOffset = 2000 * 1000);


        /**
         * Points to the scheduled action.
         */
        std::function<void(const uint64_t)> scheduled_action_;

        /**
         * Saves the component configuration and sets up the loggers with the appropriate names.
         * @param c_conf The component definition from the model file.
         */
        void set_config(ComponentConf& c_conf, const std::vector<GroupConf> &group_conf);

        /**
         * Returns the group instance based on its id.
         * @param group_id Group instance id.
         * @return Pointer to the group instance.
         */
        riaps::groups::Group* getGroupById(const riaps::groups::GroupId &group_id);



    private:

        /**
         * Framework logger.
         */
        std::shared_ptr<spd::logger> riaps_logger_;

        /**
         * Component logger
         */
        std::shared_ptr<spd::logger> component_logger_;

        /**
         * Name of the component logger.
         */
        std::string                  component_logger_name_;

        /**
         * Initializes a publisher port and stores it in the #ports_.
         * @param config Port configuration. Comes from the model file.
         * @return Pointer to the newly created port.
         */
        const ports::PublisherPort*  InitPublisherPort  (const ComponentPortPub& config);

        /**
         * Initializes a subscriber port and stores it in the #ports_.
         * @param config Port configuration. Comes from the model file.
         * @return Pointer to the newly created port.
         */
        const ports::SubscriberPort* InitSubscriberPort (const ComponentPortSub& config);

        /**
         * Initializes a response port and stores it in the #ports_.
         * @param config Port configuration. Comes from the model file.
         * @return Pointer to the newly created port.
         */
        const ports::ResponsePort*   InitResponsePort   (const ComponentPortRep& config);

        /**
         * Initializes a request port and stores it in the #ports_.
         * @param config Port configuration. Comes from the model file.
         * @return Pointer to the newly created port.
         */
        const ports::RequestPort*    InitRequestPort    (const ComponentPortReq& config);

        /**
         * Initializes a query port (ZMQ_DEALER) and stores it in the #ports_.
         * @param config Port configuration. Comes from the model file.
         * @return Pointer to the newly created port.
         */
        const ports::QueryPort*      InitQueryPort      (const ComponentPortQry& config);

        /**
         * Initializes an answer (ZMQ-ROUTER) port and stores it in the #ports_.
         * @param config Port configuration. Comes from the model file.
         * @return Pointer to the newly created port.
         */
        const ports::AnswerPort*     InitAnswerPort     (const ComponentPortAns& config);

        /**
         * Initializes a timer port and stores it in the #ports_.
         * @param config Port configuration. Comes from the model file.
         * @return Pointer to the newly created port.
         */
        const ports::PeriodicTimer*  InitTimerPort      (const ComponentPortTim& config);

        /**
         * Initializes an inside port and stores it in the #ports_.
         * @param config Port configuration. Comes from the model file.
         * @return Pointer to the newly created port.
         */
        const ports::InsidePort*     InitInsidePort     (const ComponentPortIns& config);



        //std::string             getTimerChannel();
        std::string             getOneShotTimerChannel();


        /**
         * The component configuration. Comes from the riaps model file.
         */
        ComponentConf component_config_;


        /**
         * Pointers to the component ports.
         */
        std::unordered_map<std::string, std::unique_ptr<ports::PortBase>> ports_;

        /**
         * Pointers to the group instances.
         */
        std::map<riaps::groups::GroupId, std::unique_ptr<riaps::groups::Group>> groups_;

        // Maintaining ZMQ Socket - riaps port pairs. For the quick retrieve.
        std::unordered_map<const zsock_t*, const ports::PortBase*>   port_sockets_;

        // Maintaining ZMQ Socket - group port pairs. For the quick retrieve.
        std::unordered_map<const zsock_t*, groups::Group*> group_sockets_;



        uint64_t timer_counter_;

        /**
         * Unique ID of the componenet. Regenerated at every start.
         */
        zuuid_t* component_uuid_;

        /**
         * Points to the component owner.
         */
        std::shared_ptr<PyActor> actor_;

        /**
         * The component thread.
         */
        zactor_t* component_zactor_;

        /**
         * Poller, runs in the component thread.
         */
        zpoller_t* component_poller_;

        /**
         * This setting is coming from the riaps.conf file.
         * If the security is turned on, this is true. False, otherwise.
         */
        bool has_security_;
    };

    /**
     * Finds a port by name and converts it to T*
     * @tparam T The expected port type.
     * @param port_name The name of the port to be searched.
     * @return Pointer to the port. Nullptr if not found or wrong type.
     */
    template<class T>
    T* ComponentBase::GetPortAs(const std::string& port_name) {
        ports::PortBase* port_base = GetPortByName(port_name);
        if (port_base == nullptr) return nullptr;
        return dynamic_cast<T*>(port_base);
    }
}


#endif //RIAPS_R_COMPONENTBASE_H
