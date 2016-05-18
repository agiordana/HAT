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

class RestService : public http_resource<RestService> {
public:
    
    RestService() {};

    ~RestService() {
    };

    bool xmlLoad(std::string xmlfile);
	
    bool start(std::string n = "");

    void render_GET(const http_request &req, http_response** res);

    void render_PUT(const http_request &req, http_response** res);

    void render_POST(const http_request &req, http_response** res);

    void render(const http_request &req, http_response** res);

    void render_HEAD(const http_request &req, http_response** res);

    void render_OPTIONS(const http_request &req, http_response** res);

    void render_CONNECT(const http_request &req, http_response** res);

    void render_DELETE(const http_request &req, http_response** res);

private:
    MMessage nonRest_POST(std::string);
    std::string name;
    uint16_t s_port;
};


#endif
