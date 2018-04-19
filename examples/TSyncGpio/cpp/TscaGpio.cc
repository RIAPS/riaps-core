#include <TscaGpio.h>

namespace tsyncgpio {
    namespace components {

        TscaGpio::TscaGpio(_component_conf &config, riaps::Actor &actor)
                : TscaGpioBase(config, actor), m_hasJoined(false) {
            _logger->set_level(spd::level::info);
            _logger->set_pattern("[%H:%M:%S] %v");

            m_generator = std::mt19937(m_rd());
            m_distrPeriod = std::uniform_int_distribution<int>(2, 4);

            /**
             * Initialize the GPIO port
             */
            m_pps_output = libsoc_gpio_request(PPS_OUTPUT, LS_GPIO_SHARED);
            sleep(1);

            if (!m_pps_output) {
                _logger->error("unable to request gpio pin");
            }

            libsoc_gpio_set_direction(m_pps_output, OUTPUT);
            sleep(1);

            if (libsoc_gpio_get_direction(m_pps_output) != OUTPUT) {
                _logger->error("unable to set output direction");
            }
        }

        /**
         * Handler of the periodic timer, it is called in every 5th sec.
         * - If the component hasn't joined to the group then itt calls JoinToGroup()
         * - If the component has succesfully joined to the group, then proposes an action.
         */
        void TscaGpio::OnClock(riaps::ports::PortBase *port) {

            /**
             * If the component hasn't joined
             */
            if (!m_hasJoined) {
                /**
                 * Join to groupA (defined in the riaps file)
                 */
                bool rc = this->JoinToGroup({GROUP_TYPE_GROUPA, "ActionSync"});
                if (rc) {
                    _logger->info("Successfully joined to group");
                    m_hasJoined = true;
                } else {
                    _logger->error("Failed to join");
                }
            }

                /**
                 * The component has already joined, propose the action if the current component is not the leader
                 **/
            else {
                riaps::groups::GroupId gid{GROUP_TYPE_GROUPA, "ActionSync"};

                // If the component is not the leader, then proposeAction()
                if (GetLeaderId(gid) != "" && GetLeaderId(gid) != GetCompUuid()) {
                    /**
                     * Agreeing to run the given action in the 2nd second;
                     */
                    timespec now;
                    clock_gettime(CLOCK_REALTIME, &now);

                    // Random between 2 and 4
                    now.tv_sec += m_distrPeriod(m_generator);
                    now.tv_nsec = 0;

                    /**
                     * Propose the action with id "0" to the leader. If accepted, it will
                     * be executed "now" (t+2secs).
                     */
                    std::string proposeId = ProposeAction(gid, "0", now);
                }
            }
        }

        /**
         * The leader forwarded the propose to the clients
         * @param groupId The group where the voting process happens
         * @param proposeId UniqueId of the propose. (was set by proposeAction())
         * @param actionId  The action to be executed
         * @param timePoint The time when the action must be started.
         */
        void TscaGpio::OnActionPropose(riaps::groups::GroupId &groupId,
                                       const std::string &proposeId,
                                       const std::string &actionId,
                                       const timespec &timePoint) {
            /**
             * If the action has already been scheduled -> reject
             */
            if (m_pendingActions.find("0") != m_pendingActions.end()) {
                SendVote(groupId, proposeId, false);
                return;
            }

            /**
             * Check that if there is any reason to not schedule the action.
             * Now all the proposed actions will be accepted.
             */
            if (false) {
                // There is a reason to not schedule the action
                SendVote(groupId, proposeId, false);
                return;
            }

            /**
             * Accepted propose, save the exact time for later.
             */
            m_accepted[proposeId] = timePoint;

            /**
             * Send the vote, accept.
             */
            SendVote(groupId, proposeId, true);
        }

        /**
         * The leader announced the vote results of the proposed timesync coord.act.
         * @param groupId
         * @param proposeId
         * @param accepted
         */
        void TscaGpio::OnAnnounce(const riaps::groups::GroupId &groupId,
                                  const std::string &proposeId, bool accepted) {

            /**
             * If accepted and the propose is accepted previously on this node
             * And the same action is not scheduled now.
             */
            if (accepted && m_accepted.find(proposeId) != m_accepted.end() &&
                m_pendingActions.find("0") == m_pendingActions.end()) {

                // Get the time when the action must be performed
                timespec tp = m_accepted[proposeId];

                /**
                 * Schedule the action, but the timer thread will wake up 2000microsec
                 * earlyer than the scheduled time.
                 */
                auto timerId = ScheduleAction(
                        tp, std::bind(&TscaGpio::ActionA, this, std::placeholders::_1),
                        2000 * 1000);

                char buffer[80];
                std::strftime(buffer, 80, "%T", std::localtime(&tp.tv_sec));
                _logger->info("Action is scheduled for : {}.{}", buffer,
                              tp.tv_nsec / 1000);

                m_scheduled[timerId] = tp;
                m_pendingActions.insert("0");
                m_accepted.erase(proposeId);
            }
        }

        void TscaGpio::ActionA(const uint64_t timerId) {
            /**
             * Busy wait. The action wakes up earlier (last param of ScheduleAction())
             * Use the high-precision WaitUntil() to reach the right time to fire the
             * action.
             */
            WaitUntil(m_scheduled[timerId]);

            timespec tpBeforeHigh;
            clock_gettime(CLOCK_REALTIME, &tpBeforeHigh);

            _logger->error_if(libsoc_gpio_set_level(m_pps_output, HIGH) ==
                              EXIT_FAILURE,
                              "Couldn't set GPIO to HIGH");
            _logger->error_if(libsoc_gpio_set_level(m_pps_output, LOW) ==
                              EXIT_FAILURE,
                              "Couldn't set GPIO to LOW");


            auto diff = -1 * (((BILLION - tpBeforeHigh.tv_nsec) / 1000) - 50);
            _logger->info("Action is executed, diff: {}{} us", diff > 0 ? "+" : "",
                          diff);

            /**
             * The action is not scheduled anymore, remove the timerId.
             */
            m_scheduled.erase(timerId);

            /**
             * The action is not pending anymore.
             */
            m_pendingActions.erase("0");
        }





        void TscaGpio::OnGroupMessage(const riaps::groups::GroupId &groupId,
                                      capnp::FlatArrayMessageReader &capnpreader,
                                      riaps::ports::PortBase *port) {}

        TscaGpio::~TscaGpio() {
            _logger->error_if(libsoc_gpio_free(m_pps_output) == EXIT_FAILURE,
                              "Couldn'r release gpio");
        }
    } // namespace components
} // namespace tsyncgpio

riaps::ComponentBase *create_component(_component_conf &config,
                                       riaps::Actor &actor) {
    auto result = new tsyncgpio::components::TscaGpio(config, actor);
    return result;
}

void destroy_component(riaps::ComponentBase *comp) { delete comp; }
