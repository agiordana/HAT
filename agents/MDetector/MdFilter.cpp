//
//  MdFilter.cpp
//  soapagent
//
//  Created by Attilio Giordana on 2/4/13.
//  Copyright (c) 2013 Università del Piemonte Orientale. All rights reserved.
//

using namespace std;

#include "MdFilter.h"

MdFilter::MdFilter(string n) {
    id = n;
    MParams *commands = new MParams("commands");
    commands->load();
    filterCMD = commands->get("filter");
}

MdFilter::~MdFilter() {

}


std::string MdFilter::path2Url(std::string path) {
    NameList nl;
    string url;
    nl.init(path, '/');
    if(nl.size() <= 2) return path;
    url = nl[nl.size()-2]+"/"+nl[nl.size()-1];
    return url;
}

void MdFilter::start_service_threads() {
    this->service_thread.push_back(new Thread(&MdFilter::do_work, this));
}

void MdFilter::do_work(MdFilter* obj) {
    MMessage out_mess;
    MMessage::iterator it;
    MMessage msg;
    string current_time;

    while(1) {        
	msg = obj->receive_message();
	string pathname = msg.getString("pathname");
	string parentDir = hsrv::archive->getParentDirectory(pathname);
	if(msg.mtype=="local_event" && msg.getString("status") == "OFF") {
	   FileManager::deleteFile(parentDir, FileManager::getTail(pathname));
	}
	else if(msg.mtype=="local_event") {
	  string response;
	  if(filterCMD == "") response = "ALARM\n";
	  else {
	    string cmd = filterCMD;
	    hsrv::strReplace(cmd, "$INPUT", pathname);
	    response = hsrv::cmdExec(cmd);
	  }
	  if (response == "ALARM\n") {
	    
	    //move the motion event video into the archive
	    string evtime = hsrv::getasciitimecompact(); 

	    //notify
	    msg.mtype = "event";
	    msg.msubtype = msg.getString("event_source");
	    msg.remove("pathname");
	    msg.add("video", path2Url(pathname));
	    msg.add("value", "ON");
	    msg.add("evtime", evtime);
	    /**********************/
	    SubjectSet::notify(msg);
	    hsrv::logger->info("ACCEPTED: "+FileManager::getTail(pathname));
	    
	  }
	  parentDir = hsrv::archive->getParentDirectory(pathname);
	  FileManager::deleteFile(parentDir, FileManager::getTail(pathname));
	}
    }
}

