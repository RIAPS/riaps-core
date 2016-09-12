#include <czmq.h>
#include <string>
#include <iostream>

int main(int argc, char* argv[]) {

    std::cout << argv[0] << std::endl;

//  Test peer-to-peer operations
    zactor_t *base = zactor_new (zgossip, (void*)"base");
    assert (base);
    
    zstr_send (base, "VERBOSE");
    //  Set a 100msec timeout on clients so we can test expiry
    zstr_sendx (base, "SET", "server/timeout", "100", NULL);
    zstr_sendx (base, "BIND", "tcp://*:9999", NULL);

    if (argc>1){
      std::string target("tcp://" + std::string(argv[1]));
      

      zstr_sendx (base, "CONNECT", target.c_str(), NULL);  
    }

    //zactor_t *alpha = zactor_new (zgossip, (void*)"alpha");
    //assert (alpha);
    //zstr_send (alpha, "VERBOSE");
    //zstr_sendx (alpha, "CONNECT", "inproc://base", NULL);
    //zstr_sendx (alpha, "PUBLISH", "random string1", "service1", NULL);
    //zstr_sendx (alpha, "PUBLISH", "random string2", "service2", NULL);

    //zactor_t *beta = zactor_new (zgossip, (void*)"beta");
    //assert (beta);
    //zstr_send (beta, "VERBOSE");
    //zstr_sendx (beta, "CONNECT", "inproc://base", NULL);
    //zstr_sendx (beta, "PUBLISH", "random string4", "service1", NULL);
    //zstr_sendx (beta, "PUBLISH", "random string3", "service2", NULL);

    //  got nothing
    zclock_sleep (200);

    int i =0;

    while (!zsys_interrupted) {
        char message [1024];
        if (!fgets (message, 1024, stdin))
            break;
        message [strlen (message) - 1] = 0;
        
        zstr_sendx (base, "PUBLISH", message, "service1", NULL);
    }

    zactor_destroy (&base);
    //zactor_destroy (&alpha);
    //zactor_destroy (&beta);

    //  @end
    printf ("OK\n");

}