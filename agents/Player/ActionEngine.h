//
//  ActionEngine.h
//  mngagent
//
//  Created by Attilio Giordana on 3/24/13.
//  Copyright (c) 2013 Università del Piemonte Orientale. All rights reserved.
//

#ifndef mngagent_ActionEngine_h
#define mngagent_ActionEngine_h

#include "agentlib.h"
#include <boost/thread.hpp>
#include "ObserverThread.h"
#include "actionset.h"

class ActionEngine: public ObserverThread {
public:
    ActionEngine(std::string);
    ~ActionEngine();
    void start_service_threads();
    bool getSubjects(MParams& subj);
private:
    ActionSet* actions;
    MMessage program;
    void do_work(ActionEngine* obj);
    bool makeActionPage(ActionSet& actions);
    bool makeActiveActionPage(ActionSet&, MMessage&);
};


#endif
