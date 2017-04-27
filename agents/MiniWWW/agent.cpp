/*
 *  agent.cpp
 *  iopwragent
 *
 *  Created by Davide Monfrecola on 07/11/12.
 *  Copyright 2012 Università del Piemonte Orientale. All rights reserved.
 *
 */

using namespace std;

#include "agent.h"
#include "Subject.h"
#include "agentnet.h"

Agent::Agent() {
    NameList pp;
    if(!install()) exit(-1);

    sethomedir();

    hsrv::archive = new ArchiveManager("archive");
    if (!hsrv::archive->init()) {
        exit(-1);
    }

    hsrv::router = new ConfRoutingMap(hsrv::configdir+"/route.xml");
    
    if(mklock()<0) {
        cout << "a copy of this iopwragent is already running! abort!" << endl; 
        exit(-1);
    };
	
    setStatus();
    
    string info = "Agent: "+ hsrv::agentname + " Started";
    hsrv::logger->info(info, __FILE__, __FUNCTION__, __LINE__);

    // TEMP -> effettua lo start di tutti i gateway caricati
    AgentNet::init(hsrv::agentnet);
    if(_DEBUG_) SubjectSet::printSet();
    AgentNet::SubjectRegister();
    mkHomePage();
    if (!start_rest_server()) {
      exit(-1);
    }
}

bool Agent::mkHomePage() {
    NameList agl;
    getAgentList(agl, true);
    if(agl.size() == 1 && agl[0] == "miniwww") return mkInstallPage();
    if(agl.size() > 1) return mkIndexPage();
    return false;
}

bool Agent::mkInstallPage() {
   string page="";
   char ch;
   string templpath = hsrv::configdir+"/installtemplate.html";
   string indexpath = hsrv::configdir+"/doc/index.html";
   string capechoice = getBBlist();
   ifstream in(templpath.c_str());
   while((ch = in.get())&&!in.eof()) page += ch;
   in.close();
   hsrv::strReplace(page, "$CAPETYPE", capechoice);
   ofstream out(indexpath.c_str());
   for(size_t i=0; i<page.size(); i++) out.put(page[i]);
   out.close();
   return true;
}

bool Agent::mkIndexPage() {
   string page="";
   char ch;
   string host = hsrv::cmdExec("hostname");
   string ip = AgentNet::getAgentIp();
   NameList agent;
   getAgentList(agent);
   string templpath = hsrv::configdir+"/indextemplate.html";
   string indexpath = hsrv::configdir+"/doc/index.html";
   ifstream in(templpath.c_str());
   while((ch = in.get())&&!in.eof()) page += ch;
   in.close();
   hsrv::strReplace(page, "$HOSTNAME", host);
   string tpage = "";
   for(size_t i=0; i<agent.size(); i++) {
     tpage+= "<li> <a href=\"http://";
     tpage+= ip;
     tpage+=":";
     tpage+=get_port_of(agent[i]);
     tpage+="\">";
     tpage+= get_description_of(agent[i]);
     tpage+="</a> </li>\n";
   }
   hsrv::strReplace(page, "$AGENTS", tpage);
   ofstream out(indexpath.c_str());
   for(size_t i=0; i<page.size(); i++) out.put(page[i]);
   out.close();
   return true;
}

bool Agent::getAgentList(NameList& ag, bool all) {
   string conf = FileManager::getRoot(hsrv::configdir);
   NameList tmp;
   tmp.clear();
   size_t i;
   FileManager::dirList(conf, tmp);
   for(i=0; i<tmp.size(); i++)
     if(all) ag.push_back(tmp[i]);
     else if(tmp[i] != "miniwww" && tmp[i] != "logger" && 
			tmp[i]!="sshtunnel" && tmp[i]!="websocket") ag.push_back(tmp[i]);
   return true;
}

string  Agent::get_port_of(string ag) {
   string conf = hsrv::configdir;
   size_t i;
   for(i=conf.size()-1; i>0 && conf[i]!='/'; i--);
   if(i==0) return "";
   conf = conf.substr(0,i+1);
   conf+=ag;
   string agn = conf+="/agentnet.xml";
   MParams agnet;
   agnet.xmlLoad(agn); 
   return agnet.get("port");
}

string  Agent::get_description_of(string ag) {
   string conf = hsrv::configdir;
   size_t i;
   for(i=conf.size()-1; i>0 && conf[i]!='/'; i--);
   if(i==0) return "";
   conf = conf.substr(0,i+1);
   conf+=ag;
   string global = conf+="/global.xml";
   MParams agglobal;
   agglobal.xmlLoad(global); 
   return agglobal.get("description");
}

string Agent::getBBlist() {
   string choice = "";
   NameList bbl;
   string configuration = CONFIGURATIONS;
   FileManager::dirList(configuration, bbl);
   for(size_t i=0; i<bbl.size(); i++) {
      if(FileManager::getStem(bbl[i]) == "bbdevices") {
         string tchoice = "<input type=\"radio\" class=\"inrb\" name=\"boardname\" value=\""+bbl[i]+"\">" + FileManager::getExt(bbl[i]) +"\n";
	 choice += tchoice;
      }
   }
   return choice;
}
