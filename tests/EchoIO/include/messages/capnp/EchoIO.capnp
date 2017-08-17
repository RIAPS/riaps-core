@0xbfb641a12011cb2c;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("echoio::messages");

struct EchoReq {
  value @0 : Text;
 }

struct EchoRep {
   msg @0 : Text;
 }