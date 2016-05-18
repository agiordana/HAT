#include "RuleEngine.h"

using namespace std;

RuleEngine::RuleEngine(string n) {
    id = n;
    MParams params("threads/"+n);
    MParams timeparams("params/wait_times.xml");
    params.load();
    timeparams.load();
    
    this->event_counter = new EventCounterSet("eventcounters");
    
    string cdir = hsrv::configdir +"/conditions";
    if(FileManager::isDir(cdir)) this->condition = new BooleanConditionSet("conditions");
    else condition = NULL;
    this->rule = new RuleSet("rules");
    
    alarm_on = false;
    wait_after_on = timeparams.get("wait_after_on");
    if(wait_after_on=="") wait_after_on = params.get("wait_after_on");
    wait_forbidden = timeparams.get("wait_forbidden");
    if(wait_forbidden=="") wait_forbidden = params.get("wait_forbidden");
}

RuleEngine::~RuleEngine() {
    
}

void RuleEngine::start_service_threads() {
    this->service_thread.push_back(new Thread(&RuleEngine::do_work, this));
}

void RuleEngine::do_work(RuleEngine* obj) {
    MMessage msg;
    setof<MMessage> action_todo;
    bool res;
    obj->makeEventcounterPage(*obj->event_counter);
    obj->makeRulePage(*obj->rule);
    obj->makeParameterPage(obj->wait_after_on, obj->wait_forbidden);
    
    while(true) {
        action_todo.clear();
        msg = obj->receive_message();
        
        if(msg.mtype == "trigger" && msg.msubtype == "eventupdate") {
            if(obj->event_counter->timeUpdate() ||obj->rule->timeUpdate()){
                obj->rule->match(event_counter, condition, action_todo, active, obj->alarm_on);
                for (size_t i=0; i<action_todo.size(); i++) {
                    SubjectSet::notify(action_todo[i]);
                }
            }
     }
     else if(msg.mtype == "event" && (msg.msubtype == "_SWITCH" || msg.msubtype == "_ON")) {
	msg.mtype = "cmd";
        msg.add("program", msg.msubtype);
	msg.msubtype="setprogram";
        event_counter->clearDQ();
	SubjectSet::notify(msg);
     }
     else if(msg.mtype == "event") {
        obj->event_counter->timeUpdate();
        if(condition !=NULL) res = obj->condition->eventUpdate(msg);
        res = obj->event_counter->eventUpdate(msg);
        obj->rule->match(event_counter, condition, action_todo, active, obj->alarm_on);
        for (unsigned i=0; i<action_todo.size(); i++) {
            SubjectSet::notify(action_todo[i]);
        }
    }
    else if(msg.mtype == "program" && msg.msubtype == "prgstatus") {
        obj->program = msg;
        if(obj->program.getString("status") !="OFF")
            obj->alarm_on = true;
        else obj->alarm_on = false;
        obj->active = msg.getList("active");
        obj->rule->setWait(wait_after_on);
        event_counter->clearDQ();
    }
    else if(msg.mtype == "cmd" && msg.msubtype == "setwait") {
        obj->wait_after_on = msg.getString("wait");
        obj->makeParameterPage(obj->wait_after_on, obj->wait_forbidden);
    }
    else if(msg.mtype == "cmd" && msg.msubtype == "waitfor") {
        if (msg.getString("wait")!="") obj->rule->setWait(msg.getString("wait"));
        else obj->rule->setWait(obj->wait_after_on, obj->wait_forbidden);
    }
    else if(msg.mtype == "cmd" && msg.msubtype == "seteventcounters") {
        for (unsigned i = 0; i < obj->event_counter->size(); i++)
            if(msg.getString((*obj->event_counter)[i].name)!="") {
                (*obj->event_counter)[i].set("status", msg.getString((*event_counter)[i].name));
                (*obj->event_counter)[i].save();
            }
        obj->makeEventcounterPage(*obj->event_counter);
    }
    else if (msg.mtype == "cmd" && msg.msubtype == "reseteventcounters") {
        for (unsigned i = 0; i < obj->event_counter->size(); i++) {
            (*obj->event_counter)[i].set("status", "ON");
            (*obj->event_counter)[i].save();
        }
        obj->makeEventcounterPage(*obj->event_counter);
    }
    else if(msg.mtype == "cmd" && msg.msubtype == "setconditions" && condition!=NULL) {
        for (unsigned i = 0; i < obj->condition->size(); i++) {
            if(msg.getString((*obj->condition)[i].name)!="") {
                (*obj->condition)[i].set("status", msg.getString((*obj->condition)[i].name));
                (*obj->condition)[i].save();
            }
        }
        obj->makeConditionPage(*obj->condition);
    }
    else if (msg.mtype == "cmd" && msg.msubtype == "resetconditions" && condition!=NULL) {
        for (unsigned i = 0; i < obj->condition->size(); i++) {
            (*obj->condition)[i].set("status", "ON");
            (*obj->condition)[i].save();
        }
        obj->makeConditionPage(*obj->condition);
    }
    else if (msg.mtype == "cmd" && msg.msubtype == "setrulemode") {
	string name = msg.getString("name");
        string rstatus = msg.getString("status");
        string from = msg.getString("from");
        string to = msg.getString("to");
        if(rstatus == "ON" || rstatus == "OFF") (*obj->rule).setStatus(name, rstatus);
        if(from != "" && to != "") (*obj->rule).setValidityTime(name, from, to);
	obj->makeRulePage(*obj->rule);
    }
    else if (msg.mtype == "cmd" && msg.msubtype == "setrules") {
        for (unsigned i = 0; i < rule->size(); i++)
            if(msg.getString((*rule)[i].name)!="") {
                (*obj->rule)[i].set("status", msg.getString((*obj->rule)[i].name));
                (*obj->rule)[i].save();
            }
        obj->makeRulePage(*obj->rule);
    }
    else if (msg.mtype == "cmd" && msg.msubtype == "setvaliditytime") {
	(*obj->rule).setValidityTime(msg.getString("rule"), msg.getString("from"), msg.getString("to"));
    }
    else if (msg.mtype == "cmd" && msg.msubtype == "resetrules") {
        for (unsigned i = 0; i < rule->size(); i++) {
            (*obj->rule)[i].set("status", "ON");
            (*obj->rule)[i].save();
        }
        obj->makeRulePage(*obj->rule);
    }
  }
}


bool RuleEngine::makeRulePage(RuleSet& r) {
    vector< pair<string,string> >  page;
    string dynamic_desc;

    for(size_t i=0; i<r.size(); i++) {
        pair<string, string> p(r[i].name, r[i].get("status"));
        page.push_back(p);
// publish on DynamicPage.....	
        hsrv::publish(r[i].name,"alias=\""+r[i].alias+"\" status=\""+r[i].get("status")+"\" from=\""+r[i].get("from")+"\" to=\""+r[i].get("to")+"\"");
    }
    hsrv::saveItemList("rules", "status", page);
    return true;
}

bool RuleEngine::makeEventcounterPage(EventCounterSet& ec) {
    vector< pair<string,string> >  page;

    for(size_t i=0; i<ec.size(); i++) {
        pair<string, string> p(ec[i].name, ec[i].get("status"));
        page.push_back(p);
    }
    hsrv::saveItemList("eventcounters", "status", page);
    return true;
}

bool RuleEngine::makeConditionPage(BooleanConditionSet& cnd) {
    vector< pair<string,string> >  page;

    for(size_t i=0; i<cnd.size(); i++) {
        pair<string, string> p(cnd[i].name, cnd[i].get("status"));
        page.push_back(p);
    }
    hsrv::saveItemList("conditions", "status", page);
    return true;
}


bool RuleEngine::makeParameterPage(string wf, string wt) {
    vector< pair<string,string> >  page;

    pair<string, string> p("wait_after_on", wt);
    page.push_back(p);
    pair<string, string> p1("wait_forbidden", wf);
    page.push_back(p);
    hsrv::saveItemList("parameters", "status", page);
    return true;
}

bool RuleEngine::getObservations(MParams &obs) {
    ObserverThread::getObservations(obs);
    event_counter->observations(obs);
    if(condition != NULL) condition->observations(obs);
    return true;
}

bool RuleEngine::getSubjects(MParams &subj) {
    ObserverThread::getSubjects(subj);
    subj.add("todo","subject","rlcmd");
    rule->subjects(subj);
    return true;
}
