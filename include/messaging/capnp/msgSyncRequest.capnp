@0x8cc70ef4c672446a;

struct SyncRequest {
    union {
        registerActor :group{
            actorName @0 : Text;
        }
        registerNode :group{
            useLater @1 : Text;
        }
    }
}