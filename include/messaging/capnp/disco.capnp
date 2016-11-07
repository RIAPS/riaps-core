@0xbfb632a00018ca1b;

# RIAPS disco messages

enum Status { ok @0; err @1; }
enum Kind { none @0; pub @1; sub @2; clt @3; srv @4; req @5; rep @6; }
enum Scope { none @0; global @1; local @2; }

struct ActorRegReq {
  appName @0 : Text;
  version @1 : Text;
  actorName @2 : Text;
}

struct ActorRegRep {
  status @0 : Status;
  port @1 : Int32;
}

struct Path {
  appName @0 : Text;
  msgType @1 : Text;
  kind @2 : Kind;
  scope @3 : Scope;
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

struct DiscoUpd {
  client @0 : Client;
  scope @1 : Scope;
  socket @2 : Socket;
}

struct DiscoReq {
   union {
      actorReg @0 : ActorRegReq;
      serviceReg @1 : ServiceRegReq;
      serviceLookup @2 : ServiceLookupReq;
   }
}

struct DiscoRep {
   union {
      actorReg @0 : ActorRegRep;
      serviceReg @1 : ServiceRegRep;
      serviceLookup @2 : ServiceLookupRep;
   }
}



