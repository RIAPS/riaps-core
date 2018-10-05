@0xbfb633a11019ca1c;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("leaderelection::messages");

struct LeaderMessage {
  msg @0 : Text;
 }