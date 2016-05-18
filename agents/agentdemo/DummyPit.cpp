//
//  DummySource.cpp
//  soapagent
//
//  Created by Attilio Giordana on 2/4/13.
//  Copyright (c) 2013 Università del Piemonte Orientale. All rights reserved.
//

using namespace std;

#include "DummyPit.h"
#include "NameService.h"

DummyPit::DummyPit(string n) {
    id = n;
}

DummyPit::~DummyPit() {

}

void DummyPit::start_service_threads() {
    this->service_thread.push_back(new Thread(&DummyPit::do_work, this));
}

void DummyPit::do_work(DummyPit* obj) {
    MMessage out_mess;
    MMessage::iterator it;
    MMessage msg;
    string current_time;
    string fname;
    while(1) {
        msg = obj->receive_message();
        if(hsrv::behaviour=="logger") {
	    current_time = hsrv::getasciitimecompact();
	    cout<<current_time<<" - ";
	    for(it=msg.begin(); it!=msg.end(); it++) 
		if(it->first != "time" && it->first != "agent" && it->first!="language" &&it->first != "url" && it->first != "method") {
			fname = it->first;
		    cout<<fname <<" = "<<msg.getString(fname)<<"; ";
	        }
	    cout<<" time = " << msg.getString("time") <<endl;
	}
	else cout <<"DummyPIT-----: "<< msg.xmlEncode(0)<<endl;
        vector<string> names = NameService::getNames("devices","SE");
        for(size_t i=0; i<names.size(); i++) cout<<names[i]<<endl;
    }
}

/*

bool DummyPit::getObservations(MParams &obs) {
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

bool DummyPit::getSubjects(MParams &subj) {
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
