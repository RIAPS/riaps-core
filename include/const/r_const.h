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
constexpr auto CURVE_FOLDER = "/usr/local/riaps/keys";
constexpr auto CURVE_KEY    = "riaps-sys.cert";

//** Groups **
constexpr auto INTERNAL_SUB_NAME = "$SUB#";
constexpr auto INTERNAL_PUB_NAME = "$PUB#";
constexpr auto INTERNAL_MESSAGETYPE = "InternalGroupMessage";

/**
 * Actor argument index
 */
constexpr auto ARG_IDX_APP   = 1;
constexpr auto ARG_IDX_MODEL = 2;
constexpr auto ARG_IDX_ACTOR = 3;
constexpr auto ARG_IDX_DEVICE = ARG_IDX_ACTOR;

#endif //RIAPS_CORE_R_CONST_H
