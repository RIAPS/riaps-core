@0xa5b4ccd232a91945;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("riaps::distrcoord");

enum HeartBeatType { ping @0; pong @1; }

struct GroupHeartBeat {
  heartBeatType     @0 : HeartBeatType;
  sourceComponentId @1 : Text;
}




struct LeaderElection {
        appendEntry       @0 : AppendEntry;
        requestForVoteReq @1 : RequestForVoteReq;
        requestForVoteRep @2 : RequestForVoteRep;

        struct AppendEntry {
            sourceComponentId @0 : Text;
            electionTerm      @1 : UInt32;
        }

        struct RequestForVoteReq {
            sourceComponentId @0 : Text;
            electionTerm      @1 : UInt32;
        }

        struct RequestForVoteRep {
            # Component ID of the sender
            sourceComponentId @0 : Text;

            # Component ID of the candidate
            voteForId         @1 : Text;

            electionTerm      @2 : UInt32;
        }

}

struct GroupInternals {
    union{
        groupHeartBeat    @0 : GroupHeartBeat;
        leaderElection    @1 : LeaderElection;
    }
}
