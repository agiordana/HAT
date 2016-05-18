#include "TimedRuleEngine.h"

using namespace std;

TimedRuleEngine::TimedRuleEngine(string n) {
    
    id = n;
    this->timed_rule = new TimedRuleSet("timedrules");
    string cdir = hsrv::configdir +"/tr_conditions";
    
    if(FileManager::isDir(cdir)) this->condition = new BooleanConditionSet("tr_conditions");
        else this->condition = NULL;
    
    alarm_on = false;
    active.clear();
}

TimedRuleEngine::~TimedRuleEngine() {
    
}

void TimedRuleEngine::start_service_threads() {
    this->service_thread.push_back(new Thread(&TimedRuleEngine::do_work, this));
}

void TimedRuleEngine::do_work(TimedRuleEngine* obj) {
    MMessage msg;
    setof<MMessage> action_todo;
    obj->alarm_on = false;
    makeTimedRulePage(*obj->timed_rule);
    while(true) {
        action_todo.clear();
        msg = obj->receive_message();
        if (msg.mtype == "program" && msg.msubtype == "prgstatus") {
            obj->program = msg;
            obj->active = msg.getList("active");
            if(obj->program.getString("status") !="OFF"&&obj->active.size() > 0)
                obj->alarm_on = true;
            else obj->alarm_on = false;
        }
        else if(msg.mtype == "event" && condition != NULL) {
            condition->eventUpdate(msg);
        }
        else if(msg.mtype == "trigger" && msg.msubtype == "timedruleupdate") {
            obj->timed_rule->timeUpdate(obj->program.getString("status"), obj->alarm_on, condition, action_todo);
            for (unsigned i=0; i<action_todo.size(); i++) {
                SubjectSet::notify(action_todo[i]);
		hsrv::logger->info("TMUPDATE: " + action_todo[i].xmlEncode(1));
            }
        }
        else if (msg.mtype == "cmd" && msg.msubtype == "settimedrules") {
            for (unsigned i = 0; i < obj->timed_rule->size(); i++)
                if(msg.getString((*obj->timed_rule)[i].name)!="") {
                    (*obj->timed_rule)[i].set("status", msg.getString((*obj->timed_rule)[i].name));
                    (*obj->timed_rule)[i].save();
                }
            makeTimedRulePage(*obj->timed_rule);
        }
        else if (msg.mtype == "cmd" && msg.msubtype == "resettimedrules") {
            for (unsigned i = 0; i < obj->timed_rule->size(); i++) {
                (*obj->timed_rule)[i].set("status", "ON");
                (*obj->timed_rule)[i].save();
            }
        }
        else if(msg.mtype == "cmd" && msg.msubtype == "settrconditions" && condition != NULL) {
            for (unsigned i = 0; i < obj->condition->size(); i++) {
                if(msg.getString((*obj->condition)[i].name)!="") {
                    (*obj->condition)[i].set("status", msg.getString((*obj->condition)[i].name));
                    (*obj->condition)[i].save();
                }
            }
            obj->makeConditionPage(*obj->condition);
        }
        else if (msg.mtype == "cmd" && msg.msubtype == "resettrconditions" && condition != NULL) {
            for (unsigned i = 0; i < obj->condition->size(); i++) {
                (*obj->condition)[i].set("status", "ON");
                (*obj->condition)[i].save();
            }
            obj->makeConditionPage(*obj->condition);
        }
        else if (msg.mtype == "cmd" && msg.msubtype == "settrrulemode") {
             string name = msg.getString("name");
             string rstatus = msg.getString("status");
             string from = msg.getString("from");
             string to = msg.getString("to");
             if(rstatus == "ON" || rstatus == "OFF") (*obj->timed_rule).setStatus(name, rstatus);
             if(from != "" && to != "") (*obj->timed_rule).setValidityTime(name, from, to);
        }
	else if (msg.mtype == "cmd" && msg.msubtype == "settrvaliditytime") {
       		(*obj->timed_rule).setValidityTime(msg.getString("rule"), msg.getString("from"), msg.getString("to"));
    	}
	else if (msg.mtype == "cmd" && msg.msubtype == "settractions") {
		(*obj->timed_rule).setActionList(msg.getString("rule"), msg.getString("set"), msg.getString("reset"));
	}
        makeTimedRulePage(*obj->timed_rule);
    }
}

bool TimedRuleEngine::makeTimedRulePage(TimedRuleSet& tr)  {
    vector< pair<string,string> >  page;

    for(size_t i=0; i<tr.size(); i++) {
        pair<string, string> p(tr[i].name, tr[i].get("status"));
	hsrv::publish(tr[i].name,"alias=\""+tr[i].alias+"\" status=\""+tr[i].get("status")+"\" from=\""+tr[i].get("from")+"\" to=\""+tr[i].get("to")+"\"");
        page.push_back(p);
    }
    hsrv::saveItemList("timedrules", "status", page);

    return true;
}

bool TimedRuleEngine::makeConditionPage(BooleanConditionSet& cnd) {
    vector< pair<string,string> >  page;

    for(size_t i=0; i<cnd.size(); i++) {
        pair<string, string> p(cnd[i].name, cnd[i].get("status"));
        page.push_back(p);
    }
    hsrv::saveItemList("tr_conditions", "status", page);

    return true;
}

bool TimedRuleEngine::getObservations(MParams &obs) {
    ObserverThread::getObservations(obs);
    if(condition!=NULL) condition->observations(obs);
    return true;
}

bool TimedRuleEngine::getSubjects(MParams &subj) {
    ObserverThread::getSubjects(subj);
    subj.add("todo","subject", "rlcmd");
    timed_rule->subjects(subj);
    return true;
}

