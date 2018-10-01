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

    if(mklock()<0) {
        cout << "a copy of this iopwragent is already running! abort!" << endl; 
        exit(-1);
    };

    hsrv::router = new RoutingMap(hsrv::configdir+"/route.xml");
	
    setStatus();
    trigger = new Trigger("trigger");
    
    filter = new MdFilter("MdFilter");
    filter->init();
    
    dirwatch = new DirWatch("DirWatch");
    dirwatch->init();
    

    string info = "Agent: "+ hsrv::agentname + " Started";
    hsrv::logger->info(info, __FILE__, __FUNCTION__, __LINE__);

    // TEMP -> effettua lo start di tutti i gateway caricati
    AgentNet::init(hsrv::agentnet);
    filter->start();
    dirwatch->start();
    trigger->start();
    if(_DEBUG_) SubjectSet::printSet();

    AgentNet::SubjectRegister();

    if (!start_rest_server()) {
      exit(-1);
    }  
}
