#ifndef R_HTTP_API
#define R_HTTP_API

#include <string>
#include <iostream>
#include <neon/ne_session.h>
#include <neon/ne_request.h>
#include <neon/ne_utils.h>
#include <neon/ne_uri.h>
#include "json.h"
#include "r_riaps_actor.h"
#include "utils/r_message.h"

int do_put(std::string host, int port, std::string url, std::string put_body);

int httpResponseReader(void *userdata, const char *buf, size_t len);

int do_get(std::string host, int port, std::string getparam, std::string& response);

int do_delete(std::string host, int port, std::string getparam, std::string& response);

#endif