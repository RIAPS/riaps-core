/*
 *  Copyright (C) 2014-2016 Savoir-faire Linux Inc.
 *
 *  Author: Adrien Béraud <adrien.beraud@savoirfairelinux.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

// Common utility methods used by C++ OpenDHT tools.

#include <opendht.h>
#ifndef WIN32_NATIVE
#include <getopt.h>
#include <readline/readline.h>
#include <readline/history.h>
#else
#define SIGHUP 0
#include "wingetopt.h"
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>

#include <string>
#include <vector>
#include <chrono>
#include <iostream>
#include <sstream>
#include <fstream>

/**
 * Split "[host]:port" or "host:port" to pair<"host", "port">.
 */
std::pair<std::string, std::string>
splitPort(const std::string& s) {
    if (s.empty())
        return {};
    if (s[0] == '[') {
        std::size_t closure = s.find_first_of(']');
        std::size_t found = s.find_last_of(':');
        if (closure == std::string::npos)
            return {s, ""};
        if (found == std::string::npos or found < closure)
            return {s.substr(1,closure-1), ""};
        return {s.substr(1,closure-1), s.substr(found+1)};
    }
    std::size_t found = s.find_last_of(':');
    std::size_t first = s.find_first_of(':');
    if (found == std::string::npos or found != first)
        return {s, ""};
    return {s.substr(0,found), s.substr(found+1)};
}

/*
 * The mapString shall have the following format:
 *
 *      k1:v1[,k2:v2[,...]]
 */
std::map<std::string, std::string> parseStringMap(std::string mapString) {
    std::istringstream keySs(mapString);
    std::string mapStr;
    std::map<std::string, std::string> map;

    while (std::getline(keySs, mapStr, ',')) {
        std::istringstream mapSs(mapStr);
        std::string key, value;

        while (std::getline(mapSs, key, ':')) {
            std::getline(mapSs, value, ':');
            map[key] = { value };
        }
    }
    return map;
}

dht::indexation::Pht::Key createPhtKey(std::map<std::string, std::string> pht_key_str_map) {
    dht::indexation::Pht::Key pht_key;
    for (auto f : pht_key_str_map) {
        dht::Blob prefix {f.second.begin(), f.second.end()};
        pht_key.emplace(f.first, std::move(prefix));
    }
    return pht_key;
}

bool isInfoHash(dht::InfoHash& h) {
    static constexpr dht::InfoHash INVALID_ID {};

    if (h == INVALID_ID) {
        std::cout << "Syntax error: invalid InfoHash." << std::endl;
        return false;
    }
    return true;
}

static const constexpr in_port_t DHT_DEFAULT_PORT = 4222;

struct dht_params {
    bool help {false}; // print help and exit
    bool log {false};
    std::string logfile {};
    in_port_t port {0};
    dht::NetId network {0};
    bool generate_identity {false};
    bool daemonize {false};
    std::pair<std::string, std::string> bootstrap {};
};

static const constexpr struct option long_options[] = {
   {"help",       no_argument      , nullptr, 'h'},
   {"port",       required_argument, nullptr, 'p'},
   {"net",        required_argument, nullptr, 'n'},
   {"bootstrap",  required_argument, nullptr, 'b'},
   {"identity",   no_argument      , nullptr, 'i'},
   {"verbose",    no_argument      , nullptr, 'v'},
   {"daemonize",  no_argument      , nullptr, 'd'},
   {"logfile",    required_argument, nullptr, 'l'},
   {nullptr,      0                , nullptr,  0}
};

dht_params
parseArgs(int argc, char **argv) {
    dht_params params;
    int opt;
    while ((opt = getopt_long(argc, argv, "hidvp:n:b:l:", long_options, nullptr)) != -1) {
        switch (opt) {
        case 'p': {
                int port_arg = atoi(optarg);
                if (port_arg >= 0 && port_arg < 0x10000)
                    params.port = port_arg;
                else
                    std::cout << "Invalid port: " << port_arg << std::endl;
            }
            break;
        case 'n':
            params.network = strtoul(optarg, nullptr, 0);
            break;
        case 'b':
            params.bootstrap = splitPort((optarg[0] == '=') ? optarg+1 : optarg);
            if (not params.bootstrap.first.empty() and params.bootstrap.second.empty()) {
                std::stringstream ss;
                ss << DHT_DEFAULT_PORT;
                params.bootstrap.second = ss.str();
            }
            break;
        case 'h':
            params.help = true;
            break;
        case 'l':
            params.logfile = optarg;
            break;
        case 'v':
            params.log = true;
            break;
        case 'i':
            params.generate_identity = true;
            break;
        case 'd':
            params.daemonize = true;
            break;
        default:
            break;
        }
    }
    return params;
}

static const constexpr char* PROMPT = ">> ";

std::string
readLine(const char* prefix = PROMPT)
{
#ifndef WIN32_NATIVE
    const char* line_read = readline(prefix);
    if (line_read && *line_read)
        add_history(line_read);

#else
    char line_read[512];
    std::cout << PROMPT;
    fgets(line_read, 512 , stdin);
#endif
    return line_read ? std::string(line_read) : std::string("\0", 1);
}

void signal_handler(int sig)
{
    switch(sig) {
    case SIGHUP:
        break;
    case SIGTERM:
        exit(EXIT_SUCCESS);
        break;
    }
}

void daemonize()
{
#ifndef WIN32_NATIVE
    pid_t pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);

    umask(0);

    pid_t sid = setsid();
    if (sid < 0) {
        exit(EXIT_FAILURE);
    }

    if ((chdir("/")) < 0) {
        exit(EXIT_FAILURE);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    signal(SIGCHLD,SIG_IGN); /* ignore child */
    signal(SIGTSTP,SIG_IGN); /* ignore tty signals */
    signal(SIGTTOU,SIG_IGN);
    signal(SIGTTIN,SIG_IGN);
    signal(SIGHUP,signal_handler); /* catch hangup signal */
    signal(SIGTERM,signal_handler); /* catch kill signal */
#endif
}
