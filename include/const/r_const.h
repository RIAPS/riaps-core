#ifndef RIAPS_CORE_R_CONST_H
#define RIAPS_CORE_R_CONST_H

#include <string>

constexpr auto ENV_RIAPSAPPS = "RIAPSAPPS";

// ** Config **
constexpr auto CONF_PATH = "/etc/riaps/riaps.conf";

// Name of endpoint for actor-devm communication
constexpr auto DEVMANAGER_ENDPOINT = "tcp://127.0.0.1:9790";
constexpr auto DEPLO_ENDPOINT = "tcp://127.0.0.1:9780";

// ** DISCO **
constexpr auto DISCO_LOGGER_NAME = "discovery";
constexpr auto CMD_DISCO_JOIN = "JOIN";
constexpr auto REGULAR_MAINTAIN_PERIOD = 3000; //msec
constexpr auto RIAPS_DHT_NODE_PORT     = 4222;
constexpr auto DHT_RESULT_CHANNEL      = "ipc:///tmp/dhtrouterchannel";

// IPC socket address for sending control messages to the discovery service
constexpr auto CONTROL_SOCKET = "ipc:///tmp/discoverycontrol";
constexpr auto DISCOVERY_ENDPOINT = "tcp://127.0.0.1:9700";

// ** RSA keys ** //
// For discovery service (beacon signature, value encryption in DHT)
constexpr auto KEY_FOLDER = ".ssh";
constexpr auto KEY_FILE   = "id_rsa.key";

/**
 * Actor argument index
 */
constexpr auto ARG_IDX_APP   = 1;
constexpr auto ARG_IDX_MODEL = 2;
constexpr auto ARG_IDX_ACTOR = 3;
constexpr auto ARG_IDX_DEVICE = ARG_IDX_ACTOR;

#endif //RIAPS_CORE_R_CONST_H
