//
// Created by istvan on 9/30/16.
//

#include <componentmodel/r_actor.h>

int main(){
    riaps::Actor a("ActorProducer");
    a.start("prod_config.json");

    return 0;
}