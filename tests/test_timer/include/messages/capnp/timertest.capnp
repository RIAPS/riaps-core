@0xbf2613a11018ba1c;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("timertest::messages");

struct SignalValue {
  val @0 : Float32;
 }