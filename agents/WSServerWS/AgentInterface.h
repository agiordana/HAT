/* 
 * File:   AgentInterface.h
 * Author: attilio
 *
 * Created on July 10, 2015, 8:11 AM
 */

#ifndef AGENTINTERFACE_H
#define	AGENTINTERFACE_H
#include "agentlib.h"
#include "ObserverThread.h"
#include "NameTable.h"

class AgentInterface: public ObserverThread {
public:
    AgentInterface(std::string n);
    ~AgentInterface();
    void start_service_threads();
protected:
    void do_work(AgentInterface* obj);
};


#endif	/* AGENTINTERFACE_H */

