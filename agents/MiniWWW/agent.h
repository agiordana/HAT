/*
 *  agent.h
 *  ioagent
 *
 *  Created by Attilio Giordana on 1/5/12.
 *  Copyright 2012 Università del Piemonte Orientale. All rights reserved.
 *
 */

#ifndef _MINIWWW_
#define _MINIWWW_

#include "agentlib.h"
#include "ObserverTCP.h"
#include "agentnet.h"
#include "Configurator.h"
#include "ConfRoutingMap.h"

class Agent : public AgentConf {
public:
    Agent();    
protected:
    /**
     @var Trigger*
     @brief Istanza della classe Trigger che è responsabile della generazione dei 
     *      messaggi di tipo poll e update 
     */
    
	bool mkHomePage();
	bool mkInstallPage();
	bool mkIndexPage();
	bool getAgentList(NameList& ag, bool all=false);
        std::string getBBlist();
	std::string get_port_of(std::string);
	std::string get_description_of(std::string);
};

#endif
