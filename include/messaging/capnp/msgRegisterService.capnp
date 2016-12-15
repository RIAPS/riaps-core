@0xcbc60a5a7842a860;

struct ServiceParameters {
  serviceId    @0 :Text;
  serviceName  @1 :Text;
  ipAddress    @2 :Text;
  port         @3 :UInt16;
  tags         @4 :List(Text);
}