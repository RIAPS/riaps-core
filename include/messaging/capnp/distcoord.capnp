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

struct LeaderHeartBeat {
    sourceComponentId @0 : Text;
}

struct RequestForVoteReq {
    sourceComponentId @0 : Text;
}

struct RequestForVoteRep {
    sourceComponentId @0 : Text;
}

struct GroupInternals {
    union{
        groupHeartBeat    @0 : GroupHeartBeat;
        leaderHeartBeat   @1 : LeaderHeartBeat;
        requestForVoteReq @2 : RequestForVoteReq;
        requestForVoteRep @3 : RequestForVoteRep;
    }
}
