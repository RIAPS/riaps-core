@0xcca08999da12b820;

# RIAPS log messages


enum LogMessageType { error @0; warning @1; notice @2; info @3; debug @4; }

struct NewLogPublisher {
    endpoint @0 : Text;
    #socket @0 :import "disco.capnp".Socket;
}

struct LogMessage {
    logMessageType  @0 : LogMessageType;
    message         @1 : Text;
}

struct Log {
    union {
          newLogPublisher @0 : NewLogPublisher;
          logMessage      @1 : LogMessage;
    }
}