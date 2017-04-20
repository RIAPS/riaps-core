@0xbf2613a11018ba1c;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("timertest::messages");

struct TimeStamp {
    sec @0 : Int64;
    nsec  @1 : Int64;
}

struct SignalValue {
  val        @0 : Float32;
  timestamp @1 : TimeStamp;
 }