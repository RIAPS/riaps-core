#include <componentmodel/ports/r_answerportgroup.h>

using namespace std;

namespace riaps{
    namespace ports{

        AnswerPortGroup::AnswerPortGroup(const ComponentPortAns &config, const ComponentBase *parent_component) :
                AnswerPort(config, parent_component)
        {

        }

        void AnswerPortGroup::Init() {
            string end_point = fmt::format("tcp://{}:!", host_);
            port_ = zsock_bind(port_socket_, "%s", end_point.c_str());

            if (port_ == -1) {
                logger()->error("Couldn't bind response port.");
            }

            logger()->info("Answerport is created on: {}:{}", host_, port_);
        }
    }
}

