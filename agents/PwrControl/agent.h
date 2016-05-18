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
#include "agentnet.h"
#include "Controller.h"

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
    
    Controller* controller;
        
    /**
     * Controlla se è presente la directory home dell'agente e in caso la crea
     * @return true se la directory è stata creata o se esiste già. In caso di 
     *         errore termina l'esecuzione del programma
     */
	bool install();
    
};

#endif
