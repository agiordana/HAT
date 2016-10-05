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
    
    if(mklock()<0) {
        cout << "a copy of this iopwragent is already running! abort!" << endl; 
        exit(-1);
    };
	
    setStatus();
    trigger = new Trigger("trigger");
    
    receiver = new Receiver("Receiver");
    receiver->init();
    
    archive_thread = new ArchiveThread("ArchiveThread");
    archive_thread->init();    

    aginterface = new AgentInterface("AgentInterface");
    aginterface->init();

    push_thread = new PushThread("PushThread");
    push_thread->init();

    remote_server = new RemoteServerThread("RemoteServerThread");
    remote_server->init();

    string info = "Agent: "+ hsrv::agentname + " Started";
    hsrv::logger->info(info, __FILE__, __FUNCTION__, __LINE__);

    // TEMP -> effettua lo start di tutti i gateway caricati
    AgentNet::init(hsrv::agentnet);
    trigger->start();
    receiver->start();
    aginterface->start();
    archive_thread->start();    
    push_thread->start();
    remote_server->start();

    if(_DEBUG_) SubjectSet::printSet();

    AgentNet::SubjectRegister();
    
    if (!start_rest_server()) {
      exit(-1);
    }

}



bool Agent::install() {
    string obs = hsrv::homedir+"/observers";
    if(!FileManager::makeDir(hsrv::homedir, true)) {
	exit(-1);
    }
    
    if(!FileManager::makeDir(obs, true)) {
       exit(-1);
    }

    hsrv::archive = new ArchiveManager("archive");

    if (!hsrv::archive->init()) {
        exit(-1);
    }
    
    hsrv::router = new RpcRoutingMap(hsrv::configdir+"/route.xml");  
    DeviceManager::init("devicemanager");
    AlarmManager::init("alarmmanager");
    AutomatismManager::init("automatismmanager");
    AuthManager::init("authmanager");
    LogManager::init("logmanager");
    PwrManager::init("pwrmanager");
    SrvManager::init("srvmanager");
    ConfManager::init("confmanager");

    return true;
}
