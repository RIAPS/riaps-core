#include <TempMonitor.h>

namespace weathermonitor {
    namespace components {		

        TempMonitor::TempMonitor(_component_conf_j &config, riaps::Actor &actor) :
                TempMonitorBase(config, actor) {
        }

		void TempMonitor::OnTempupdate(const messages::TempData::Reader &message, riaps::ports::PortBase *port) {
		    std::cout << "TempMonitor::OnTempupdate(): " << std::to_string(message.getTempature()) << std::endl;
		}
		
		void TempMonitor::OnOneShotTimer(const std::string& timerid){
		
		}

        TempMonitor::~TempMonitor() {

        }
    }
}

riaps::ComponentBase *create_component(_component_conf_j &config, riaps::Actor &actor) {
    auto result = new weathermonitor::components::TempMonitor(config, actor);
    return result;
}

void destroy_component(riaps::ComponentBase *comp) {
    delete comp;
}
