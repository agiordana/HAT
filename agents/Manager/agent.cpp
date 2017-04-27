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
 
    hsrv::archive = new ArchiveManager("archive");
    if (!hsrv::archive->init()) {
        exit(-1);
    }

    hsrv::router = new RoutingMap(hsrv::configdir+"/route.xml");

    
    if(mklock()<0) {
        cout << "a copy of this iopwragent is already running! abort!" << endl; 
        exit(-1);
    };
	
    setStatus();
    trigger = new Trigger("trigger");
    
    rule_engine = new RuleEngine("RuleEngine");
    rule_engine->init();
    
    timedrule_engine = new TimedRuleEngine("TimedRuleEngine");
    timedrule_engine->init();
    
    program_engine = new ProgramEngine("ProgramEngine");
    program_engine->init();
    
    action_engine = new ActionEngine("ActionEngine");
    action_engine->init();
    
    string info = "Agent: "+ hsrv::agentname + " Started";
    hsrv::logger->info(info, __FILE__, __FUNCTION__, __LINE__);

    // TEMP -> effettua lo start di tutti i gateway caricati
    AgentNet::init(hsrv::agentnet);
    
    action_engine->start();
    
    program_engine->start();
    
    rule_engine->start();

    trigger->start();
    
    timedrule_engine->start();
    
    if(_DEBUG_) SubjectSet::printSet();
    AgentNet::SubjectRegister();

    if(!start_rest_server()) {
        exit(-1);
    }
}

