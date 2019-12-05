@0xf5d0efd4f6338d24;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("groupapp::messages");

# riaps:keep_msg:begin
struct Msg {
    value @0 : Text;

}
# riaps:keep_msg:end

