//
//  Scheduler.h
//  engagent
//
//  Created by Attilio Giordana on 7/14/14.
//  Copyright (c) 2014 Università del Piemonte Orientale. All rights reserved.
//

#ifndef engagent_Scheduler_h
#define engagent_Scheduler_h

#include "agentlib.h"
#include "ControlSet.h"
#include "SensorSet.h"
#include "NameService.h"

class Scheduler: public MParams {
public:
    Scheduler(std::string);
    bool checkControl(MMessage&, setof<MMessage>&);
    bool aligneControl(setof<MMessage>&);
    bool subjects(MParams& subj){ if(control) return control->subjects(subj); return false;};
    bool observations(MParams& obs);
    bool setControl(MMessage&);
    bool resetControl(MMessage&);
    bool setTarget(MMessage&);
    bool resetTarget(MMessage&);
    bool makePage();
    bool makeControlDevices();
    bool makeSensorDevices();
    bool configure() {
	return makeControlDevices() && makeSensorDevices();
    };
private:
    ControlSet* control;
    SensorSet* sensor;
    bool reduceLoad(double, setof<MMessage>&);
    bool tryIncreaseLoad(double, setof<MMessage>&);
    size_t last_stopped;
    std::string threshold;
    std::string control_measure;
    double stop_time;
    double targetvalue_atstop;
    double current;
};


#endif
