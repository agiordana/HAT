//
//  action.cpp
//  mngagent
//
//  Created by Attilio Giordana on 3/24/13.
//  Copyright (c) 2013 Università del Piemonte Orientale. All rights reserved.
//

#include "action.h"

#define EXTRAWAIT 30

using namespace std;

Action::Action(string dir, string nm): MParams::MParams(dir, nm) {
   load();
   if(get("name") == "") name = nm;
        else name = get("name");
}

bool Action::setStatus(string value) {
    this->set("status", value);
    this->save();
    return true;
}

bool Action::exec(string d) {
   delay = d;
   if(this->get("status") != "ON") return false;
   double tt = hsrv::gettime();
   double deadline = hsrv::gettime()+hsrv::a2double(d)/10 + EXTRAWAIT;
//cout<<name<<" "<<hsrv::double2a(deadline)<<deadline<<" "<<hsrv::double2a(tt)<<endl;
   ths.push_back(ThreadEntry(deadline,new boost::thread(boost::bind(&Action::do_work, this, this))));
   return true;
}

void Action::do_work(Action* obj) {
    struct timespec rqtp, rmtp;
    MMessage out_mess;
    out_mess.mtype = "cmd";
    out_mess.msubtype = obj->get("cmd");
    out_mess.add("value", obj->get("value"));
    if(obj->delay!="") {
       //wait for delay before executing the command
       double waittime = hsrv::a2double(obj->delay);
       waittime /= 10;
       rqtp.tv_sec = (int) waittime;
       rqtp.tv_nsec = (int)(NANOPERSECOND*(waittime - rqtp.tv_sec));
       int res = nanosleep(&rqtp, &rmtp);
    }
    SubjectSet::notify(out_mess);
}

bool Action::subjects(MParams& subj) {
    subj.add(this->get("cmd"), "subject", "cmd");
    return true;
}

bool Action::observations(MParams& obs) {
    obs.add(this->get("name"), "observe", "rlcmd");
    return true;
}

int Action::member(string n) {
    if(this->get("name")==n) return 1;
    return 0;
}

bool Action::purge_expired(double tt) {
   for(size_t i=0; i<ths.size(); i++) {
      if(ths[i].deadline < tt) {
//cout<<"Purge: "<<name<<": "<<i<<endl;
         ths[i].th->join();
	 ths.erase(ths.begin()+i);
	 return true;
      }
   }
   return false;
} 
 
