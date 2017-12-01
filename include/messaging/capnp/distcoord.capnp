@0xa5b4ccd232a91945;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("riaps::distrcoord");

enum HeartBeatType { ping @0; pong @1; }

struct GroupHeartBeat {
  heartBeatType     @0 : HeartBeatType;
  sourceComponentId @1 : Text;
  # debug pingpong sequences
  # sequenceNumber    @2 : UInt16;
}

