/*
 *  eventcounter.cpp
 *  mngagent
 *
 *  Created by Attilio Giordana on 1/13/12.
 *  Copyright 2012 Università del Piemonte Orientale. All rights reserved.
 *
 */


#include "eventcounter.h"
using namespace std;
#define MAXDELAYQSZ 100

//------------ Class EventList -------------------------

int EvList::update(double time) {
	int count = 0;
	vector<Event>::iterator it;
    
	for(it=begin(); it<end(); it++) {
        
		if(it->time <time) {
			erase(it);
			count++;
		}
        
    }
    
	return count;
}

string EvList::history() {
	string tmp = "";
	vector<Event>::iterator it;
    
	for(it=begin(); it<end(); it++) {
		if(tmp!="") tmp+="+";
		tmp+=it->source;
	}
    
	return tmp;
}

string EvList::video() {
	string tmp = "";
	vector<Event>::iterator it;
    
	for(it=begin(); it<end(); it++) {
		if(tmp!="") tmp+="+";
		tmp+=it->video;
	}
    
	return tmp;
}

//------------ Class DelayQueue ---------------------------

bool DelayQueue::deadlineUpdate(){
    double t;
    next_message = 0;
    next_deadline = INFINITY;
    for(unsigned i=0; i<this->size(); i++) {
        t = (*this)[i].getDouble("time");
        if(t < next_deadline) {
            next_message = i;
            next_deadline = t;
        }               
    }           
    return true;
}

bool DelayQueue::messageGet(MMessage &m) {
    if(size()==0) return false;

    if(hsrv::gettime()>= next_deadline) {
        m = (*this)[next_message];
        m.add("time", hsrv::gettime());
        this->erase(this->begin()+next_message);
        deadlineUpdate();
        return true;
    }
    return false;
}

bool DelayQueue::messageAppend(MMessage &m){
    if(this->size() > MAXDELAYQSZ) return false;

    double waituntil = hsrv::gettime() + delay;
    string tm = hsrv::double2a(waituntil);
    m.add("time", tm);
    this->push_back(m);
    deadlineUpdate();
    return true;
}

bool DelayQueue::print() {
    printf("%ld %ld %u\n", (long)hsrv::gettime(), (long)next_deadline, (unsigned)size());
    return true;
}


//------------ Class EventCounter -------------------------

EventCounter::EventCounter(string dir, string nm): MParams::MParams(dir, nm) {
   counter = 0;
   load();
   if(get("name") == "") name = nm;
        else name = get("name");
   string tmp = get("threshold");
   threshold = hsrv::a2double(tmp);
	
    tmp = get("increment");
	dIncrement = hsrv::a2double(tmp);
	
    tmp = get("decrement");
	dDecrement = hsrv::a2double(tmp);
	
    alias.init(get("alias"));
    values.init(get("accepted_values"));
	last_time = current_time = hsrv::gettime();
    if(get("delay") != "")
        dq = new DelayQueue(get("delay"));
    else dq = NULL;
}

double EventCounter::eventUpdate(MMessage& in) {
    MMessage m;
    if(!match(in)) return 0;
    if(dq != NULL) {
        dq->messageAppend(in);
        if(dq->messageGet(m)==false) return 0;
    }
    else m = in;
    return Update(m);
}

double EventCounter::Update(MMessage& m) {
    string name = m.msubtype;
    string source = m.getString("source");
    string c = m.getString("comment");
    string video = m.getString("video");
    if(source=="") source=name;
    counter += dIncrement;
    if(video!="") history.push_back(Event(source, video, hsrv::gettime()));
      else history.push_back(Event(source, hsrv::gettime()));
    comment = c;
	return counter;
}


double EventCounter::timeUpdate() {
    MMessage m;
    if(dq != NULL && dq->messageGet(m)) {
        Update(m);
        last_time = hsrv::gettime();
    }
    
	current_time = hsrv::gettime();

    counter -= (current_time - last_time) * dDecrement;

    if(counter < 0) counter = 0;
	
    last_time = current_time;
	
    string memory = get("memory");
	if (memory == "") memory = "10";
	
    history.update(current_time - hsrv::a2double(memory));
    
    return counter;
}

bool EventCounter::observations(MParams &obs) {
    string nn = this->get("name");
    obs.add(nn,"observe","event");
    for(size_t i=0; i<alias.size(); i++) 
        if(nn != alias[i]) obs.add(alias[i], "observe", "event");
    return true;
}

bool EventCounter::match(MMessage& m) {
    string name = m.msubtype;
    string source = m.getString("source");
    string value = m.getString("value");
    if(!values.member(value)) return false;
    return (name==ty || alias.member(name) || m.getString(source) == ty || alias.member(source));
}

bool EventCounter::clearDQ() {
    if(dq!=NULL) dq->clear();
    return true;
}

