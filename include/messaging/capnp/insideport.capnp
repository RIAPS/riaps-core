@0xbfb632a11118aa2b;

# Inside port message

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("riaps::ports");


struct InsideMessage {
  value @0 : Text;
}