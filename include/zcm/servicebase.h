//
// Created by parallels on 9/5/16.
//

#ifndef SERVICEBASE_H
#define SERVICEBASE_H

#include <czmq.h>
#include "../r_framework.h"
#include <iostream>

namespace zcm {


    class servicebase{
    public:
        servicebase(std::string name);
        ~servicebase();

        /**
         * @brief Returns the service name
         * @return Service name
         */
        std::string get_name();

    protected:


        /** @brief Name of the service */
        std::string name;

    private:
        zactor_t* servicecheck;
    };

};

#endif //SERVICEBASE_H
