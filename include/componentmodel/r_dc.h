//
// Created by istvan on 11/25/19.
//

#ifndef RIAPS_CORE_R_DC_H
#define RIAPS_CORE_R_DC_H

#include <string>

// Group coordination: default timing values (in msec)
constexpr uint16_t GROUP_HEARTBEAT        = 1000; // Group heartbeat period
constexpr uint16_t GROUP_ELECTION_MIN     = 1500; // Minimum leader election timeout
constexpr uint16_t GROUP_ELECTION_MAX     = 2000; // Maximum leader election timeout
constexpr uint16_t GROUP_PEERTIMEOUT      = 3000; // Peer is declared lost after this timeout
constexpr uint16_t GROUP_CONSENSUSTIMEOUT = 1500; // Deadline for consensus vote

class Dc {
public:
    Dc() = default;
    void Init();
    ~Dc() = default;
private:

};

#endif //RIAPS_CORE_R_DC_H
