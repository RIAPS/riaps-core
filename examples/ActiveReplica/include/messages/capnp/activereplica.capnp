@0xcda54ca269a5ad1a;


using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("activereplica::messages");

struct SensorReady {
  msg @0 : Text;
 }

struct SensorValue {
   value @0 : Float64;
 }

 struct SensorQuery {
    msg @0 : Text;
 }

 struct Estimate {
    id     @0    : UInt32;
    values @1    : List(Float64);
 }

 struct QueryRequest {
    id @0 : UInt32;
 }