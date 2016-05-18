//
//  Control.h
//  engagent
//
//  Created by Attilio Giordana on 7/14/14.
//  Copyright (c) 2014 Università del Piemonte Orientale. All rights reserved.
//

#ifndef engagent_Control_h
#define engagent_Control_h

#include "agentlib.h"

class Control: public MParams {
public:
    Control(std::string dir, std::string n): MParams(dir, n){load_estimate = 0; wait_until = 0; latency = -1;};
    double last_update;
    bool isstatusON(){return get("status")=="ON";};
    bool ismodeON(){return get("mode")=="ON";};
    bool subjects(MParams& subj){subj.add(this->get("control"), "subject", "cmd"); return true;};
    bool observations(MParams& obs){obs.add(this->get("name"), "observe", "event"); return true;};
    bool setMode(std::string md){
        if(md!="ON" && md!="OFF") return false; 
        last_update = hsrv::gettime();
        set("mode", md);
        save();
        return true;
    };
    bool setStatus(std::string md){
        if(md!="ON" && md!="OFF") return false;
        set("status", md);
        save();
        return true;
    };
    bool setLoad(std::string md){
        if(md=="") return false;
        set("nominalload", md);
        save();
        return true;
    };
    bool setPriority(std::string md){
        if(md=="") return false;
        set("priority", md);
        save();
        return true;
    };
    int priority();
    bool stop(setof<MMessage>& action, bool force=false);
    bool start(setof<MMessage>& action, bool force=false);
    bool mayStart(double);
    bool mayStop();
    bool loadestimateUpdate(double);
private:
    double latency;
    double load_estimate;
    double upd_time;
    double wait_until;
};


#endif
