/*
 *  rule.cpp
 *  mngagent
 *
 *  Created by Attilio Giordana on 1/13/12.
 *  Copyright 2012 Università del Piemonte Orientale. All rights reserved.
 *
 */


#include "rule.h"

using namespace std;

Rule::Rule(string dir, string nm): MParams::MParams(dir, nm) {
   string tmp;
   load();
   if(get("name") == "") name = nm;
	else name = get("name");
   latentUntil = 0;
   from = -1;
   to = -1;
   midnight = -1;
   nextcyclestart = -1;
   reftime = (long)hsrv::gettime();
   setValidityTime();
   if ((tmp=get("cycle"))=="INF"||tmp == "") cycle = -1;
     else cycle = hsrv::a2int(tmp);
   if(get("status") == "ON") status = ON;
     else status = OFF;
   behavior = get("mode");
   if(behavior == "") behavior = "always";
   prcondition = false;
   tcondition = false;
   tcondition = false;
   prcondition_time = 0;
   fcondition_time = 0;
   tcondition_time = 0;
   alias = get("alias");
   if(alias=="") alias = get("action")+"_"+name;
}

bool Rule::cycleUpdate() {
  reftime = (long)hsrv::gettime();
  if (cycle <= 0 ) {
     cycleactive = false;
     return cycleactive;
  }
  else {
    if(nextcyclestart < 0) {
       string asc_time = hsrv::getasciitimeday();
       long now = (long)hsrv::string2second(asc_time);
       nextcyclestart = reftime - now;
       while (nextcyclestart < reftime) {
         nextcyclestart += cycle;
       }
       cycleactive = false;
       return cycleactive;
    }
    else if(reftime >= nextcyclestart) {
       nextcyclestart += cycle;
       cycleactive = true;
       return cycleactive;
    }
  }
  return cycleactive;
}



bool Rule::init() {
   condition.init(get("clause"), '+');
   enabling_true_condition.init(get("enabling_true_condition"), '+');
   enabling_false_condition.init(get("enabling_false_condition"), '+');
   action.init(get("action"), '+');
   string tmp = get("min_enabling_true_conditions");
   if(tmp == "") mine_t_cond = enabling_true_condition.size();
   else mine_t_cond = hsrv::a2int(tmp);
   tmp = get("min_enabling_false_conditions");
   if(tmp == "") mine_f_cond = enabling_false_condition.size();
   else mine_t_cond = hsrv::a2int(tmp);
   tmp = get("min_conditions");
   if(tmp == ""||condition.size()==0) mincond = condition.size();
   else mincond = hsrv::a2int(tmp);

   return true;
};


bool Rule::match(vector<string>& active, bool prgstatus, EventCounterSet* ec, BooleanConditionSet* cnd, setof<MMessage>& answer) {
    string source = "";
    string video = "";
    string comment = "";
    string latency = "";
    unsigned i, j;
    int k;
    int m = 0;
    int n = 0;
    double current_time=hsrv::gettime();
      
    if (status != ON) {
    	return false;
    }
    if(behavior == "programon" && !isRuleActive(active)) {
       if(!prgstatus) prcondition = false;
       return false;
    }
    if(behavior == "programon" && !prgstatus) {
       prcondition = false;
       return false;
    }
    if(behavior == "programoff" && prgstatus) {
       prcondition = false;
       return false;
    }
    if(!prcondition) {
       prcondition = true;
       prcondition_time = current_time;
    }
    if(cnd!=NULL&&enabling_true_condition.size()>0) {
       for(i=0; i<cnd->size(); i++) {
          if((*cnd)[i].isActive() && (k = enabling_true_condition.find((*cnd)[i].get("name"))) >=0) {
	      m++;
          }
       }
       if(m<mine_t_cond) {
	  tcondition = false;
	  return false;
       }
    }
    if(!tcondition) {
       tcondition = true;
       tcondition_time = current_time;
    }
    m = 0;
    if(cnd!=NULL&&enabling_false_condition.size()>0) {
        for(i=0; i<cnd->size(); i++) {
            if(!(*cnd)[i].isActive() && (k = enabling_false_condition.find((*cnd)[i].get("name"))) >=0) {
		  m++;
            }
        }
       if(m<mine_f_cond) {
          fcondition = false;
          return false;
       }
    }
    if(!fcondition) {
       fcondition = true;
       fcondition_time = current_time;
    }

    for(i=0; i<ec->size(); i++) {
    	if((*ec)[i].isActive() && (k = condition.find((*ec)[i].get("name"))) >=0) {
            n++;
            if(source!="") source += "+";
	    if(comment!="") comment += ";";
	    source += (*ec)[i].getHistory();
	    video += (*ec)[i].getVideo();
	    comment += (*ec)[i].getComment();
	}
     }
     if(n >= mincond) {
	evcondition = true;
        evcondition_time = current_time;
     }
     else evcondition = false;
    if(!evcondition && !cycleactive) return false;
    if(mincond>0 && current_time <latentUntil) return false;
    if(mincond==0 && !cycleactive && prcondition_time < current_time
	 && tcondition_time < current_time
	 && fcondition_time < current_time) {
       return false;
    }
    if(mincond >0 && cycle <= 0) {
       latency = get("latency");
       if(latency!="") latentUntil = current_time + hsrv::a2int(latency);
    }
    i = 0;
    cycleactive = false;
    answer.push_back(MMessage("alarm", get("alarm")));
    answer[i].add("alarm", get("alarm"));
    answer[i].add("source", source);
    if(video != "") answer[i].add("video", video);
    answer[i].add("comment", comment);
    i++;
// add enabled actions 
    for(j=0; j<action.size(); j++) {
	if(!isNotMember(action[j], active)) {
	   answer.push_back(MMessage("rlcmd","todo"));
	   answer[i].add("action", action[j]);
	   answer[i].add("value", "ON");
	   answer[i].add("source", source);
	   answer[i].add("comment", comment);
	   i++;
	}
    }
    
    return true;
}

bool Rule::subjects(MParams &subj) {
    subj.add(this->get("alarm"), "subject", "alarm");
//    NameList tactions;
//    tactions.init(this->get("action"),'+');
//    for(size_t i=0; i<tactions.size(); i++)
//        subj.add(tactions[i], "subject", "rlcmd");
    return true;
}

bool Rule::setStatus(string rstatus) {
   if(rstatus == "ON") add("status", rstatus);
      else add("status", "OFF");
   save();
   return true;
}

bool Rule::setValidityTime(string asfrom, string asto) {
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
	return true;
   }
   else return false;
}

bool Rule::setValidityTime() {
  string asfrom = get("from");
  string asto = get("to");
  if(asfrom == "" || asfrom == "-INF") asfrom = "00:00:00";
  if(asto == "" || asto == "INF") asto = "24:00:00";
  return setValidityTime(asfrom, asto);
}


bool Rule::isActive() {

  int c_time;
  string asc_time;
  bool res = false;

  if(to < 0 || from <0) setValidityTime();
  if(from < 0 || to <0) return true;
  if(from == 0 && to == midnight) return true;
  asc_time = hsrv::getasciitimeday();
  c_time = hsrv::string2second(asc_time);
  if(from < to) {
	res = (c_time >= from && c_time <= to);
  }
  else {
	res = ((c_time >= from && c_time <= midnight) || (c_time >= 0 && c_time <= to));
  }
  return res;
}

bool Rule::isRuleActive(vector<string>& active) {
  for(size_t i=0; i<active.size(); i++)
    if(active[i]=="ALL" || active[i]=="All" || active[i]=="all") return true;
  for(size_t i = 0; i<condition.size(); i++) 
    if(isNotMember(condition[i], active)) {
	return false;
    }
  if(isNotMember(get("alarm"), active)) return false;
  return true;
}


bool Rule::isNotMember(string s, vector<string>& active) {
// when mode is always or programoff is always active
  if(behavior == "always" || behavior == "programoff") return false;
    for(size_t i=0; i<active.size(); i++) {
        if (s == active[i]||active[i]=="ALL"||active[i]=="All" || active[i]=="all") {
            return false;
        }
    }
    return true;
}


