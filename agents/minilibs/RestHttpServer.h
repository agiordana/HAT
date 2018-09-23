/* 
 * File:   RestHttpServer.cpp
 * Author: andrea
 * 
     * Created on 18 novembre 2014, 9.44
 */

#ifndef RESTHTTPSERVER_H
#define RESTHTTPSERVER_H

#include <httpserver.hpp>


using namespace httpserver;

class RestService : public http_resource {
public:
    
    RestService() {};

    ~RestService() {
    };

    bool xmlLoad(std::string xmlfile);
	
    bool start(std::string n = "");

    const http_response render_GET(const http_request &req);

    const http_response render_PUT(const http_request &req);

    const http_response render_POST(const http_request &req);

    const http_response render(const http_request &req);

    const http_response render_HEAD(const http_request &req);

    const http_response render_OPTIONS(const http_request &req);

    const http_response render_CONNECT(const http_request &req);

    const http_response render_DELETE(const http_request &req);

private:
    MMessage nonRest_POST(std::string);
    std::string name;
    uint16_t s_port;
};


#endif
