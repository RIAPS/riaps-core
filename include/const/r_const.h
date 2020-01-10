#ifndef RIAPS_CORE_R_CONST_H
#define RIAPS_CORE_R_CONST_H

#include <string>

constexpr auto ENV_RIAPSAPPS = "RIAPSAPPS";

// ** RIAPS-CONF Path **
constexpr auto CONF_PATH = "/usr/local/riaps/etc/riaps.conf";

// ** DEVM endpoints **
constexpr auto DEVMANAGER_ENDPOINT = "tcp://127.0.0.1:9790";
constexpr auto DEPLO_ENDPOINT      = "tcp://127.0.0.1:9780";

// ** DISCOVERY PARAMETERS **
constexpr auto DISCO_LOGGER_NAME       = "discovery";
constexpr auto CMD_DISCO_JOIN          = "JOIN";
constexpr auto REGULAR_MAINTAIN_PERIOD = 3000; //msec
constexpr auto RIAPS_DHT_NODE_PORT     = 4222;

// ** DISCOVERY POINTS **
constexpr auto DHT_RESULT_CHANNEL = "ipc:///tmp/dhtrouterchannel";
constexpr auto CONTROL_SOCKET     = "ipc:///tmp/discoverycontrol";
constexpr auto DISCOVERY_ENDPOINT = "tcp://127.0.0.1:9700";

// ** Security **
// *** RSA keys ***
constexpr auto KEY_FOLDER = "/usr/local/riaps/keys";
constexpr auto KEY_FILE   = "id_rsa.key";

// *** CURVE Path ***
//constexpr auto CURVE_FOLDER = "/usr/local/riaps/keys";
//constexpr auto CURVE_KEY    = "riaps-sys.cert";
constexpr auto CURVE_KEY    = "riaps-app.cert";

//** Groups **
constexpr auto INTERNAL_SUB_NAME = "$SUB#";
constexpr auto INTERNAL_PUB_NAME = "$PUB#";
constexpr auto INTERNAL_ANS_NAME = "$ANS#";
constexpr auto INTERNAL_QRY_NAME = "$QRY#";
constexpr auto INTERNAL_ANS_REQ_TYPE = "group-mtl";
constexpr auto INTERNAL_ANS_REP_TYPE = "group-mfl";
constexpr auto INTERNAL_QRY_REQ_TYPE = "group-mtl";
constexpr auto INTERNAL_QRY_REP_TYPE = "group-mfl";

// Group coordination: default timing values (in msec)
constexpr uint16_t GROUP_HEARTBEAT        = 1000; // Group heartbeat period
constexpr uint16_t GROUP_ELECTION_MIN     = 1500; // Minimum leader election timeout
constexpr uint16_t GROUP_ELECTION_MAX     = 2000; // Maximum leader election timeout
constexpr uint16_t GROUP_PEERTIMEOUT      = 3000; // Peer is declared lost after this timeout
constexpr uint16_t GROUP_CONSENSUSTIMEOUT = 1500; // Deadline for consensus vote
constexpr uint16_t AUTHORITY_TIMEOUT      = 750;  // Leader heartbeat period

// Message frames
constexpr auto HEARTBEAT = "tic";
constexpr auto REQVOTE   = "req"; //{ term ; ownId }
constexpr auto RSPVOTE   = "vot"; //{ term; candId; bool; ownId }
constexpr auto AUTHORITY = "ldr"; //{ term; ldrId; ldrHost; ldrPort }

constexpr auto GROUP_MSG = "msg"; // Group message
constexpr auto GROUP_MTL = "mtl"; // Message to leader
constexpr auto GROUP_MFL = "mfl"; // Message from leader
constexpr auto GROUP_RFV = "rfv"; // Request for vote
constexpr auto GROUP_RCM = "rcm"; // Request consensus from member (by leader)
constexpr auto GROUP_RTC = "rtc"; // Reply to consensus request
constexpr auto GROUP_ANN = "ann"; // Announce

#endif //RIAPS_CORE_R_CONST_H
