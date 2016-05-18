//
//  timedrule.cpp
//  mngagent
//
//  Created by Attilio Giordana on 3/28/12.
//  Copyright (c) 2012 Università del Piemonte Orientale. All rights reserved.
//

#include "timedrule.h"

using namespace std;


TimedRule::TimedRule(string dir, string nm): MParams::MParams(dir, nm) {
    load();
    string tmp;
    name = get("name");
    if(name == "") name = nm;
    alias = get("alias");
    if(alias == "") alias = get("set") + "_" + get("reset");
    behavior = get("mode");
    setValidityTime();
    if ((tmp=get("cycle"))=="INF"||tmp == "") cycle = -1;
        else cycle = hsrv::a2int(tmp);
    if(get("status")=="ON") status= ON;
        else status = OFF;
    string asc_time = hsrv::getasciitimeday();
    c_time = hsrv::string2second(asc_time);
    reftime = (long)hsrv::gettime();
    active = OFF;
    nextcyclestart = -1;
    exec = false;
    init();
}

bool TimedRule::timeUpdate(string program_name, int program, BooleanConditionSet* cnd, setof<MMessage>& res) {
    string asc_time = hsrv::getasciitimeday();
    reftime = hsrv::string2second(asc_time);
    c_time = hsrv::string2second(asc_time);
    exec = false;
    cycleUpdate();
    if(get("status")=="ON") status= ON;
        else status = OFF;
    if (status==OFF) {
        if (active) {
            makeAction(OFF, res);
            active = false;
        }
        return false;
    }
    if(behavior=="timed") {
        if(isInRange(c_time, from, to)&&checkCondition(cnd)) {
            if(exec||!active) {
                makeAction(ON, res);
                active = ON;
            }
        }
        else {
            if(active) {
                makeAction(OFF, res);
                active = OFF;
            }
        }
        return true;
    }
    if (behavior=="program_timed") {
        if(checkProgramName(program_name) && program&&isInRange(c_time, from, to)&&checkCondition(cnd)) {
            if(exec||!active) {
                makeAction(ON, res);
                active = ON;
            }
        }
        else {
            if(active) {
                makeAction(OFF, res);
                active = OFF;
            }
        }
        return true;
    }
    if (behavior=="cyclic"&&checkCondition(cnd)) {
        if(exec && isInRange(c_time, from, to)) {
            makeAction(ON, res);
            return true;
        }
    }
    if (behavior=="program_cyclic" && checkCondition(cnd)) {
        if(checkProgramName(program_name) && exec && program&&isInRange(c_time, from, to)) {
            makeAction(ON, res);
            return true;
        }
    }
    return false;
}

bool TimedRule::cycleUpdate() {
    reftime = (long)hsrv::gettime();
    if ((behavior == "timed"||behavior == "program_timed") && cycle > 0) {
        if(nextcyclestart < 0) {
            nextcyclestart = reftime + cycle;
            exec = true;
            return exec;
        }
        else if(reftime > nextcyclestart) {
            nextcyclestart += cycle;
            exec = true;
            return exec;
        }
    }
    if ((behavior == "cyclic"||behavior == "program_cyclic" ) && cycle > 0 ) {
        if(nextcyclestart < 0) {
            string asc_time = hsrv::getasciitimeday();
            long now = (long)hsrv::string2second(asc_time);
            nextcyclestart = reftime - now;
            while (nextcyclestart < reftime) {
                nextcyclestart += cycle;
            }
            exec = false;
            return exec;
        }
        else if(reftime > nextcyclestart) {
            nextcyclestart += cycle;
            exec = true;
            return exec;
        }
    }
}

bool TimedRule::makeAction(int mode,  setof<MMessage>& res) {
    size_t i;
    NameList actions;
    if(mode == ON) actions.init(get("set"), '+');
    else actions.init(get("reset"), '+');
    for(size_t j=0; j<actions.size(); j++) 
	if(actions[j]!="none" && actions[j]!="NONE" && actions[j] != "None") {
        	res.push_back(MMessage("rlcmd","todo"));
        	i = res.size()-1;
		res[i].add("action", actions[j]);
        	res[i].add("source", "timer");
        	res[i].add("value", "ON");
        	res[i].add("comment", get("comment"));
        }
    return true;
}

bool TimedRule::subjects(MParams& subj) {
    NameList tactions;
//    tactions.init(this->get("set"), '+');
//    for(size_t i=0; i<tactions.size(); i++)
//        subj.add(tactions[i], "subject", "rlcmd");
//    tactions.init(this->get("reset"), '+');
//    for(size_t i=0; i<tactions.size(); i++)
//        subj.add(tactions[i], "subject", "rlcmd");
    return true;
}

bool TimedRule::checkCondition(BooleanConditionSet *cnd) {
    vector<bool> cflag;
    unsigned i;
    int k;
    if (cnd==NULL) {
        return true;
    }
    for (i=0; i<enabling_condition.size(); i++) {
		cflag.push_back(bool(false));
	}
    for(i=0; i<cnd->size(); i++) {
        if((*cnd)[i].isActive() && (k = enabling_condition.find((*cnd)[i].get("name"))) >=0)
            cflag[(unsigned)k] = true;
    }
    for (i=0; i<cflag.size(); i++) {
		if (!cflag[i]) {
			return false;
		}
	}
    return true;
}

bool TimedRule::setStatus(string rstatus) {
   if(rstatus == "ON") add("status", rstatus);
      else add("status", "OFF");
   save();
   return true;
}

bool TimedRule::setValidityTime(string asfrom, string asto) {
   int tfrom = hsrv::string2second(asfrom);
   int tto = hsrv::string2second(asto);
   string mdn = "24:00:00";
   midnight = hsrv::string2second(mdn);
   if(tfrom>=0&& tfrom<midnight && tto>0 && tto<=midnight) {
        from = tfrom;
        add("from", asfrom);
        to = tto;
        add("to", asto);
        save();
	string info=get("name") + " VTIME: "+hsrv::int2a(from)+ " " +hsrv::int2a(to);
	hsrv::logger->info(info);
        return true;
   }
   else return false;
}

bool TimedRule::setValidityTime() {
  string asfrom = get("from");
  string asto = get("to");
  if(asfrom == "" || asfrom == "-INF") asfrom = "00:00:00";
  if(asto == "" || asto == "INF") asto = "24:00:00";
  return setValidityTime(asfrom, asto);
}

bool TimedRule::setActionList(string setlist, string resetlist) {
  if(setlist != "") add("set", setlist);
  if(resetlist != "") add("reset", resetlist);
  save();
  return true;
}

bool TimedRule::checkProgramName(string program_name) {
   string program = get("active_for_program");
   if(program == "") return true;
   if(program == "ALL") return true;
   if(program == program_name) return true;
   return false;
}
