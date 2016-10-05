//
//  BooleanCondition.cpp
//  mngagent
//
//  Created by Attilio Giordana on 1/1/15.
//  Copyright (c) 2015 Università del Piemonte Orientale. All rights reserved.
//

#include "BooleanCondition.h"

using namespace std;

BooleanCondition::BooleanCondition(string dir, string nm): MParams::MParams(dir, nm) {
    load();
    if(get("name") == "") name = nm;
        else name = get("name");
    string tmp = get("threshold");
    threshold = hsrv::a2double(tmp);
    alias.init(get("alias"));
    last_time = hsrv::gettime();
    value = 0;
}

double BooleanCondition::eventUpdate(MMessage& m) {
    if (match(m)) {
        string tvalue = m.getString("value");
        if(tvalue == "ON" || tvalue == "On" || tvalue == "on") value = 1;
        else if(tvalue == "OFF" | tvalue == "Off" || tvalue == "off") value = 0;
        else value = hsrv::a2double(tvalue);
    }
    return value;
}

bool BooleanCondition::match(MMessage& m) {
    string name = m.msubtype;
    string source = m.getString("source");
    return (name==ty || alias.member(name) || m.getString(source) == ty || alias.member(source));
}

bool BooleanCondition::observations(MParams &obs) {
    string nn = this->get("name");
    obs.add(nn,"observe","event");
    for(size_t i=0; i<alias.size(); i++)
        if(nn != alias[i]) obs.add(alias[i], "observe", "event");
    return true;
}
