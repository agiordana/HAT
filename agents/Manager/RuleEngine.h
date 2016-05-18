//
//  RuleEngine.h
//  soapagent
//
//  Created by Attilio Giordana on 2/4/13.
//  Copyright (c) 2013 Università del Piemonte Orientale. All rights reserved.
//

#ifndef _RULE_ENGINE
#define _RULE_ENGINE

#include "agentlib.h"
#include "ObserverThread.h"
#include <boost/thread.hpp>
#include "eventcounter.h"
#include "ruleset.h"
//#include "timedruleset.h"
#include "BooleanConditionSet.h"

class RuleEngine: public ObserverThread {
public:
    RuleEngine(std::string n);
    ~RuleEngine();
    void start_service_threads();
    bool getObservations(MParams& obs);
    bool getSubjects(MParams& subj);
private:
    std::string wait_after_on;
    std::string wait_forbidden;
    EventCounterSet* event_counter;
    BooleanConditionSet* condition;
    RuleSet* rule;
    MMessage program;
    bool alarm_on;
    std::vector<std::string> active;
    void do_work(RuleEngine* obj);
    bool makeEventcounterPage(EventCounterSet& ec);
    bool makeConditionPage(BooleanConditionSet& cnd);
    bool makeRulePage(RuleSet& r);
    bool makeParameterPage(std::string wf, std::string wt);
};

#endif
