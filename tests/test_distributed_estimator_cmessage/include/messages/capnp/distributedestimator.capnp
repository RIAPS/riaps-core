@0xbfb633a11018ca1c;

struct SensorReady {
  msg @0 : Text;
 }

struct SensorValue {
   msg @0 : Text;
 }

 struct SensorQuery {
    msg @0 : Text;
 }

 struct Estimate {
    msg @0     : Text;
    strings @1 : List(Float64);
 }