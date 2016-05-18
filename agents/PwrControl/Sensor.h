//
//  Control.h
//  engagent
//
//  Created by Attilio Giordana on 7/14/14.
//  Copyright (c) 2014 Università del Piemonte Orientale. All rights reserved.
//

#ifndef engagent_Sensor_h
#define engagent_Sensor_h

#include "agentlib.h"

class Sensor: public MParams {
public:
    Sensor(std::string dir, std::string n): MParams(dir, n){last_update=hsrv::gettime();};
    double last_update;
    bool isstatusON(){return get("status")=="ON";};
    bool ismeasure(){return get("control_measure")=="YES";};
    bool observations(MParams& obs){obs.add(this->get("name"), "observe", "event"); return true;};
    bool setMeasure(std::string md){
        if(md!="YES" && md!="NO") return false; 
        last_update = hsrv::gettime();
        set("control_measure", md);
        save();
        return true;
    };
    bool setStatus(std::string md){
        if(md!="ON" && md!="OFF") return false;
        set("status", md);
        save();
        return true;
    };
    bool setThreshold(std::string md){
        if(md=="") return false;
        set("threshold", md);
        save();
        return true;
    };
};


#endif
