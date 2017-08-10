/*
 *  agent.cpp
 *  iopwragent
 *
 *  Created by Davide Monfrecola on 07/11/12.
 *  Copyright 2012 Università del Piemonte Orientale. All rights reserved.
 *
 */

#include "agent.h"
#include "Subject.h"
#include "agentnet.h"

using namespace std;

Agent::Agent() {
    if(!install()) exit(-1);

    sethomedir();
 
    hsrv::archive = new ArchiveManager("archive");
    if (!hsrv::archive->init()) {
        exit(-1);
    }

    hsrv::router = new ConfRoutingMap(hsrv::configdir+"/route.xml");

    
    if(mklock()<0) {
        cout << "a copy of this netdevices is already running! abort!" << endl; 
        exit(-1);
    };
	
    setStatus();
    trigger = new Trigger("trigger");

    string info = "Agent: "+ hsrv::agentname + " Started";
    hsrv::logger->info(info, __FILE__, __FUNCTION__, __LINE__);

    mkDevices();

    // TEMP -> effettua lo start di tutti i gateway caricati
    AgentNet::init(hsrv::agentnet);
    
    trigger->start();
    // starts device threads
    for(size_t i=0; i<device.size(); i++) device[i]->start();

    if(_DEBUG_) SubjectSet::printSet();
    AgentNet::SubjectRegister();

    if(!start_rest_server()) {
        exit(-1);
    }
}

bool Agent::mkDevices() {
   vector<string> dev;
   string dev_dir = hsrv::configdir+"/devices";
   string config_dir;
   Device* curdev;
   FileManager::dirList(dev_dir, dev);
   for(size_t i=0; i<dev.size(); i++) {
      config_dir = "devices/"+dev[i];
      device.push_back(new Device(config_dir, dev[i]));
      device[device.size()-1]->init();
   }
   for(size_t i=0; i<device.size(); i++) device[i]->setSync(device.size(),i);
   return true;
}
