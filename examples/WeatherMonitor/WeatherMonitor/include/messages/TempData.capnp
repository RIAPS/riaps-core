@0x83a94aca849cde76;


using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("weathermonitor::messages");

struct TempData
{
    tempature @0: Float64;
}
