//
// Created by istvan on 11/11/16.
//

#include "loggerd/r_loggerd.h"

int main()
{

    /// Initialize ZLOG logger
    zsys_set_logident ("riaps discovery log");
    zsys_set_logsender (RIAPS_LOG_DISCOVERY);

    void *logger = zsys_socket (ZMQ_SUB, NULL, 0);
    assert (logger);
    int rc = zmq_connect (logger, RIAPS_LOG_DISCOVERY);
    assert (rc == 0);
    rc = zmq_setsockopt (logger, ZMQ_SUBSCRIBE, "", 0);
    assert (rc == 0);

    zsock_set_rcvtimeo(logger, 1000);

    while (!zsys_interrupted){
        char *received = zstr_recv (logger);

        if (received!=NULL) {
            std::cout << "Alma";
            std::cout << received << std::endl << std::flush;

            assert (received);
            zstr_free(&received);
        }

    }

    zsys_close(logger, NULL, 0);

    return 0;
}