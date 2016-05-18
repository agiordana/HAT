//
//  httpserver.cpp
//  phoneagent
//
//  Created by Davide Monfrecola on 11/15/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

using namespace std;

#include "agentlib.h"

//------------------------- Class HttpServer -------------------------------

int HttpServer::load() {
	xmlLoad("HttpServer.xml");
	rvport.init("127.0.0.1:"+port);
	fd = 0;
	return 1;
}

int HttpServer::load(string &path) {
    xmlLoad(path, "HttpServer.xml");
	rvport.init("127.0.0.1:" + port);
	fd = 0;
	return 1;
}

int HttpServer::xmlLoad(string xmlfile) {
	bool res;
	TiXmlDocument* doc;
	TiXmlNode* prg;
	string xmldesc = hsrv::configdir+"/"+xmlfile;
	
    if(!hsrv::isfile(xmldesc, "xml")) {
		port = AgentNet::getAgentPort();
	}
    
	doc = new TiXmlDocument(xmldesc.c_str());
	res = doc->LoadFile();
	
    if(!res) return 0;
	
    prg = doc->FirstChild();
	prg = prg->NextSibling();
	
    if(prg == NULL) {
		port = "8082";
		delete doc;
		return 0;
	}
	
    port = hsrv::getAttrbyName(prg->ToElement(),"port");
	
    if(port == "*") port = "8082";
	
    delete doc;
	return 1;
}

int HttpServer::xmlLoad(string path, string xml_file) {
    bool res;
	TiXmlDocument* doc;
	TiXmlNode* prg;
	string xmldesc = path + "/" + xml_file;
	
    if(!hsrv::isfile(xmldesc, "xml")) {
		port = "8082";
		return 0;
	}
    
	doc = new TiXmlDocument(xmldesc.c_str());
	res = doc->LoadFile();
	
    if(!res) return 0;
	
    prg = doc->FirstChild();
	prg = prg->NextSibling();
	
    if(prg == NULL) {
		port = "8082";
		delete doc;
		return 0;
	}
	
    port = hsrv::getAttrbyName(prg->ToElement(),"port");
	
    if(port == "*") port = "8082";
	
    delete doc;
	return 1;

}

int HttpServer::xmlSave() {
	ofstream out;
	string r;
	string path = hsrv::configdir+"/HttpServer.xml";

	out.open(path.c_str());
	r = xmlEncode(1);
	out << r << endl;
	out.close();

	return 1;
}

int HttpServer::getBodyLength(string& hd) {
	string val="";
	size_t i;
	if((i=hd.find("Content-Length:"))==string::npos) return 0;
	i+=string("Content-Length:").size();
	while((unsigned)i<hd.length() && hd[(unsigned)i]!='\r') {
		if(hd[(unsigned)i]!=' ') val+=hd[(unsigned)i];
		i++;
	}
	return atoi(val.c_str());
}

Message* HttpServer::getRequest() {
	char buffer[MAXB];
	string mtd="";
	string url="";
	string message="";
	string body;
	string hd;
	size_t count=1;
	size_t i, k;
	unsigned j;
	socklen_t client_len = sizeof(struct sockaddr);
	Message* tmp;
    
	if((fd = accept(rvport.sd, (struct sockaddr *)&client, &client_len)) <0) {
		if(errno != EINTR) hsrv::logger->error("accept failed",__FILE__, __FUNCTION__, __LINE__);
        return NULL;
	}
	while((i = message.find("\r\n\r\n")) !=string::npos && count > 0) {
		for(j = 0; j < MAXB; j++) 
            buffer[j]='\0';
		count = read(fd, buffer, MAXB-1);
		if(_DEBUG_) hsrv::logger->debug(string(buffer), __FILE__, __FUNCTION__, __LINE__);
		message = message+buffer;
	}
	hd = message.substr(0,(unsigned)i);
	i+=4;
	k = getBodyLength(hd);
	while(message.length()<(unsigned)(i+k) && count>0) {
		for(j = 0; j < MAXB; j++) 
            buffer[j]='\0';
		count = read(fd, buffer, MAXB-1);
		message = message+buffer;
	}
	if(message.length() >= (unsigned)(i+k))
        body = message.substr((unsigned)i, (unsigned)k);
	
	k = hd.find(' ',1);
	mtd = hd.substr(0,k);
	i = hd.find(' ', 1+k);
	url = hd.substr(k+1,i-k-1);
	tmp = new Message("httprequest");
	tmp->add("method",mtd);
	tmp->add("url", url);
    //	tmp->add("message",body);
    
	if(hsrv::isXmlstring(body)) {
		body+="\r\n";
		tmp->add("language", "xml");
		tmp->xmlAppend(body);
	}
	else {
		tmp->add("language", "html");
		tmp->htmlAppend(body);
	}
	return tmp;
}

MMessage* HttpServer::getRequestM() {
	char buffer[MAXB];
	string mtd="";
	string url="";
	string message="";
	string body;
	string hd;
	size_t count=1;
	size_t i, k;
	unsigned j;
	socklen_t client_len = sizeof(struct sockaddr);
	MMessage* tmp;
    
	if((fd = accept(rvport.sd, (struct sockaddr *)&client, &client_len)) <0) {
		if(errno != EINTR) hsrv::logger->error("accept failed",__FILE__, __FUNCTION__, __LINE__);
        return NULL;
	}
	while((i = message.find("\r\n\r\n")) == string::npos && count > 0) {
		for(j = 0; j < MAXB; j++) 
            buffer[j]='\0';
		count = read(fd, buffer, MAXB);
		if(_DEBUG_ == 1) hsrv::logger->info(string(buffer), __FILE__, __FUNCTION__, __LINE__);
		message = message+buffer;
	}
	hd = message.substr(0,(unsigned)i);
	i+=4;
	k = getBodyLength(hd);
	while(message.length()<(unsigned)(i+k) && count>0) {
		for(j = 0; j < MAXB; j++) 
            buffer[j]='\0';
		count = read(fd, buffer, MAXB);
		message = message+buffer;
	}
	if(message.length() >= (unsigned)(i+k))
        body = message.substr((unsigned)i, (unsigned)k);
	
	k = hd.find(' ',1);
	mtd = hd.substr(0,k);
	i = hd.find(' ', 1+k);
	url = hd.substr(k+1,i-k-1);
	tmp = new MMessage("httprequest");
	tmp->add("method",mtd);
	tmp->add("url", url);
    //	tmp->add("message",body);
    
	if(hsrv::isXmlstring(body)) {
		body+="\r\n";
		tmp->add("language", "xml");
		tmp->xmlAppend(body);
	}
	else {
		tmp->add("language", "html");
		tmp->htmlAppend(body);
	}
    
	return tmp;
}

int HttpServer::sendAnswer(Message* answer) {
	char* buffer;
	string body;
	string language = answer->getString("language");
    size_t r;
    
	if(language == "xml" || language == "html") 
        	answer->erase("language");
    
	body = (language == "xml"? answer->xmlEncode(1):answer->htmlEncode());
	
	buffer = (char*) malloc(body.size()+HDLEN);
	sprintf(buffer, "HTTP/1.1 200 OK\r\nConnection: close\r\nContent-type: text/html\r\n\r\n");
	sprintf(&buffer[strlen(buffer)], "%s\r\n", body.c_str());
	r = write(fd, buffer, strlen(buffer));
	close(fd);
	free(buffer);
	
    return (int)r;
}

int HttpServer::sendAnswerM(MMessage* answer) {
	char* buffer;
	string body;
	string language = answer->getString("language");
    size_t r;
    
	if(language == "xml" || language == "html") 
        	answer->remove("language");
    
	body = (language == "xml"? answer->xmlEncode(1):answer->htmlEncode());
	
	buffer = (char*) malloc(body.size()+HDLEN);
	sprintf(buffer, "HTTP/1.1 200 OK\r\nConnection: close\r\nContent-type: text/html\r\n\r\n");
	sprintf(&buffer[strlen(buffer)], "%s\r\n", body.c_str());
	r = write(fd, buffer, strlen(buffer));
	close(fd);
	free(buffer);
	
    return (int)r;
}
