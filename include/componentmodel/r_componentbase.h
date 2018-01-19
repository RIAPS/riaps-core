/**
 * RIAPS ComponentBase
 *
 * @brief Parent class of all RIAPS components. Controls the communication ports, messaging, timers, groups.
 *
 * @author Istvan Madari
 */


#ifndef RIAPS_R_COMPONENTBASE_H
#define RIAPS_R_COMPONENTBASE_H

#include <componentmodel/r_discoverdapi.h>
#include <componentmodel/r_configuration.h>
#include <componentmodel/r_periodictimer.h>
#include <componentmodel/r_actor.h>
#include <componentmodel/r_messagebase.h>
#include <componentmodel/r_oneshottimer.h>
#include <componentmodel/r_payload.h>
#include <groups/r_group.h>
#include <messaging/disco.capnp.h>

#include <msgpack.hpp>
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <spdlog/spdlog.h>

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


namespace spd = spdlog;


namespace riaps {

    class Actor;

    namespace groups{
        class Group;
        struct GroupId;
    }



    /**
     * @brief Declare the component thread fn.
     * @param pipe ZMQ pair socket to send/recv messages from the component thread.
     * @param args Component instance is passed to the component thread.
     */
    void component_actor(zsock_t* pipe, void* args);

    typedef void (riaps::ComponentBase::*riaps_handler)(const std::string&, msgpack::sbuffer*, riaps::ports::PortBase*);

    class ComponentBase {
        friend riaps::groups::Group;
    public:

        /// @param config Configuration, parsed from the model file.
        /// @param actor Parent actor, the owner the component.
        ComponentBase(component_conf& config, Actor& actor);

        /// @return The owner actor.
        const Actor* GetActor() const;

        zactor_t* GetZmqPipe() const;

        friend void component_actor(zsock_t* pipe, void* args);


        // Obsolote
        //bool SendMessageOnPort(std::string message, const std::string& portName);
        //bool SendMessageOnPort(msgpack::sbuffer& message, const std::string& portName);
        //bool SendMessageOnPort(MessageBase* message, const std::string& portName);

        /**
         * @brief Sends capnp message on port.
         *
         * Copies the capnp message buffer into a ZMQ message frame and sends the message on port, identified by portName.
         * @param message The capnp buffer that holds the message
         * @param portName The message is sent on this port. Available names are declared in the riaps model.
         * @return True if the message is sent.
         */
        bool SendMessageOnPort(capnp::MallocMessageBuilder& message,
                               const std::string& portName);

        /**
         * Sends message on answer port
         *
         * @return
         */
        bool SendMessageOnPort(capnp::MallocMessageBuilder& message,
                               const std::string& portName,
                               std::shared_ptr<riaps::MessageParams> params);

        /**
         * Sends a message on query port
         *
         */
        bool SendMessageOnPort(capnp::MallocMessageBuilder& message,
                               const std::string&           portName,
                               std::string&                 requestId);



        bool SendGroupMessage(const riaps::groups::GroupId& groupId,
                              capnp::MallocMessageBuilder& message,
                              const std::string& portName);

        bool SendGroupMessage(const riaps::groups::GroupId&& groupId,
                              capnp::MallocMessageBuilder& message,
                              const std::string& portName);



        /**
         * @brief Stops the component
         *
         * Stops the component in the following order:
         *   1) Stops the timers
         *   2) Destroys the component thread
         *   3) Deletes ports
         */
        void StopComponent();

        void UpdateGroup(riaps::discovery::GroupUpdate::Reader& msgGroupUpdate);





        /**
         *
         * @return The component configuration.
         */
        const component_conf& GetConfig() const;

        /**
         *
         * @return The component unique ID.
         */
        const std::string GetCompUuid() const;


        /**
         * @brief Debug function. Prints the details of the given port to the standard output.
         *
         * The output: <direction><componentType>::<portName>:messageType->message
         */
        virtual void PrintMessageOnPort(ports::PortBase* port, std::string message="");

        /**
         * @brief For debugging. Prints all the commandline parameters of the component.
         *
         */
        virtual void PrintParameters();

        virtual ~ComponentBase();

    protected:
        std::shared_ptr<spd::logger> _logger;


        /**
         * Sends a ZMQ message on the given inside port. This Send() is just for InsidePorts
         *
         * @param message ZMQ message structure to be sent
         * @param portName The port, which sends the message.
         * @return True if the message was sent successfully.
         */
        bool SendMessageOnPort(zmsg_t** message, const std::string& portName);

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

        virtual void OnMessageFromLeader(const riaps::groups::GroupId& groupId,
                                         capnp::MallocMessageBuilder& message);
        

        /**
         * Gives a snapshot about the members of a group, considering a timeout.
         * @param groupId The group id where the members are counted.
         * @param timeout A member is counted if heartbeat was recevied from it in the last "timeout" msec
         * @return
         */
        uint16_t GetGroupMemberCount(const riaps::groups::GroupId& groupId,
                                     const int64_t timeout = 1000*15 /*15 sec in msec*/);

        std::string GetLeaderId(const riaps::groups::GroupId& groupId);
//
//        virtual bool SendGroupMessage(riaps::groups::GroupId&      groupId,
//                                      capnp::MallocMessageBuilder& messageBuilder,
//                                      const std::string&           portName) = 0;

        /**
         * Search publisher port with portName.
         *
         * @param portName The name of the publisher port to be retrieved.
         * @return NULL if the port with the name wasn't found or the port is not a publisher port.
         */
        ports::PublisherPort*  GetPublisherPortByName(const std::string& portName);

        /**
         * Search request port with portName.
         *
         * @param portName The name of the request port to be retrieved.
         * @return NULL if the port with the name wasn't found or the port is not a request port.
         */
        ports::RequestPort*    GetRequestPortByName(const std::string& portName);

        ports::QueryPort*      GetQueryPortByName(const std::string& portName);

        /**
         * Search response port with portName.
         *
         * @param portName The name of the response port to be retrieved.
         * @return NULL if the port with the name wasn't found or the port is not a response port.
         */
        ports::ResponsePort*   GetResponsePortByName(const std::string& portName);

        /**
         * Search subscriber port with portName.
         *
         * @param portName The name of the subscriber port to be retrieved.
         * @return NULL if the port with the name wasn't found or the port is not a subscriber port.
         */
        ports::SubscriberPort* GetSubscriberPortByName(const std::string& portName);

        /**
         * Search a port with the given name.
         *
         * @return NULL if there is no port with the given name.
         */
        ports::PortBase* GetPortByName(const std::string&);

        //std::shared_ptr<spd::logger> GetConsoleLogger();

        void SetDebugLevel(std::shared_ptr<spd::logger> logger, spd::level::level_enum level);

        // Note: disable for now, we need more tests.
        //bool CreateOneShotTimer(const std::string& timerid, timespec& wakeuptime);
        //virtual void OnOneShotTimer(const std::string& timerid)=0;

/*        virtual void DispatchMessage(const std::string& messagetype,
                                     msgpack::sbuffer* message,
                                     ports::PortBase* port);
*/
        /*virtual void DispatchMessage(const std::string& messagetype,
                                     riaps::MessageBase* message,
                                     ports::PortBase* port) = 0;
*/

//        virtual void DispatchMessage(const std::string& messagetype,
//                                     kj::ArrayPtr<const capnp::word>* data,
//                                     ports::PortBase* port) = 0;

//        virtual void DispatchMessage(const std::string& messagetype,
//                                     capnp::FlatArrayMessageReader* capnpreader,
//                                     ports::PortBase* port) = 0;


        /**
         * Converts and forwards the incoming capnp message to the appropriate handler. Pure virtual function,
         * the inherited class must know the possible message types.
         *
         * @param capnpreader The capnp buffer holding the incoming message
         * @param port The port where the message arrived.
         */
        virtual void DispatchMessage(capnp::FlatArrayMessageReader* capnpreader,
                                     ports::PortBase* port,
                                     std::shared_ptr<MessageParams> params = nullptr) = 0;

        /**
         * Forwards the given ZMQ message to the appropriate handler. Used for inside ports only in device components.
         * @param zmsg
         * @param port
         */
        virtual void DispatchInsideMessage(zmsg_t* zmsg,
                                           ports::PortBase* port) = 0;

        /**
         * Points to the component owner.
         */
        const Actor*      _actor;

        /**
         * Unique ID of the componenet. Regenerated at every start.
         */
        zuuid_t*          _component_uuid;

        /**
         * Holds the component thread.
         */
        zactor_t*         _zactor_component;

        /**
         *
         * @param groupType
         * @param groupName
         */
        bool JoinToGroup(riaps::groups::GroupId&& groupId);
        bool JoinToGroup(riaps::groups::GroupId&  groupId);


        virtual void OnPropose (riaps::groups::GroupId& groupId, const std::string& proposeId, capnp::FlatArrayMessageReader& message);
        virtual void OnAnnounce(const riaps::groups::GroupId& groupId, const std::string& proposeId, bool accepted);
        std::string SendPropose(const riaps::groups::GroupId& groupId, capnp::MallocMessageBuilder& message);
        bool SendVote(const riaps::groups::GroupId& groupId, const std::string& proposeId, bool accept);

    private:

        const ports::PublisherPort*  InitPublisherPort  (const _component_port_pub&);
        const ports::SubscriberPort* InitSubscriberPort (const _component_port_sub&);
        const ports::ResponsePort*   InitResponsePort   (const _component_port_rep&);
        const ports::RequestPort*    InitRequestPort    (const _component_port_req&);
        const ports::QueryPort*      InitQueryPort      (const _component_port_qry&);
        const ports::AnswerPort*     InitAnswerPort     (const _component_port_ans&);
        const ports::PeriodicTimer*  InitTimerPort      (const _component_port_tim&);
        const ports::InsidePort*     InitInsidePort     (const _component_port_ins&);


        std::string             GetTimerChannel();


        // Note: disable for now, we need more tests.
        //std::string             GetOneShotTimerChannel();

        // TODO: uniqueptr
        // Note: disable for now, we need more tests.
        //timers::OneShotTimer*   _oneShotTimer;

        component_conf _configuration;


        // All the component ports
        std::unordered_map<std::string, std::unique_ptr<ports::PortBase>> _ports;

        std::map<riaps::groups::GroupId,
                 std::unique_ptr<riaps::groups::Group>> _groups;

        riaps::groups::Group* GetGroupById(const riaps::groups::GroupId& groupId);
    };
}




#endif //RIAPS_R_COMPONENTBASE_H
