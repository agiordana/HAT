//
//  Controller.h
//  soapagent
//
//  Created by Attilio Giordana on 2/4/13.
//  Copyright (c) 2013 Università del Piemonte Orientale. All rights reserved.
//

#ifndef _CONTROLLER
#define _CONTROLLER

#include "agentlib.h"
#include <boost/thread.hpp>
#include "Scheduler.h"
#include "ObserverThread.h"


class Controller: public ObserverThread {
public:
    Controller(std::string n);
    ~Controller();
    void start_service_threads();
    bool getObservations(MParams& obs);
    bool getSubjects(MParams& subj);
private:
    Scheduler* scheduler;
    std::string name;
    void do_work(Controller* obj);
    bool makeSchedulerPage(Scheduler& s);
    };

#endif
