//
//  Control.cpp
//  engagent
//
//  Created by Attilio Giordana on 7/15/14.
//  Copyright (c) 2014 Università del Piemonte Orientale. All rights reserved.
//

#include "Control.h"

using namespace std;

int Control::priority() {
    string tmp = get("priority");
    if(tmp=="") return 0;
    return hsrv::a2int(tmp);
}

bool Control::loadestimateUpdate(double value) {
    load_estimate = value;
    upd_time = hsrv::gettime();
    return true;
}

bool Control::mayStart(double available) {
    double load;
    if(wait_until>hsrv::gettime()) return false;
    if(!isstatusON()||ismodeON()) return false;
    if(load_estimate == 0) {
        string tmp = get("nominalload");
        load = hsrv::a2double(tmp);
    }
    else load = load_estimate;
    if(available * 0.9 >= load) return true;
    else return false;
}

bool Control::mayStop() {
    if(wait_until>hsrv::gettime()) return false;
    if(isstatusON() && ismodeON()) return true;
    return false;
}

bool Control::stop(setof<MMessage>& action, bool force) {
    if(isstatusON() && (ismodeON()||force)) {
        if(ismodeON()) {
	   set("mode", "OFF");
	   save();
        }
        if(latency <0) {
            string tmp = get("latency");
            latency = hsrv::a2double(tmp);
        }
        wait_until = hsrv::gettime() + latency;
        MMessage msg("cmd", get("control"));
        msg.add("value", "OFF");
        action.push_back(msg);
        return true;
    }
    return false;
}

bool Control::start(setof<MMessage>& action, bool force) {
    if(isstatusON() && (!ismodeON()||force)){
	if(!ismodeON()) {
           set("mode", "ON");
           save();
        }
        if(latency <0) {
            string tmp = get("latency");
            latency = hsrv::a2double(tmp);
        }
        wait_until = hsrv::gettime() + latency;
        MMessage msg("cmd", get("control"));
        msg.add("value", "ON");
        action.push_back(msg);
        return true;
    }
    return false;
}
