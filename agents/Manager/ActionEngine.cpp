//
//  ActionEngine.cpp
//  mngagent
//
//  Created by Attilio Giordana on 3/24/13.
//  Copyright (c) 2013 Università del Piemonte Orientale. All rights reserved.
//

#include "ActionEngine.h"

using namespace std;

ActionEngine::ActionEngine(string n) {
    id = n;
    this->actions = new ActionSet("actions", "tr_actions");
}

ActionEngine::~ActionEngine() {

}

void ActionEngine::start_service_threads() {
    this->service_thread.push_back(new Thread(&ActionEngine::do_work, this));
}

void ActionEngine::do_work(ActionEngine* obj) {
    MMessage msg;
    while(true) {
        msg = obj->receive_message();
        if(msg.mtype == "cmd" && msg.msubtype == "setactions") {
            for (size_t i=0; i<(*obj->actions).size(); i++) {
                if (msg.getString((*obj->actions)[i].get("name"))=="ON"||msg.getString((*obj->actions)[i].get("name"))=="OFF") {
                    (*obj->actions)[i].setStatus(msg.getString((*obj->actions)[i].get("name")));
                }
            }
            obj->makeActionPage(*obj->actions);
            obj->makeActiveActionPage(*obj->actions, obj->program);
        }
        if(msg.mtype == "cmd" && msg.msubtype == "resetactions") {
            for (size_t i=0; i< obj->actions->size(); i++) {
                (*obj->actions)[i].setStatus("ON");
            }
            obj->makeActionPage(*obj->actions);
            obj->makeActiveActionPage(*obj->actions, obj->program);
        }
        else if(msg.mtype == "program" && msg.msubtype == "prgstatus") {
            obj->program = msg;
            obj->makeActiveActionPage(*obj->actions, obj->program);
        }
        else if(msg.mtype == "rlcmd" && actions->member(msg.getString("action"))) {
            obj->actions->exec(msg.getString("action"));
        }
    }
}

bool ActionEngine::makeActionPage(ActionSet &actions) {
    vector< pair<string,string> >  page;

    for(size_t i=0; i<actions.size(); i++) {
	pair<string, string> p(actions[i].get("name"), actions[i].get("status"));
        page.push_back(p);
    }
    hsrv::saveItemList("actions", "status", page);
    return true;
}

bool ActionEngine::makeActiveActionPage(ActionSet& al, MMessage& p) {

    vector< pair<string,string> >  page;

    vector<string> active = p.getList("active");
    vector<string> really_active;
    if (active.size()>0 && (active[0]=="ALL" || active[0] == "all" ||active[0] == "All")) {
        for (size_t i=0; i<al.size(); i++) {
            if(al[i].get("status")=="ON") really_active.push_back(al[i].get("name"));
        }
    }
    else {
        for(size_t i=0; i<active.size(); i++)
            if(al.isActive(active[i])) really_active.push_back(active[i]);
    }

    for(size_t i=0; i<really_active.size(); i++) {
	pair<string, string> p(really_active[i], "ON");
        page.push_back(p);
    }
    hsrv::saveItemList("activeactions", "status", page);

    return true;
}

bool ActionEngine::getSubjects(MParams& subj) {
   ObserverThread::getSubjects(subj);
   actions->subjects(subj);
   return true;
}
