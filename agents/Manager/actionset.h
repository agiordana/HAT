//
//  actionset.h
//  mngagent
//
//  Created by Attilio Giordana on 3/24/13.
//  Copyright (c) 2013 Università del Piemonte Orientale. All rights reserved.
//

#ifndef mngagent_actionset_h
#define mngagent_actionset_h
#include "agentlib.h"
#include "action.h"

class ActionSet: public setof<Action> {
public:
    ActionSet(std::string, std::string);
    bool exec(std::string);
    bool setStatus(std::string, std::string);
    bool subjects(MParams& subj);
    bool observations(MParams& obs);
    bool isActive(std::string);
};


#endif
