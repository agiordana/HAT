//
//  DummySource.cpp
//  soapagent
//
//  Created by Attilio Giordana on 2/4/13.
//  Copyright (c) 2013 Università del Piemonte Orientale. All rights reserved.
//

using namespace std;

#include "DummySource.h"

DummySource::DummySource(string n) {
    id = n;
    i = 0;
}

DummySource::~DummySource() {

}

void DummySource::start_service_threads() {
    this->service_thread.push_back(new Thread(&DummySource::do_work, this));
}

void DummySource::do_work(DummySource* obj) {
    MMessage out_mess;
    MMessage msg;
    while(1) {
        msg = obj->receive_message();
        if(msg.mtype == "trigger" && msg.msubtype == "poll") {
          if(subjects.size()>0 ) {
            out_mess.mtype = obj->subjects[i].value;
            out_mess.msubtype = obj->subjects[i].name;
            out_mess.add("A1", obj->subjects[i].name);
            out_mess.add("A2", "TEST1");
            i++;
            if(i>=subjects.size()) i=0;
            SubjectSet::notify(out_mess);
          }
        }
    
    }
}

/*

bool DummySource::getObservations(MParams &obs) {
    string pathname="threads/"+get_name();
    MParams param(pathname);
    param.load();
    for (size_t i=0; i<param.size(); i++) {
        if (param[i].category=="observe") {
            obs.add(param[i].name, param[i].category, param[i].value);
        }
    }
    return true;
}

bool DummySource::getSubjects(MParams &subj) {
    string pathname="threads/"+get_name();
    MParams param(pathname);
    param.load();
    for (size_t i=0; i<param.size(); i++) {
        if (param[i].category=="subject") {
            subj.add(param[i].name, param[i].category, param[i].value);
        }
    }
    return true;
}
*/
