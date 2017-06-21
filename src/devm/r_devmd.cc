//
// Created by istvan on 5/4/17.
//

#include <devm/r_devm_actor.h>
#include <czmq.h>
#include <string>
#include <iostream>
#include <framework/rfw_network_interfaces.h>


int main() {
    std::cout << "Starting RIAPS DEVICE MANAGER SERVICE " << std::endl;


    zactor_t *r_actor = zactor_new(devm_zactor, NULL);

    while (!zsys_interrupted){
        zclock_sleep(100);
    }

    zactor_destroy(&r_actor);
    zclock_sleep(1000);
}