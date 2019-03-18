@0xc70bd0cc936c6fac;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("distributedvoltage::messages");

# riaps:keep_voltage:begin
struct Timespec {
     tvSpec @0  : Int64;
     tvNspec @1 : Int64;
}

struct Voltage {
    values @0 : List(Int64);
    time   @1 : Timespec;
}
# riaps:keep_voltage:end

