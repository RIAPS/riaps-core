@0xbfb633a11019ca1c;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("dc::messages");

struct AgreeOnThis {
  value @0 : Text;
 }