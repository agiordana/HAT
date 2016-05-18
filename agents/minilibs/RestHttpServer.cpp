/* 
 * File:   RestHttpServer.h
 * Author: andrea
 *
 * Created on 18 novembre 2014, 9.44
 */


/*
     This file is part of libhttpserver
     Copyright (C) 2014 Sebastiano Merlino

     This library is free software; you can redistribute it and/or
     modify it under the terms of the GNU Lesser General Public
     License as published by the Free Software Foundation; either
     version 2.1 of the License, or (at your option) any later version.

     This library is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
     Lesser General Public License for more details.

     You should have received a copy of the GNU Lesser General Public
     License along with this library; if not, write to the Free Software
     Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
     USA
 */


#include "agentlib.h"

bool verbose = false;

using namespace httpserver;



bool RestService::xmlLoad(string xmlfile) {
  bool res;
  TiXmlDocument* doc;
  TiXmlNode* prg;
  string xmldesc = hsrv::configdir+"/"+xmlfile;
  string attr;
  
  if(!hsrv::isfile(xmldesc, "xml")) {
    attr = AgentNet::getAgentPort();
    s_port = atoi(attr.c_str()); 
  }

  doc = new TiXmlDocument(xmldesc.c_str());
  res = doc->LoadFile();
	
  if(!res) return 0;

  prg = doc->FirstChild();
  prg = prg->NextSibling();
	
  if(prg == NULL) {
    s_port = 8082;
    delete doc;
    return 0;
  }
	

  attr = hsrv::getAttrbyName(prg->ToElement(),"port"); 
  if(attr == "*") { 
    s_port = 8082;
  } else {
    s_port = (uint16_t)atoi(attr.c_str());
  }

  delete doc;
  return 1;
}




void RestService::render_GET(const http_request &req, http_response** res) {
    int resp_code;
    MMessage mresp;
    string resp_body;
    string resp_type;
    string uri;

    if (req.get_path() == "" or req.get_path() == "/") {
      uri = "/doc/index.html";      
    } else {
      uri = req.get_path(); 
    }
    
    mresp = hsrv::router->route(uri, "GET");
    resp_body = mresp.getString("body");

    if (resp_body == "Not Found") {
        resp_code = 404;
    } else if (resp_body == "Forbidden") {
        resp_code = 403;
    } else {
        resp_code = 200;
    }
    
    resp_type = mresp.getString("content");

    *res = new http_response(http_response_builder(resp_body, resp_code, resp_type).string_response());
//    *res = new http_response(http_response_builder(resp_body, resp_code).string_response());
}


void RestService::render_PUT(const http_request &req, http_response** res) {
    int resp_code;
    MMessage mresp;
    string resp_body;
    string resp_type;
    string body;
    string uri;
    
    uri = req.get_path();
    body = req.get_content();
    mresp = hsrv::router->route(uri, "PUT", body);
    	
    resp_type = mresp.getString("content");
    if (resp_type == "application/json") {
      resp_body = mresp.getString("body");
    } else {
      resp_body = mresp.xmlEncode();
    }
    mresp.erase("content");

    if (resp_body == "Not Found") {
        resp_code = 404;
    } else if (resp_body == "Forbidden") {
        resp_code = 403;
    } else {
        resp_code = 200;
    }
    
    *res = new http_response(http_response_builder(resp_body, resp_code, resp_type).string_response());
}


void RestService::render_POST(const http_request &req, http_response** res) {
    int resp_code;
    MMessage mresp;
    string resp_body;
    string resp_type;
    string body;
    string uri;
    uri = req.get_path();
    body = req.get_content();    
    if(uri == "" || uri == "/") {
	mresp = nonRest_POST(body);
	//resp_body = mresp.getString("body");

	resp_type = mresp.getString("content");
	mresp.erase("content");	
	resp_body = mresp.xmlEncode();
    }
    else {
	mresp = hsrv::router->route(uri, "POST", body);
	
	resp_type = mresp.getString("content");
	if (resp_type == "application/json") {
 	  resp_body = mresp.getString("body");
	} else {
          resp_body = mresp.xmlEncode();
	}
        mresp.erase("content");			
    }

    if (resp_body == "Not Found") {
        resp_code = 404;
    } else if (resp_body == "Forbidden") {
        resp_code = 403;
    } else {
        resp_code = 200;
    }
    
    *res = new http_response(http_response_builder(resp_body, resp_code, resp_type).string_response());
}




void RestService::render(const http_request &req, http_response** res) {

    if (verbose) std::cout << req;

    *res = new http_response(http_response_builder("generic response", 200).string_response());

    if (verbose) std::cout << **res;
}

void RestService::render_HEAD(const http_request &req, http_response** res) {

    if (verbose) std::cout << req;

    *res = new http_response(http_response_builder("HEAD response", 200).string_response());

    if (verbose) std::cout << **res;
}

void RestService::render_OPTIONS(const http_request &req, http_response** res) {

    if (verbose) std::cout << req;

    *res = new http_response(http_response_builder("OPTIONS response", 200).string_response());

    if (verbose) std::cout << **res;
}

void RestService::render_CONNECT(const http_request &req, http_response** res) {

    if (verbose) std::cout << req;

    *res = new http_response(http_response_builder("CONNECT response", 200).string_response());

    if (verbose) std::cout << **res;
}

void RestService::render_DELETE(const http_request &req, http_response** res) {
 int resp_code;
    MMessage mresp;
    string resp_body;
    string resp_type;
    string uri;

    if (req.get_path() != "") {
      uri = req.get_path(); 
    } else {
      uri = ".";
    }
    
    mresp = hsrv::router->route(uri, "DELETE");
    resp_body = mresp.getString("body");

    if (resp_body == "Not Found") {
        resp_code = 404;
    } else if (resp_body == "Forbidden") {
        resp_code = 403;
    } else {
        resp_code = 200;
    }
    
    *res = new http_response(http_response_builder(resp_body, resp_code).string_response());
}


bool RestService::start(std::string n) {
    xmlLoad("HttpServer.xml");
    name = n;
    create_webserver cw = create_webserver(s_port).max_threads(5);
    //
    // Create webserver using the configured options
    //
    webserver ws = cw;

    //
    // Create and register service resource available at /service
    //
    ws.register_resource(name, this, true);


    //
    // Start and block the webserver
    //
    return ws.start(true);
}

MMessage RestService::nonRest_POST(std::string body) {
    MMessage msg("dummy");
    MMessage answer("answer");
    msg.add("method","POST");
    msg.add("url", "/");
    if(hsrv::isXmlstring(body)) {
	body+="\r\n";
	msg.add("language", "xml");
	answer.add("language", "xml");
	msg.xmlAppend(body);
    }
    else {
	msg.add("language", "html");
	answer.add("language", "html");
	msg.htmlAppend(body);
    }
    string dummy = "*";
    string ty = msg.mtype;
    string subty = msg.msubtype;
    if (SubjectSet::check_subject(ty, subty)||SubjectSet::check_subject(ty,dummy)) {
	SubjectSet::notify(msg);
	answer.add("result","done");
	return answer;
    }
    else answer.add("result", "Not Found");
    return answer;
}
