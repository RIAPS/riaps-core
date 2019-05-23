/**
 * RIAPS ComponentBase
 *
 * @brief Parent class of all RIAPS components. Controls the communication ports, messaging, timers, groups.
 *
 * @author Istvan Madari
 */


#ifndef RIAPS_R_COMPONENTBASE_H
#define RIAPS_R_COMPONENTBASE_H

#include <componentmodel/r_pyactor.h>
#include <componentmodel/r_discoverdapi.h>
#include <componentmodel/r_configuration.h>
#include <componentmodel/r_oneshottimer.h>
#include <componentmodel/r_payload.h>
#include <groups/r_group.h>
#include <messaging/disco.capnp.h>

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
        struct GroupId;
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

        ComponentBase(const std::string &application_name, const std::string &actor_name);

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
         * \defgroup B Metadata API
         * @{
         */

        /**
         * @return The component logger instance.
         */
        std::shared_ptr<spd::logger> component_logger();

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
        virtual void HandleCPULimit();
        virtual void HandleMemLimit();
        virtual void HandleSpcLimit();
        virtual void HandleNetLimit();
        virtual void HandleNICStateChange(const std::string& state);
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
        riaps::ports::PortError SendMessageOnPort(capnp::MallocMessageBuilder& message,
                                                  const std::string& port_name,
                                                  std::shared_ptr<riaps::MessageParams> params);

        /**
         * Sends a message on query port
         *
         */
        riaps::ports::PortError SendMessageOnPort(capnp::MallocMessageBuilder& message,
                                                  const std::string&           port_name,
                                                  std::string&                 request_id);

        void UpdateGroup(riaps::discovery::GroupUpdate::Reader& msgGroupUpdate);



        ///@}

        /**
         * \defgroup DC Distributed Coordination
         * @{
         */
        bool SendGroupMessage(const riaps::groups::GroupId& groupId,
                              capnp::MallocMessageBuilder& message,
                              const std::string& portName="");

        bool SendGroupMessage(const riaps::groups::GroupId&& groupId,
                              capnp::MallocMessageBuilder& message,
                              const std::string& portName="");
        /** @}*/

        virtual ~ComponentBase() = default;

    protected:
        /**
         * Sends a ZMQ message on the given inside port. This Send() is just for InsidePorts
         *
         * @param message ZMQ message structure to be sent
         * @param portName The port, which sends the message.
         * @return True if the message was sent successfully.
         */
        bool SendMessageOnPort(zmsg_t** message, const std::string& portName);

        /**
         * \addgroup DC
         * @{
         */
        /**
         * Fired when a message arrives on one the group ports.
         * @param groupId groupType, groupName pair (the unique identifier of the group)
         * @param capnpreader The received message in capnp buffer
         * @param port The port structure where the message was read form.
         */
        virtual void OnGroupMessage(const riaps::groups::GroupId& groupId,
                                    capnp::FlatArrayMessageReader& capnpreader,
                                    riaps::ports::PortBase* port);

        bool SendMessageToLeader(const riaps::groups::GroupId& groupId,
                                 capnp::MallocMessageBuilder& message);

        bool SendLeaderMessage(const riaps::groups::GroupId& groupId,
                               capnp::MallocMessageBuilder& message);


        

        /**
         * Gives a snapshot about the members of a group, considering a timeout.
         * @param groupId The group id where the members are counted.
         * @param timeout A member is counted if heartbeat was recevied from it in the last "timeout" msec
         * @return
         */
        uint16_t GetGroupMemberCount(const riaps::groups::GroupId &groupId,
                                     int64_t timeout = 1000 * 15 /*15 sec in msec*/);

        std::string GetLeaderId(const riaps::groups::GroupId& groupId);

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
        bool JoinGroup(riaps::groups::GroupId&  groupId);

        /**
         * The component leaves the given group.
         * @param groupId The id of the group to be left.
         */
        bool LeaveGroup(riaps::groups::GroupId&& groupId);
        bool LeaveGroup(riaps::groups::GroupId&  groupId);

        std::vector<riaps::groups::GroupId> GetGroupMemberships();
        std::vector<riaps::groups::GroupId> GetGroupMembershipsByType(const std::string& groupType);
        bool IsMemberOf(riaps::groups::GroupId& groupId);

        virtual void OnPropose (riaps::groups::GroupId& groupId, const std::string& proposeId, capnp::FlatArrayMessageReader& message);
        virtual void OnActionPropose (riaps::groups::GroupId& groupId,
                                      const std::string& proposeId,
                                      const std::string& actionId,
                                      const timespec& timePoint);

        virtual void OnMessageToLeader(const riaps::groups::GroupId& groupId, capnp::FlatArrayMessageReader& message);
        virtual void OnMessageFromLeader(const riaps::groups::GroupId& groupId, capnp::FlatArrayMessageReader& message);

        virtual void OnAnnounce(const riaps::groups::GroupId& groupId, const std::string& proposeId, bool accepted);
        std::string SendPropose(const riaps::groups::GroupId& groupId, capnp::MallocMessageBuilder& message);
        bool SendVote(const riaps::groups::GroupId& groupId, const std::string& proposeId, bool accept);

        /**
         * Proposes an action to the leader.
         * @param groupId
         * @param actionId The action to be performed. Developer-generated ID, each component knows the appropriate function to be called.
         * @param relTime  Relative time (now + tv_sec + tv_nsec later), when the action must be called.
         * @param message  Additional custom message, which necesseary by the other nodes for the decision.
         * @return The generated proposeId. The leader announces the results by this id.
         */
        std::string ProposeAction(const riaps::groups::GroupId& groupId  ,
                                  const std::string&            actionId ,
                                  const timespec&               absTime
        );
        ///@}

        uint64_t ScheduleAbsTimer(const timespec &t, uint64_t wakeupOffset = 0 /*nanosec*/);

        /**
         *
         * @param tp absolute time when the action is executed
         * @param action Pointer to the function to be executed
         * @param wakeupOffset The timer is fired earlier by wakeupOffset.
         *                     The value is in nanosec and depends on the current platform.
         *                     On BBB it is 2000 microsec, this is the worst case.
         *                     If the timer fires too early, make sure to call the WaitUntil() in the handler.
         * @return unique id of the scheduled timer
         */
        uint64_t ScheduleAction(const timespec &tp, std::function<void(const uint64_t)> action,
                                uint64_t wakeupOffset = 2000 * 1000);


        std::function<void(const uint64_t)> scheduled_action_;

        void set_config(ComponentConf& c_conf);



    private:
        std::shared_ptr<spd::logger> riaps_logger_;
        std::shared_ptr<spd::logger> component_logger_;
        std::string                  component_logger_name_;

        const ports::PublisherPort*  InitPublisherPort  (const ComponentPortPub&);
        const ports::SubscriberPort* InitSubscriberPort (const ComponentPortSub&);
        const ports::ResponsePort*   InitResponsePort   (const ComponentPortRep&);
        const ports::RequestPort*    InitRequestPort    (const ComponentPortReq&);
        const ports::QueryPort*      InitQueryPort      (const ComponentPortQry&);
        const ports::AnswerPort*     InitAnswerPort     (const ComponentPortAns&);
        const ports::PeriodicTimer*  InitTimerPort      (const ComponentPortTim&);
        const ports::InsidePort*     InitInsidePort     (const ComponentPortIns&);



        //std::string             getTimerChannel();
        std::string             getOneShotTimerChannel();

        // TODO: uniqueptr
        // Note: disable for now, we need more tests.
        //timers::OneShotTimer*   _oneShotTimer;

        ComponentConf component_config_;


        // All the component ports
        std::unordered_map<std::string, std::unique_ptr<ports::PortBase>> ports_;

        std::map<riaps::groups::GroupId,
                 std::unique_ptr<riaps::groups::Group>> groups_;

        riaps::groups::Group* getGroupById(const riaps::groups::GroupId &groupId);

        uint64_t timer_counter_;

        /**
         * Unique ID of the componenet. Regenerated at every start.
         */
        zuuid_t* component_uuid_;

        /**
         * Points to the component owner.
         */
        std::shared_ptr<PyActor> actor_;
        //const Actor* actor_;

        /**
         * Holds the component thread.
         */
        zactor_t* component_zactor_;

        bool has_security_;
    };

    template<class T>
    T* ComponentBase::GetPortAs(const std::string& portName) {
        ports::PortBase* portBase = GetPortByName(portName);
        if (portBase == nullptr) return nullptr;
        return dynamic_cast<T*>(portBase);
    }
}


#endif //RIAPS_R_COMPONENTBASE_H
