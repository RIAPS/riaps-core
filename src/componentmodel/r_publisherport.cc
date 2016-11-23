//
// Created by parallels on 9/6/16.
//

#include <componentmodel/r_publisherport.h>

namespace riaps{

    // TODO: Pass scope
    PublisherPort::PublisherPort(_component_port_pub_j& config,  ComponentBase* parent_component) {
        _configuration = config;
        _port_socket = zsock_new(ZMQ_PUB);

        std::string _host = GetInterfaceAddress();
        if (_host==""){
            throw std::runtime_error("Publisher cannot be initiated. Cannot find  available network interface.");
        }

        std::string pub_endpoint = "tcp://" + _host + ":!";
        _port = zsock_bind(_port_socket, pub_endpoint.c_str());

        if (_port == -1){
            throw std::runtime_error("Couldn't bind publisher port.");
        }


        //_port_socket = zsock_new_pub(pub_endpoint.c_str());

        //zsock_endpoint(_port_socket);

        /*if (config.port == 0) {

        } else {
            char tmp[256];
            sprintf(tmp, "tcp://*:%d", config.port);
            _port = zsock_bind(_port_socket, tmp);

        }*/

        //zsock_

        std::cout << "Publisher is created on : " << _host << " " << _port << std::endl;


        if (!register_service(parent_component->GetActor()->GetApplicationName(), config.message_type, _host, _port, Kind::PUB, Scope::GLOBAL, {})){
            throw std::runtime_error("Publisher port couldn't be registered.");
        }

    }

    //void PublisherPort::SendMessage() {
//
    //}

    _component_port_pub_j PublisherPort::GetConfig() {
        return _configuration;
    }

    std::string PublisherPort::GetEndpoint() {
        if (_port_socket) {
            return std::string(zsock_endpoint(_port_socket));
        }
        return "";
    }

    void PublisherPort::Send(zmsg_t* msg) const {
        int rc = zmsg_send(&msg, _port_socket);
        assert(rc==0);
    }

    PublisherPort::~PublisherPort() {
        if (_discovery_port!=NULL){
            zsock_destroy(&_discovery_port);
        }
        //deregister_service(_configuration.message_type);
    }
}