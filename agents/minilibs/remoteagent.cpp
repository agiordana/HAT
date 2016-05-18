/*
 *  remoteagent.cpp
 *  phoneagent
 *
 *  Created by Attilio Giordana on 11/20/11.
 *  Copyright 2011 Università del Piemonte Orientale. All rights reserved.
 *
 */

using namespace std;

#include "agentlib.h"

int RemoteAgent::init(string& n) {
	name = n; 
    post = "/"; 
    host = "127.0.0.1";
	useragent = "Mozilla/4.0";
	pragma = "no-cache";
	contenttype = " ";
	targetip = get("address");
	targetport = get("port");
	return 1;
}

int RemoteAgent::load() {
	string tmp;
	Params::load();
	if((tmp = get("post")) != "") post = tmp;
	if((tmp = get("useragent")) != "") useragent = tmp;
	if((tmp = get("pragma")) != "") pragma = tmp;
	if((tmp = get("contenttype")) != "") contenttype = tmp;
	if((tmp = get("address")) != "") targetip = tmp;
	if((tmp = get("port")) != "") targetport = tmp;
	return 1;
}

int RemoteAgent::send(Message m) {
	string message;
	
    if(name == "none") return 0;
    if(get("status") == "OFF") return 0;    
	
    messageComplete(m);
    
    if(get("language") == "xml") 
        message = m.xmlEncode(1);
	else 
        message = m.htmlPostEncode();
    
    return sendStringMessage(message);
}

int RemoteAgent::send(MMessage& m) {
	string message;
	
    if(name == "none") return 0;
    if(get("status") == "OFF") return 0;    
	
    messageComplete(m);
    
    if(get("language") == "xml") 
        message = m.xmlEncode(1);
	else 
        message = m.htmlPostEncode();
    
    return sendStringMessage(message);
}

int RemoteAgent::sendStringMessage(string message) {
    char buffer[MAXB];
	char buffin[MAXB];
	unsigned long n, buflen;
	int r;
	int timeout=0;
	int bl=0;
    string info;
    
	if(_DEBUG_) {
        info = "Message sent: "+message;
        hsrv::logger->info(info, __FILE__, __FUNCTION__, __LINE__);
    }
	
    sprintf(buffer, "POST %s HTTP/1.0\r\nHost: %s\r\nUser-Agent: %s\r\nPragma: %s\r\nContent-Length: %u\r\nContent-type: %s\r\n\r\n%s\r\n\r\n",
			post.c_str(), host.c_str(), useragent.c_str(), pragma.c_str(), (unsigned int)strlen(message.c_str()), contenttype.c_str(),
			message.c_str());
	buflen = strlen(buffer);
	r = connectSetup();
	
    if(r > 0 && sd >= 0) {
		n = write(sd, buffer, buflen);
        
		while((timeout = dataAvailable(sd)) > 0 && (n = read(sd, &buffin[bl], MAXB)) > 0) 
            if(n > 0) 
                bl += n;
		
        buffin[bl] = 0;
	}
	connectClose();
	if(timeout > 0) return 1;
	else {
		hsrv::logger->alert("tcp connection timed out (host: "+host+" - port: "+this->get("port")+")");
		return -1;
	}
    
}

bool RemoteAgent::messageComplete(Message& m) {
	if(get("language") !="") m.add("language", get("language"));
	m.add("agent", hsrv::agentname);
	if(m.mtype == "cmd" && m.getString("cmdname") == "cmdexec") {
		m.add("source", get("relay"));
		if(m.getString("value")=="") m.add("value", get("command"));
		m.add("duration", get("cmdduration"));
		m.remove("comment");
	}
	if(m.mtype == "cmd" && m.getString("cmdname") == "reset") {
		m.add("source", get("relay"));
        m.add("cmdname", "cmdexec");
		m.add("value", get("reset"));
		m.add("duration", get("resetduration"));
		m.remove("comment");
	}
	return true;
}

bool RemoteAgent::messageComplete(MMessage& m) {
	if(get("language") !="") m.add("language", get("language"));
	m.add("agent", hsrv::agentname);
	if(m.mtype == "cmd" && m.getString("cmdname") == "cmdexec") {
		m.add("source", get("relay"));
		if(m.getString("value")=="") m.add("value", get("command"));
		m.add("duration", get("cmdduration"));
		m.remove("comment");
	}
	if(m.mtype == "cmd" && m.getString("cmdname") == "reset") {
		m.add("source", get("relay"));
        m.add("cmdname", "cmdexec");
		m.add("value", get("reset"));
		m.add("duration", get("resetduration"));
		m.remove("comment");
	}
	return true;
}

bool RemoteAgent::isAlive() {
    char buffer[MAXB];
	char buffin[MAXB];
    unsigned long n, buflen;
    string answer;
	int r;
	int timeout=0;
	int bl=0;
    if(name == "none") return false;
    sprintf(buffer, "GET /AYA HTTP/1.0\r\nHost: %s\r\nUser-Agent: %s\r\nPragma: %s\r\n\r\n\r\n",
            host.c_str(), useragent.c_str(), pragma.c_str());
	buflen = strlen(buffer);
	r = connectSetup();
	
    if(r > 0 && sd >= 0) {
		n = write(sd, buffer, buflen);
        
		while((timeout = dataAvailable(sd)) > 0 && (n = read(sd, &buffin[bl], MAXB)) > 0) 
            if(n > 0) 
                bl += n;
		
        buffin[bl] = 0;
	}
	connectClose();
	if(timeout <= 0) {
        hsrv::logger->alert("tcp connection timed out (host: "+host+" - port: "+this->get("port")+")");
        return false;
    }
    answer = string(buffin);
    if(answer.find("IAA")!= string::npos) return true;
    return false;
}

int RemoteAgentSet::exec() {
	Message m = input.receive();
	int count = 0;
	string target = m.getString("target");
	for (unsigned i=0; i<size(); i++) {
		if((*this)[i].isTarget(target)) {
			(*this)[i].send(m);
			count++;
		}
    }
	return count;
}

RemoteAgentSet::RemoteAgentSet(string dir) {
	vector<string> fl;
	string conf = hsrv::configdir +"/"+dir;
	FileManager::fileList(conf, fl);
	for(unsigned i=0; i<fl.size(); i++) push_back(RemoteAgent(dir, FileManager::getStem(fl[i])));
	for(unsigned i=0; i<fl.size(); i++) (*this)[i].load();
}
												  
	
	
	
