#include <componentmodel/ports/r_queryportgroup.h>

using namespace std;
namespace riaps {
    namespace ports {

        QueryPortGroup::QueryPortGroup(const ComponentPortQry &config, const ComponentBase *component)
                :  QueryPort(config, component) {

        }

        void QueryPortGroup::Init() {

        }
    }
}
