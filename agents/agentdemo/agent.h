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
#include "DummySource.h"
#include "DummyPit.h"

class Agent : public AgentConf {
public:
    Agent();    
protected:
    /**
     @var Trigger*
     @brief Istanza della classe Trigger che è responsabile della generazione dei 
     *      messaggi di tipo poll e update 
     */
    Trigger* trigger;
    
    DummySource* dummy;
    
    DummyPit* dpit;
};

#endif
