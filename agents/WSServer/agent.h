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
#include "Receiver.h"
#include "ArchiveThread.h"
#include "PushThread.h"
#include "RemoteServerThread.h"
#include "AgentInterface.h"
#include "RpcRoutingMap.h"

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

    Receiver* receiver;

    ArchiveThread* archive_thread;
    
    PushThread *push_thread;

    RemoteServerThread *remote_server;

    AgentInterface* aginterface;
};

#endif
