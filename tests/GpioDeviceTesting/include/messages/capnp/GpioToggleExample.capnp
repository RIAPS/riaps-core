@0xbfb633a11018cb2c;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("gpiotoggleexample::messages");

struct DataValue {
  value @0 : UInt8;
 }

struct ReadRequest {
   msg @0 : Text;
 }

 struct WriteRequest {
    value @0 : UInt8;
 }