//
//  BooleanConditionSet.h
//  mngagent
//
//  Created by Attilio Giordana on 1/1/15.
//  Copyright (c) 2015 Università del Piemonte Orientale. All rights reserved.
//

#ifndef mngagent_BooleanConditionSet_h
#define mngagent_BooleanConditionSet_h

#include "BooleanCondition.h"

class BooleanConditionSet: public setof<BooleanCondition> {
public:
    BooleanConditionSet(std::string dir);
    bool eventUpdate(MMessage& m);
    bool observations(MParams& obs);
    bool reset();
};


#endif
