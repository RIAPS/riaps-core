#include "discoveryd/r_http_api.h"
#include "utils/r_message.h"


// TODO: Add tags in the registration
int do_put(std::string host, int port, std::string url, std::string put_body) {
    ne_session *sess;
    ne_request *req;
    std::string response;
 
    ne_sock_init();
 
    sess = ne_session_create("http", host.c_str(), port);
    ne_set_useragent(sess, "RiapsAgent/1.0");
 
    req = ne_request_create(sess, "PUT", url.c_str());
    
    //auto str_param = json_param.dump(1);

    ne_add_request_header(req, "Content-type", "text/xml");
    ne_set_request_body_buffer(req, put_body.c_str(), put_body.size());
 
    int result = ne_request_dispatch(req);
    int status = ne_get_status(req)->code;
 
    ne_request_destroy(req);
 
    std::string errorMessage = ne_get_error(sess);
    ne_session_destroy(sess);

    // just print result & status
    printf("result %d, status %d\n", result, status);
 
    return 0;
}

int httpResponseReader(void *userdata, const char *buf, size_t len)
{
    std::string *str = (std::string *)userdata;
    str->append(buf, len);
    return 0;
}

int do_get(std::string host, int port, std::string getparam, std::string& response)
{
    ne_session *sess;
    ne_request *req;

    ne_sock_init();

    sess = ne_session_create("http", host.c_str(), port);
    ne_set_useragent(sess, "RiapsAgent/1.0");

    //req = ne_request_create(sess, "GET", "/SomeURL/method?with=parameter&value=data");
    req = ne_request_create(sess, "GET", getparam.c_str());

    // if accepting only 2xx codes, use "ne_accept_2xx"
    ne_add_response_body_reader(req, ne_accept_always, httpResponseReader, &response);

    int result = ne_request_dispatch(req);
    int status = ne_get_status(req)->code;

    ne_request_destroy(req);

    std::string errorMessage = ne_get_error(sess);
    ne_session_destroy(sess);

    //printf("result %d, status %d\n", result, status);
    //std::cout << response << std::endl;

    //response. nlohmann::json::parse(response.c_str());

    //response.parse("alma");

    //for (auto it = json_result.begin(); it != json_result.end(); ++it) {
    //    std::cout << it.key() << '\n';
    //}

    /*switch (result) {
    case NE_OK:
        break;
    case NE_CONNECT:
        throw std::ConnectionError(errorMessage);
    case NE_TIMEOUT:
        throw TimeOutError(errorMessage);
    case NE_AUTH:
        throw AuthenticationError(errorMessage);
    default:
        throw AnotherWebError(errorMessage);
    }*/

    return result;
}

int do_delete(std::string host, int port, std::string getparam, std::string& response)
{
    ne_session *sess;
    ne_request *req;

    ne_sock_init();

    sess = ne_session_create("http", host.c_str(), port);
    ne_set_useragent(sess, "RiapsAgent/1.0");

    req = ne_request_create(sess, "DELETE", getparam.c_str());

    // if accepting only 2xx codes, use "ne_accept_2xx"
    ne_add_response_body_reader(req, ne_accept_always, httpResponseReader, &response);

    int result = ne_request_dispatch(req);
    int status = ne_get_status(req)->code;

    ne_request_destroy(req);

    std::string errorMessage = ne_get_error(sess);
    ne_session_destroy(sess);

    return result;
}