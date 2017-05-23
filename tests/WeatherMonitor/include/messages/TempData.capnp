@0xdbda55a84ff8f35c;


using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("WeatherMonitor::messages");

struct TempData
{
    tempature @0: Float64;
}
