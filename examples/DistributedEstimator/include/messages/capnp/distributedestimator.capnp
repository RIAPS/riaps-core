@0xbfb633a11018ca1c;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("distributedestimator::messages");

struct SensorReady {
  msg @0 : Text;
 }

struct SensorValue {
   msg @0 : Text;
 }

 struct SensorQuery {
    msg @0 : Text;
 }

 struct Estimate {
    msg    @0    : Text;
    values @1    : List(Float64);
 }