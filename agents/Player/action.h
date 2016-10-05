//
//  action.h
//  mngagent
//
//  Created by Attilio Giordana on 3/24/13.
//  Copyright (c) 2013 Università del Piemonte Orientale. All rights reserved.
//

#ifndef mngagent_action_h
#define mngagent_action_h
#include "agentlib.h"

class Action: public MParams {
public:
    std::string name;
    Action(std::string, std::string);
    bool setStatus(std::string);
    bool exec(std::string);
    bool observations(MParams& obs);
    bool subjects(MParams& subj);
    int member(std::string);
};


#endif
