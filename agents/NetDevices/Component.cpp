/*
 * Component.cpp
 *
 *  Created on: Jun 25, 2017
 *      Author: Attilio
 */
#include "Component.h"

using namespace std;

Component::Component(string dir, string nm): MParams::MParams(dir, nm) {
	load();
	check();
}

string Component::name() {
	return get("name","generic");
}

string Component::status() {
	return get("status","generic");
}

string Component::active() {
	if(lmode=="" || lmode=="none") sync();
	return lmode;
}

string Component::setValue(string v) {
	string cmd;
	string answer;
	string res;
	add("status","generic",v);
	if(get("active")=="OFF") return v;
	if((cmd=get(v,"cmd")) == "") {
		string info = "ERROR: "+get("name","generic")+" doesn't have command "+v;
		hsrv::logger->info(info);
		add("active","OFF");
		return v;
	}
	if((answer=get(v,"answer")) == "") {
		string info = "ERROR: "+get("name","generic")+" doesn't have answer for "+v;
		hsrv::logger->info(info);
                add("active","OFF");
                return v;
	}
	cmd = get(v,"cmd");
	if(!mkAgentRPC()) {
		add("active", "OFF");
		return "status";
	}
        string request = "POST "+cmd+" HTTP/1.1\r\n\r\n\r\n\r\n";
	res = rpc->sendStringMessage(request);
	if(res.find(answer)!=string::npos) {
		return v;
	}
	add("active","generic","OFF");
	return v;
}

bool Component::mkAgentRPC() {
	if(remote_ag_addr == ""||remote_ag_addr == "none") {
		NameList addr;
		remote_ag_addr = AgentNet::getAgentOf(get("name"));
		if(remote_ag_addr == ""||remote_ag_addr == "none") return false;
		addr.init(remote_ag_addr,':');
		rpc = new RpcCall(addr[1],addr[0]);
	}
	return true;
}

bool Component::getSubjects(MParams& subj) {
	subj.add(name(),"subject", "cmd");
}

bool Component::getObservations(MParams& obs) {
	obs.add(get("event","generic"),"observe", "event");
}

bool Component::check() {
  return true;
}

bool Component::sync() {
  string getreq = get("statusread", "cmd");
  string request = "";
  if(getreq == "") request = "GET /DynamicPage/"+get("name")+".xml HTTP/1.1\r\n\r\n\r\n\r\n";
  else request = getreq;
  if(!mkAgentRPC()) return false;
  string res = rpc->sendStringMessage(request); 
  if(getValue(res, get("active","answer")) == "ON") {
	this->add("active", "generic", "ON");
	this->add("status", "generic", getValue(res, this->get("status", "answer")));
  }
  else {
	this->add("active", "generic", "OFF");
  }
  return true;
}

string Component::getValue(string& body, string word) {
   string tmp = "";
   size_t pos=body.find(word, 0);
   if(pos == string::npos) return "";
   pos+= word.size();
   if(pos >= body.size()) return "";
   while(pos<body.size() && body[pos] != '"') pos++;
   pos++;
   while(pos<body.size() && body[pos] != '"') tmp += body[pos++];
   return tmp;
}

