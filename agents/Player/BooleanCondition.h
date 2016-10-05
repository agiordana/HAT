//
//  BooleanCondition.h
//  mngagent
//
//  Created by Attilio Giordana on 1/1/15.
//  Copyright (c) 2015 Università del Piemonte Orientale. All rights reserved.
//

#ifndef mngagent_BooleanCondition_h
#define mngagent_BooleanCondition_h

#include "agentlib.h"

class BooleanCondition: public MParams {
public:
    std::string name;
    BooleanCondition(std::string, std::string);
    double eventUpdate(MMessage& m);
    bool isActive(){return value > threshold;};
    bool observations(MParams& obs);
    bool reset() {value = 0; return true;};
protected:
    double value;
    double threshold;
    NameList alias;
    bool match(MMessage& m);
    double last_time;
};


#endif
