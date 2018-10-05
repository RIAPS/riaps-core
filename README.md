# RIAPS Framework in C++

## About

This repository contains two major parts of the RIAPS framework:

* Discovery service
* C++ component framework

## Install `riaps-core`

For detailed instructions please check the [wiki](https://github.com/RIAPS/riaps-core/wiki/Build-and-Install-riaps-core).
 
## The Discovery Service

The discovery service is responsible to save/maintain/retrieve the registered RIAPS services. The assumption here is that service providers (e.g.: publishers) and consumers (e.g.: subscribers) don't know the location or identity of the other party. The discovery service is able to provide connection details for the components. Main features of the discovery service:

* **Register service:** When the application starts, the components register their service details (ip address, port, message type, communication protocol) in the discovery service. The discovery service distributes this information among other discovery services in the cluster.
* **Look for registered services:** The components ask connection details to a service (based on message type and communication protocol). The discovery service retrieves the connection details (ip address and port).
* **Discover other nodes:** When a new node joins to the network the discovery service invites it to the cluster and shares the registered services.
* **Notifications:** If a new provider (e.g.: publisher) is registered, the interested components (e.g.: subscribers) are notified about the new provider.

## The C++ component framework

The component framework provides a base class (`ComponentBase`) for your components. The `ComponentBase` implements functions which cover several use cases in distributed systems.
