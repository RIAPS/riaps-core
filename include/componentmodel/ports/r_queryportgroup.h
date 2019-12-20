#ifndef RIAPS_FW_R_QUERYPORTGROUP_H
#define RIAPS_FW_R_QUERYPORTGROUP_H

#include <componentmodel/ports/r_queryport.h>

namespace riaps {

    class ComponentBase;

    namespace ports {
        class QueryPortGroup : public QueryPort {
        public:

            /**
             * @param config Configuration. Comes from the riaps file.
             * @param component The parent component.
             */
            QueryPortGroup(const ComponentPortQry &config, const ComponentBase *component);
            void Init() override;
        };
    }
}

#endif //RIAPS_FW_R_QUERYPORTGROUP_H
