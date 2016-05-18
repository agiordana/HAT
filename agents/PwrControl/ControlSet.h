//
//  controlset.h
//  engagent
//
//  Created by Attilio Giordana on 7/14/14.
//  Copyright (c) 2014 Università del Piemonte Orientale. All rights reserved.
//

#ifndef engagent_controlset_h
#define engagent_controlset_h

#include "Control.h"

class ControlSet: public std::vector<Control> {
public:
    ControlSet(std::string dir);
    bool subjects(MParams&);
    bool observations(MParams&);
    bool setControl(MMessage&);
    bool resetControl(MMessage&);
    bool handleMode(MMessage&);
};

#endif
