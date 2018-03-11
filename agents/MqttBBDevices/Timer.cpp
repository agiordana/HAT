//
//  Timer.cpp
//  soapagent
//
//  Created by Attilio Giordana on 3/12/13.
//  Copyright (c) 2013 Università del Piemonte Orientale. All rights reserved.
//

#include "Timer.h"

using namespace std;

//------------------------------ Class TimedCondition ------------------------------

TimedCondition::TimedCondition(string a, string b, string c) {
    type = a;
    subtype = b;
    value = c;
    string duration = getDuration(c);
    wait_time = hsrv::a2double(duration);
    expired = false;
}

bool TimedCondition::update(){
    if(expired) return true; 
    wait_time -=1; 
    return wait_time <= 0; 
}

bool TimedCondition::update(double d) {
    if(expired) return true; 
    wait_time -= d;
    return wait_time <= 0;
}


bool TimedCondition::message(MMessage& m) {
    if (wait_time>0) {
        return false;
    }
    if (expired) {
        return false;
    }
    m.mtype = type;
    m.msubtype = "end";
    m.add("value", value);
    expired = true;
    return true;
}

bool TimedCondition::isEqual(TimedCondition& c) {
    if (type != c.type) return false;
    if (subtype != c.subtype) return false;
    if (value != c.value) return false;
    return true;
}

string TimedCondition::getDuration(string& value) {
    size_t pos;
    pos = value.find('_');
    if (pos!=string::npos) {
        return value.substr(0,pos);
    }
    else return value;
}

//------------------------------ Class Timer ------------------------------

Timer::Timer(string n) {
    id = n;
    condition.clear();
}

Timer::~Timer() {

}

void Timer::start_service_threads() {
    this->service_thread.push_back(new Thread(&Timer::do_work, this));
}

bool Timer::update() {
    for(size_t i=0; i<condition.size(); i++) condition[i].update();
    return true;
}

bool Timer::update(double p) {
    for(size_t i=0; i<condition.size(); i++) {
        condition[i].update(p);
    }
    return true;
}

void Timer::do_work(Timer* obj) {
    MMessage out_mess;
    MMessage msg;
    double period;
    double msg_time;
    double last_update_time = 0;
    string info;
    while(1) {
        period = 0;
        msg = obj->receive_message();
        
        if(msg.mtype == "trigger" && msg.msubtype == "poll") {
            period = msg.getDouble("period");
	    msg_time = msg.getDouble("time");
	    if(msg_time > (period/2) + last_update_time) {
               if (period >0) obj->update(period);
               else obj->update();
               for (size_t i=0; i<condition.size(); i++) {
                 if (condition[i].message(out_mess)) { 
                    SubjectSet::notify(out_mess);
		 }
               }
	       last_update_time = msg_time;
	   }
        }
        if (msg.mtype == "wait" && msg.msubtype == "set") {
            if(getDoubleDuration(msg.getString("value")) >0 )
		obj->startTimer(TimedCondition(msg.mtype, "set", msg.getString("value")));
        }
        if (msg.mtype == "wait" && msg.msubtype == "reset") {
            if(getDoubleDuration(msg.getString("value")) >0 ) {
               obj->startTimer(TimedCondition(msg.mtype, msg.msubtype, msg.getString("value")));
               obj->resetTimer(TimedCondition(msg.mtype, msg.msubtype, msg.getString("value")));
	    }
        }
	if (msg.mtype == "register" && msg.msubtype == "observation") {
	    setof<MAttribute>::iterator it;
	    for(it = this->observes.begin();it != this->observes.end();it++) {
	         SubjectSet::add_observer(it->value, it->name, dynamic_cast<Observer*>(this));
	    }
        }
    }
}

bool Timer::startTimer(TimedCondition tc) {
    bool found = false;
    for (size_t i=0; i<condition.size() && !found; i++) {
        if(condition[i].isEqual(tc)) {
		condition[i] = tc;
        	found = true;
	}
    }
    if (!found) {
        condition.push_back(tc);
    }
    return found;
}

bool Timer::resetTimer(TimedCondition tc) {
    bool found = false;
    for (size_t i=0; i<condition.size() && !found; i++) {
        if(condition[i].isEqual(tc)) condition[i].wait_time = 0;
        found = true;
    }
    return found;
}

double Timer::getDoubleDuration(string value) {
    size_t pos;
    string duration;
    pos = value.find('_');
    if (pos!=string::npos) {
        duration = value.substr(0,pos);
    }
    else duration = value;
    return hsrv::a2double(duration);
}


