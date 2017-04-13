@0xbfba33b11019ca1c;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("testing::messages");

struct SensorValue {
   msg @0 : Text;
 }
