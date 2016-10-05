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
    NameList pp;
    if(!install()) exit(-1);
    sethomedir();
    
    if(mklock()<0) {
        cout << "a copy of this iopwragent is already running! abort!" << endl; 
        exit(-1);
    };
	
    setStatus();
    trigger = new Trigger("trigger");
    
    rule_engine = new RuleEngine("RuleEngine");
    rule_engine->init();
    
    action_engine = new ActionEngine("ActionEngine");
    action_engine->init();
    
    string info = "Agent: "+ hsrv::agentname + " Started";
    hsrv::logger->info(info, __FILE__, __FUNCTION__, __LINE__);

    // TEMP -> effettua lo start di tutti i gateway caricati
    AgentNet::init(hsrv::agentnet);
    
    action_engine->start();
    
    rule_engine->start();

    trigger->start();
    
    if(_DEBUG_) SubjectSet::printSet();
    AgentNet::SubjectRegister();

    if(!start_rest_server()) {
        exit(-1);
    }
}



bool Agent::install() {
    string obs = hsrv::homedir+"/observers";
    string actions = hsrv::homedir+"/actions";
    string tr_actions = hsrv::homedir+"/tr_actions";
    string eventcounters = hsrv::homedir+"/eventcounters";
    string rules = hsrv::homedir+"/rules";
    string conditions = hsrv::homedir+"/conditions";
    string tr_conditions = hsrv::homedir+"/tr_conditions";
    if(!FileManager::makeDir(hsrv::homedir, true)) {
    	exit(-1);
    }
    if(!FileManager::makeDir(obs, true)) {
		exit(-1);
    }
    if(!FileManager::makeDir(actions, true)) {
		exit(-1);
    }
    if(!FileManager::makeDir(tr_actions, true)) {
		exit(-1);
    }
    if(!FileManager::makeDir(eventcounters, true)) {
		exit(-1);
    }
    if(!FileManager::makeDir(eventcounters, true)) {
		exit(-1);
    }
    if(!FileManager::makeDir(rules, true)) {
		exit(-1);
    }
    if(!FileManager::makeDir(conditions, true)) {
		exit(-1);
    }
    if(!FileManager::makeDir(tr_conditions, true)) {
		exit(-1);
    }

    hsrv::archive = new ArchiveManager("archive");
    if (!hsrv::archive->init()) {
        exit(-1);
    }

    hsrv::router = new ConfRoutingMap(hsrv::configdir+"/route.xml");

	return true;
}
