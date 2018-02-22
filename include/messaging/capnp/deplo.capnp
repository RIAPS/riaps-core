@0xb487bf324bf367d9;

# RIAPS depl messages

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("riaps::deplo");

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

struct DeplReq {
   union {
      actorReg @0 : ActorRegReq;
      deviceReg @1 : DeviceRegReq;
      deviceUnreg @2: DeviceUnregReq;
   }
}

struct DeplRep {
   union {
      actorReg @0 : ActorRegRep;
      deviceReg @1 : DeviceRegRep;
      deviceUnreg @2 : DeviceUnregRep;
   }
}

struct ResCPUX {
	msg @0 : Text;
}

struct ResMemX {
	msg @0 : Text;
}

struct ResSpcX {
	msg @0 : Text;
}

struct ResNetX {
	msg @0 : Text;
}

struct ResMsg {
	union {
		resCPUX @0 : ResCPUX;
		resMemX @1 : ResMemX;
		resSpcX @2 : ResSpcX;
		resNetX @3 : ResNetX;
	}
}
