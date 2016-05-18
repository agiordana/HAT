//
//  timedruleset.h
//  mngagent
//
//  Created by Attilio Giordana on 3/28/12.
//  Copyright (c) 2012 Università del Piemonte Orientale. All rights reserved.
//

#ifndef mngagent_timedruleset_h
#define mngagent_timedruleset_h

#include "timedrule.h"

class TimedRuleSet: public setof<TimedRule> {
public:
    bool timeUpdate(std::string, int, BooleanConditionSet*, setof<MMessage>&);
    TimedRuleSet(std::string);
    bool subjects(MParams& subj);
    bool setStatus(std::string, std::string);
    bool setValidityTime(std::string, std::string, std::string);
    bool setActionList(std::string, std::string, std::string);
};

#endif
