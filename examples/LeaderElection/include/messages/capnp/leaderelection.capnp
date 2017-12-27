@0xbfb633a11019ca1c;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("leaderelection::messages");

struct Dummy {
  msg @0 : Text;
 }