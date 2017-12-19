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
    # Component ID of the sender
    sourceComponentId @0 : Text;

    # Component ID of the candidate
    voteForId         @1 : Text;
}

struct LeaderElection {
        leaderHeartBeat   @0 : LeaderHeartBeat;
        requestForVoteReq @1 : RequestForVoteReq;
        requestForVoteRep @2 : RequestForVoteRep;
}

struct GroupInternals {
    union{
        groupHeartBeat    @0 : GroupHeartBeat;
        leaderElection    @1 : LeaderElection;
    }
}
