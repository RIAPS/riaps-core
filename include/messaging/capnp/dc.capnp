@0xf99c15c18d87043c;

# RIAPS dc messages

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("riaps::groups::poll");

enum Vote { yes @0; no @1; timeout @2; }
enum Voting { majority @0; consensus @1; }
enum Subject { value @0; action @1; }

struct ReqForVote {
	topic 	@0: Data;
	rfvId 	@1: Text;
	kind 	@2: Voting;
	subject @3: Subject;
	release @4: Float64;
	started @5: Float64;
	timeout @6: Float64;
}

struct RepToVote {
	rfvId @0: Text;
	vote  @1: Vote;
}

struct AnnounceVote {
	rfvId @0: Text;
	vote  @1: Vote;
}

struct GroupVote {
  union {
	rfv @0: ReqForVote;
	rtc @1: RepToVote;
	ann @2: AnnounceVote;
  }
}
