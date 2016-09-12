//
// Created by parallels on 9/6/16.
//

#include "actor_mock.h"
#include <czmq.h>

#include <iostream>

int main(){
    actor_mock a;
    a.init();
    a.start();

    return 0;
}