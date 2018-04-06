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

struct Consensus {
    sourceComponentId @0 : Text;
    voteType          @1 : VoteType;

    union {
        proposeToLeader   @2 : ProposeToLeader;
        proposeToClients  @3 : ProposeToClients;
        vote              @4 : Vote;
        announce          @5 : Announce;
    }
    tsyncCoordA       @6 : TimeSyncCoordA;

    struct ProposeToLeader {
        proposeId @0 : Text;
    }

    struct ProposeToClients {
        proposeId @0 : Text;
        leaderId  @1 : Text;
    }

    struct Vote {
        proposeId  @0 : Text;
        #leaderId   @1 : Text;
        voteResult @1 : VoteResults;
    }

    struct Announce {
        proposeId  @0 : Text;
        voteResult @1 : VoteResults;
    }

    struct TimeSyncCoordA {
        actionId @0 : Text;
        time     @1 : TimeSpec;

        struct TimeSpec {
            tvSec  @0 : Int32;
            tvNsec @1 : Int32;
        }
    }

    enum VoteResults { accepted @0; rejected @1; }
    enum VoteType {value @0; action @1; }
}

struct GroupMessage {
    sourceComponentId @0 : Text;
}

struct GroupInternals {
    union{
        groupHeartBeat    @0 : GroupHeartBeat;
        leaderElection    @1 : LeaderElection;
        messageToLeader   @2 : MessageToLeader;
        consensus         @3 : Consensus;
        groupMessage      @4 : GroupMessage;
    }
}
