@0xa9b61477210cf813;

# RIAPS devm messages

enum Status { ok @0; err @1; }

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

struct DeviceArg {
  name @0 : Text;
  value @1 : Text;
}

struct DeviceRegReq {
  appName @0 : Text;
  modelName @1 : Text;
  typeName @2 : Text;
  deviceArgs @3 : List(DeviceArg); 
}

struct DeviceRegRep {
  status @0 : Status;
}

struct DeviceUnregReq {
  appName @0 : Text;
  modelName @1 : Text;
  typeName @2 : Text;
}

struct DeviceUnregRep {
  status @0 : Status;
}

struct DevmReq {
   union {
      actorReg @0 : ActorRegReq;
      deviceReg @1 : DeviceRegReq;
      deviceUnreg @2: DeviceUnregReq;
   }
}

struct DevmRep {
   union {
      actorReg @0 : ActorRegRep;
      deviceReg @1 : DeviceRegRep;
      deviceUnreg @2 : DeviceUnregRep;
   }
}

