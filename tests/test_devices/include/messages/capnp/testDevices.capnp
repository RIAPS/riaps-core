@0xbfb633a11018cb2c;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("testdevices::messages");

struct PubDeviceData {
  msg @0 : Text;
 }

struct SubDeviceData {
   msg @0 : Text;
 }

 struct ComponentData {
    msg @0 : Text;
 }