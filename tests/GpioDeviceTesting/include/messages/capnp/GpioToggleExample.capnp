@0xbfb633a11018cb2c;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("gpiotoggleexample::messages");

struct DataValue {
  msg @0 : Text;
 }

struct ReadRequest {
   msg @0 : Text;
 }

 struct WriteRequest {
    msg @0 : Text;
 }