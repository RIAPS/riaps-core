@0x86bbe73a97ada7d3;

# RIAPS disco messages

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("riaps::discovery");

# RIAPS disco messages

enum Status { ok @0; err @1; }
enum Kind { none @0; pub @1; sub @2; clt @3; srv @4; req @5; rep @6; qry @7; ans @8; }
enum Scope { none @0; global @1; local @2; }

struct ActorRegReq {
  appName @0 : Text;
  version @1 : Text;
  actorName @2 : Text;
  pid @3 : Int32;
}

struct ActorRegRep {
  status @0 : Status;
  port @1 : Int32;
}

struct ActorUnregReq {
  appName @0 : Text;
  version @1 : Text;
  actorName @2 : Text;
  pid @3 : Int32;
}

struct ActorUnregRep {
  status @0 : Status;
  port @1 : Int32;
}

struct Path {
  appName @0 : Text;
  actorName @1 : Text;
  msgType @2 : Text;
  kind @3 : Kind;
  scope @4 : Scope;
}

struct Client {
  actorHost @0 : Text;
  actorName @1 : Text;
  instanceName @2 : Text;
  portName @3 : Text;
}


struct Socket {
  host @0 : Text;
  port @1 : Int32;
 }

struct ServiceRegReq  {
  path @0 : Path;
  socket @1 : Socket;
  pid @2 : Int32;
}

struct ServiceRegRep {
  status @0 : Status;
}

struct ServiceLookupReq {
  path @0 : Path;
  client @1 : Client;
}

struct ServiceLookupRep {
  status @0  : Status;
  sockets @1 : List(Socket);
}

struct PortUpd {
  client @0 : Client;
  scope @1 : Scope;
  socket @2 : Socket;
}

struct DiscoUpd {
    union {
        portUpdate  @0 : PortUpd;
        groupUpdate @1 : GroupUpdate;
    }
}



struct DiscoReq {
   union {
      actorReg      @0 : ActorRegReq;
      serviceReg    @1 : ServiceRegReq;
      serviceLookup @2 : ServiceLookupReq;
      actorUnreg    @3 : ActorUnregReq;
      groupJoin     @4 : GroupJoinReq;
   }
}

struct DiscoRep {
   union {
      actorReg      @0 : ActorRegRep;
      serviceReg    @1 : ServiceRegRep;
      serviceLookup @2 : ServiceLookupRep;
      actorUnreg    @3 : ActorUnregRep;
      groupJoin     @4 : GroupJoinRep;
   }
}

# Groups
struct GroupId {
    groupType   @0 : Text;
    groupName   @1 : Text;
}

struct GroupService {
    messageType @0 : Text;
    address     @1 : Text; # <IPaddress:port>
}

struct GroupJoinReq {
    appName     @0 : Text;
    groupId     @1 : GroupId;
    services    @2 : List(GroupService);
    componentId @3 : Text;
}

struct GroupJoinRep {
    status @0  : Status;
}

# Messages between OpenDHT - rdiscoveryd threads

struct ProviderListUpdate {
        providerpath @0 : Text;
        newvalues    @1 : List(Text);
}

struct ProviderListGet {
    path         @0 : Path;
    client       @1 : Client;
    results      @2 : List(Text);
}

struct GroupUpdate {
    appName           @0 : Text;
    groupId           @1 : GroupId;
    componentId       @2 : Text;
    services          @3 : List(GroupService);
}

struct DhtUpdate {
    union {
        providerUpdate @0: ProviderListUpdate;
        providerGet    @1: ProviderListGet;
        zombieList     @2: List(Text);
        groupUpdate    @3: GroupUpdate;
    }
}

