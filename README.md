# RIAPS Framework in C++

## About

This repository contains two major parts of the RIAPS framework:

* Discovery service
* C++ component framework

## Install `riaps-core`

You can install the `riaps-core` and all dependencies from our apt repository. The instructions to setup of the apt repository is located [here](https://github.com/RIAPS/riaps.github.io/blob/develop/distro.md). If you prefer to install the `riaps-core` manually please read the [wiki](https://github.com/RIAPS/riaps-core/wiki/Build-and-Install-riaps-core).
 
## The Discovery Service

The discovery service is responsible to save/maintain/retrieve the registered RIAPS services. The assumption here is that service providers (e.g.: publishers) and consumers (e.g.: subscribers) don't know the location or identity of the other party. The discovery service is able to provide connection details for the components. Main features of the discovery service:

* **Register service:** When the application starts, the components register their service details (ip address, port, message type, communication protocol) in the discovery service. The discovery service distributes this information among other discovery services in the cluster.
* **Look for registered services:** The components ask connection details to a service (based on message type and communication protocol). The discovery service retrieves the connection details (ip address and port).
* **Discover other nodes:** When a new node joins to the network the discovery service invites it to the cluster and shares the registered services.
* **Notifications:** If a new provider (e.g.: publisher) is registered, the interested components (e.g.: subscribers) are notified about the new provider.

## The C++ component framework

The component framework provides a base class (`ComponentBase`) for your components. The `ComponentBase` implements functions which cover several use cases in distributed systems. The following main functions and services are available:

* Registers services in the discovery service. It happens automatically, based on the application model file :exclamation: LINK :exclamation:
* Connects the components to the available services. (Also happens automatically, based on the application model file).
* Dispatcher: the incoming messages are automatically dispatched to the component message handler (based on the message type).
* Messaging patterns: publish-subscribe, request-response, query-answer.
* Dynamic group formation: components are able to form groups dynamically.
* Messaging between group members.
* Leader election: group members are able to elect a leader component using modified [RAFT](https://raft.github.io/) leader election.
* Messaging between the leader and the group members.
* Consensus: components can propose values to the leader, other components can agree/disagree with the proposed values. Leader announces the result of the voting.
* Time-synchronized coordinated action: components are agreeing on executing an action at the proposed time, each component schedules the action and executes.