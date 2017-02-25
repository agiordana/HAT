//
//  action.cpp
//  mngagent
//
//  Created by Attilio Giordana on 3/24/13.
//  Copyright (c) 2013 Università del Piemonte Orientale. All rights reserved.
//

#include "action.h"

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
   string delay = d;
   if(this->get("status") != "ON") return false;
   MMessage out_mess;
   out_mess.mtype = "cmd";
   out_mess.msubtype = this->get("cmd");
   out_mess.add("value", this->get("value"));
   SubjectSet::notify(out_mess);
   return true;
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
