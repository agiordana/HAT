//
//  DummySource.cpp
//  soapagent
//
//  Created by Attilio Giordana on 2/4/13.
//  Copyright (c) 2013 Università del Piemonte Orientale. All rights reserved.
//

using namespace std;

#include "Receiver.h"

Receiver::Receiver(string n) {
    id = n;
}

Receiver::~Receiver() {

}


void Receiver::start_service_threads() {
    this->service_thread.push_back(new Thread(&Receiver::do_work, this));
}

void Receiver::do_work(Receiver* obj) {
    MMessage out_mess;
    MMessage::iterator it;
    MMessage msg;
    string current_time;
    string fname;
    string mtime; 
    while(1) {
        msg = obj->receive_message();
        out_mess.clear();
        out_mess = msg;
        out_mess.mtype = "logcmd";
        out_mess.msubtype = "archive";
        current_time = hsrv::getasciitimecompact();
        mtime = msg.getString("time");
        out_mess.add("class", msg.mtype);
        out_mess.add("feature", msg.msubtype);
	if(msg.getString("agent") != "") out_mess.add("agentname", msg.getString("agent"));
	if(msg.getString("Ipower") != "") out_mess.add("value", msg.getString("Ipower"));
        if(msg.getString("timeofday")=="") out_mess.add("timeofday", hsrv::getasciitimecompact());
        out_mess.add("time", mtime);
        SubjectSet::notify(out_mess);
    }
}

