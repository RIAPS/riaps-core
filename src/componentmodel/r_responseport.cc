//
// Created by parallels on 9/29/16.
//

#include <componentmodel/r_network_interfaces.h>
#include <componentmodel/r_responseport.h>

namespace riaps{
    namespace ports{

        ResponsePort::ResponsePort(const _component_port_rep_j &config, ComponentBase *parent_component) :
            PortBase(PortTypes::Response, (component_port_config*)&config)
        {
            _port_socket = zsock_new(ZMQ_REP);
            _host = GetIPAddress();

            if (_host == "") {
                throw std::runtime_error("Response cannot be initiated. Cannot find  available network interface.");
            }

            std::string rep_endpoint = "tcp://" + _host + ":!";
            _port = zsock_bind(_port_socket, rep_endpoint.c_str());


            if (_port == -1) {
                throw std::runtime_error("Couldn't bind response port.");
            }

            std::cout << "Response is created on : " << _host << ":" << _port << std::endl;


            if (!register_service(parent_component->GetActor()->GetApplicationName(),
                                  config.messageType,
                                  _host,
                                  _port,
                                  Kind::REP,
                                  (config.isLocal?Scope::LOCAL:Scope::GLOBAL),
                                  {})) {
                throw std::runtime_error("Response port couldn't be registered.");
            }
        }

        const _component_port_rep_j* ResponsePort::GetConfig() const{
            return (_component_port_rep_j*)GetPortBaseConfig();
        }

        bool ResponsePort::Send(zmsg_t** msg) const {
            zmsg_pushstr(*msg, GetConfig()->rep_type.c_str());

            int rc = zmsg_send(msg, _port_socket);
            assert(rc == 0);
        }

        bool ResponsePort::Send(std::string msg) const {
            zmsg_t* zmsg = zmsg_new();
            zmsg_addstr(zmsg, msg.c_str());

            return Send(&zmsg);
        }

        ResponsePort* ResponsePort::AsResponsePort() {
            return this;
        }

    }
}

/*
namespace riaps{


    std::unique_ptr<ResponsePort> ResponsePort::InitFromConfig(response_conf& config) {
        std::unique_ptr<ResponsePort> result(new ResponsePort());

        result->configuration = config;
        result->port_socket = zsock_new(ZMQ_REP);

        if (config.port == 0) {
            result->port = zsock_bind(result->port_socket, "tcp://*:!");
        } else {
            char tmp[256];
            sprintf(tmp, "tcp://*:%d", config.port);
            result->port = zsock_bind(result->port_socket, tmp);
        }

        result->endpoint = std::string(zsock_endpoint(result->port_socket));

        std::cout << "ResponsePort is created on port: " << result->port << std::endl;

        std::string ifaddress = result->GetInterfaceAddress(config.network_iface);

        // TODO: error handling
        // NOTE: Should be separated form construct
        if (ifaddress!="") {

            std::cout << "Registering response port" << std::endl;

            // TODO: Add tags
            //register_service(config.servicename, config.servicename, ifaddress, std::to_string(result->port), {});
        }

        return std::move(result);
    }


    ResponsePort::ResponsePort(){

    }

    //ResponsePort::ResponsePort(response_conf &config) {
//
    //}

    response_conf ResponsePort::GetConfig() {
        return configuration;
    }

    std::string ResponsePort::GetEndpoint() {
        if (port_socket) {
            return std::string(zsock_endpoint(port_socket));
        }
        return "";
    }

    void ResponsePort::SendMessage(zmsg_t **msg) {
        int rc = zmsg_send(msg, port_socket);
        assert(rc==0);
    }

    ResponsePort::~ResponsePort() {
        deregister_service(configuration.servicename);
    }
}*/