//
//  Configurator.cpp
//  mngagent
//
//  Created by Attilio Giordana on 1/10/15.
//  Copyright (c) 2015 Università del Piemonte Orientale. All rights reserved.
//

#include "Configurator.h"

using namespace std;

Configurator::Configurator(string n): RouterExec(n) {
   MParams p("threads", "Configurator");
   p.load();
}
   

MMessage Configurator::exec(std::string uri, std::vector<std::string> params, std::string method, std::string body) {
    std::string response, devname, filetype="";
    MMessage msg, mresp;
    NameList nl;
    size_t pos;
    nl.init(uri, '/');
    if(!hsrv::checkConfigureEnabled()) {
        response = "Not Found";
        mresp.add("body", response);
        return mresp;
    }
    if(method != "configure") {
	response = "Not Found";
        mresp.add("body", response);
        return mresp;
    }
    if(body != "")  addBody(msg, body);
    if(params.back() == "install") {
	if(installer(msg)) response = "done";
	else response = "Fail";
    }
    if(params.back() == "reset") {
	if(doReset()) response = "done";
	else response = "Fail";
    }
    else if(params.back() == "reboot") {
	response = "done";
	system("/sbin/reboot");
    }
    mresp.add("body", response);
    return mresp;
}  

bool Configurator::installer(MMessage& m) {
   NameList agent;
   NameList oldagent;
   string cmd; 
   size_t i;
   string configuration = CONFIGURATIONS;
   string conf;
   string pathsrc;
   string pathdst;
   conf = FileManager::getRoot(hsrv::configdir);
   if(!FileManager::isDir(configuration)) configuration = CONFIGURATIONS1;
   if(!FileManager::isDir(configuration)) configuration = CONFIGURATIONS2;
   if(!FileManager::isDir(configuration)) return false;
   FileManager::dirList(conf, oldagent);
   for(size_t i=0; i<oldagent.size(); i++)
      if(oldagent[i] != "miniwww") {
         string delpath = conf + "/" + oldagent[i];
	 FileManager::deleteDir(delpath);
      }
   agent.init(m.getString("agents"),'+');
   for(size_t i=0; i<agent.size(); i++) {
	pathsrc = configuration + "/" +agent[i];
	pathdst = conf + "/" +FileManager::getStem(agent[i]);
        cmd = "/bin/cp -r "+pathsrc+" "+pathdst;
        hsrv::cmdExec(cmd);
	if(agent[i] == "wsserverws") {
	    pathsrc = configuration+"/wspusher";
	    pathdst = conf + "/websocket";
	    cmd = "/bin/cp -r "+pathsrc+" "+pathdst;
	    hsrv::cmdExec(cmd);
	    pathsrc = configuration+"/sshtunnel";
	    pathdst = conf + "/sshtunnel";
	    cmd = "/bin/cp -r "+pathsrc+" "+pathdst;
	    hsrv::cmdExec(cmd);
	}
	if(agent[i] == "wsserver") {
	    pathsrc = configuration+"/websocket";
	    pathdst = conf + "/websocket";
	    cmd = "/bin/cp -r "+pathsrc+" "+pathdst;
	    hsrv::cmdExec(cmd);
	    pathsrc = configuration+"/sshtunnel";
	    pathdst = conf + "/sshtunnel";
	    cmd = "/bin/cp -r "+pathsrc+" "+pathdst;
	    hsrv::cmdExec(cmd);
	}
   } 
   return true;
}

bool Configurator::addBody(MMessage& m, std::string body) {
   if(hsrv::isXmlstring(body)) {
      m.xmlAppend(body);
   }
   else if(isHtml(body)) {
      m.htmlAppend(body);
   } else {
      m.add("body", body);
   }
   return true;
}

bool Configurator::isHtml(std::string& body) {
   if(body.find("&")!=std::string::npos) return true;
   else return false;
}

bool Configurator::doReset() {
   NameList oldagent;
   string cmd; 
   string conf;
   conf = FileManager::getRoot(hsrv::configdir);
   FileManager::dirList(conf, oldagent);
   for(size_t i=0; i<oldagent.size(); i++)
      if(oldagent[i] != "miniwww") {
         string delpath = conf + "/" + oldagent[i];
	 FileManager::deleteDir(delpath);
      }
   cmd = "/sbin/reboot";
   hsrv::cmdExec(cmd);
   return true;
}
