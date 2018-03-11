/*
 *  agent.h
 *  ioagent
 *
 *  Created by Attilio Giordana on 1/5/12.
 *  Copyright 2012 Università del Piemonte Orientale. All rights reserved.
 *
 */

#ifndef _MDETECTOR_
#define _MDETECTOR_

#include "agentlib.h"
#include "ObserverTCP.h"
#include "Trigger.h"
#include "agentnet.h"
#include "MdFilter.h"
#include "DirWatch.h"

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
    
    MdFilter* filter;

    DirWatch* dirwatch;
    
};

#endif
