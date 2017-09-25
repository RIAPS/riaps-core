/*!
 * RIAPS ComponentBase
 *
 * \brief Parent class of all RIAPS components. Controls the communication ports, messaging and timers.
 *
 * \author Istvan Madari
 */


#ifndef RIAPS_R_COMPONENTBASE_H
#define RIAPS_R_COMPONENTBASE_H

#include "r_discoverdapi.h"
#include "r_configuration.h"
#include "r_periodictimer.h"
#include "r_actor.h"
#include "r_messagebase.h"
#include "r_oneshottimer.h"
#include "r_groupbase.h"



#include <msgpack.hpp>
#include <capnp/message.h>
#include <capnp/serialize.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <queue>
#include <random>
#include <ctime>


#include "r_subscriberport.h"
#include "r_responseport.h"
#include "r_requestport.h"
#include "r_insideport.h"
#include "r_portregister.h"

namespace riaps {

    class Actor;



    /**
     * @brief Declare the component thread fn.
     * @param pipe ZMQ pair socket to send/recv messages from the component thread.
     * @param args Component instance is passed to the component thread.
     */
    void component_actor(zsock_t* pipe, void* args);

    typedef void (riaps::ComponentBase::*riaps_handler)(const std::string&, msgpack::sbuffer*, riaps::ports::PortBase*);

    class ComponentBase : private PortRegister {
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
        bool SendMessageOnPort(capnp::MallocMessageBuilder& message, const std::string& portName);

        /**
         * @brief Stops the component
         *
         * Stops the component in the following order: 1) Stops the timers 2) Destroys the component thread
         * 3) Deletes ports
         */
        void StopComponent();

        /**
         *
         * @return The component configuration.
         */
        const component_conf& GetConfig() const;



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


        const ports::SubscriberPort* InitSubscriberPort (const _component_port_sub&);
        const ports::ResponsePort*   InitResponsePort   (const _component_port_rep&);
        const ports::RequestPort*    InitRequestPort    (const _component_port_req&);
        const ports::PeriodicTimer*  InitTimerPort      (const _component_port_tim&);
        const ports::InsidePort*     InitInsiderPort    (const _component_port_ins&);

        const ports::PortBase* GetPort(std::string portName) const;

        /**
         * @return Returns the details of the group types.
         */
        const groupt_conf& GetGroupTypeConfig() const;


        //
        //virtual void RegisterHandler(const std::string& portName, riaps_handler);

        /**
         * Sends a ZMQ message on the given port.
         *
         * @param message ZMQ message structure to be sent
         * @param portName The port, which sends the message.
         * @return True if the message was sent successfully.
         */
        bool SendMessageOnPort(zmsg_t** message, const std::string& portName);

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
                                     ports::PortBase* port) = 0;

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
        void JoinToGroup(const std::string& groupType, const std::string& groupName);




    private:


        std::string             GetTimerChannel();
        std::string             GetCompUuid();

        // Note: disable for now, we need more tests.
        //std::string             GetOneShotTimerChannel();

        // TODO: uniqueptr
        // Note: disable for now, we need more tests.
        //timers::OneShotTimer*   _oneShotTimer;

        component_conf _configuration;



        //riaps::groups::Groups _groups;
    };
}




#endif //RIAPS_R_COMPONENTBASE_H
