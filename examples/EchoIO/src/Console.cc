//
// Created by istvan on 6/20/17.
//

#include <Console.h>


#include <czmq.h>

#include <string>
#include <iostream>

int main() {
    std::string targetAddress = "tcp://127.0.0.1:" + std::string(port);
    std::cout << "Connecting to " << targetAddress <<std::endl;
    zsock_t* reqPort = zsock_new(ZMQ_REQ);
    int rc = zsock_connect(reqPort, targetAddress.c_str());

    assert(rc==0);

    std::cout << "Connected" << std::endl;

    while (!zsys_interrupted) {
        std::cout << "> ";
        std::string input;
        std::cin >> input;

        zmsg_t *msg = zmsg_new();
        zmsg_addstr(msg, input.c_str());

        int rc = zmsg_send(&msg, reqPort);
        assert(rc == 0);

        msg = zmsg_recv(reqPort);
        char *output = zmsg_popstr(msg);
        std::cout << "< " << output << std::endl;
        zstr_free(&output);
        zmsg_destroy(&msg);
    }

    zsock_destroy(&reqPort);


}