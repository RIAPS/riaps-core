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

struct MessageToLeader {
    sourceComponentId @0 : Text;
}

struct DistrCoord {
    sourceComponentId @0 : Text;
    proposeToLeader   @1 : ProposeToLeader;
    proposeToSlaves   @2 : ProposeToSlaves;
    voteForLeader     @3 : VoteForLeader;
    announce          @4 : Announce;

    struct ProposeToLeader {
        proposeId @0 : Text;
    }

    struct ProposeToSlaves {
        proposeId @0 : Text;
        leaderId  @1 : Text;
    }

    struct VoteForLeader {
        proposeId  @0 : Text;
        leaderId   @1 : Text;
        voteResult @2 : VoteResults;
    }

    struct Announce {
        proposeId  @0 : Text;
        voteResult @1 : VoteResults;
    }

    enum VoteResults { accepted @0; rejected @1; }
}

struct GroupInternals {
    union{
        groupHeartBeat    @0 : GroupHeartBeat;
        leaderElection    @1 : LeaderElection;
        messageToLeader   @2 : MessageToLeader;
        distrCoord        @3 : DistrCoord;
    }
}
