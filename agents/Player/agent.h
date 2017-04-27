/*
 *  agent.h
 *  ioagent
 *
 *  Created by Attilio Giordana on 1/5/12.
 *  Copyright 2012 Università del Piemonte Orientale. All rights reserved.
 *
 */

#ifndef _IOPWRAGENT_
#define _IOPWRAGENT_

#include "agentlib.h"
#include "ObserverTCP.h"
#include "Trigger.h"
#include "agentnet.h"
#include "RuleEngine.h"
#include "ActionEngine.h"
#include "Configurator.h"
#include "ConfRoutingMap.h"

class Agent: public AgentConf {
public:
    Agent();    
protected:
    /**
     @var Trigger*
     @brief Istanza della classe Trigger che è responsabile della generazione dei 
     *      messaggi di tipo poll e update 
     */
    Trigger* trigger;
    
    RuleEngine* rule_engine;
    
    
    ActionEngine* action_engine;

    Configurator* configurator;
    
};

#endif
