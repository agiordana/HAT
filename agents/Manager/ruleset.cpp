/*
 *  ruleset.cpp
 *  mngagent
 *
 *  Created by Attilio Giordana on 1/13/12.
 *  Copyright 2012 Università del Piemonte Orientale. All rights reserved.
 *
 */

#include "ruleset.h"

using namespace std;

RuleSet::RuleSet(string dir) {
	vector<string> filelist;
    string config_dir = hsrv::configdir+"/"+dir;
	
    FileManager::fileList(config_dir, filelist);
	
    for(unsigned i = 0; i < filelist.size(); i++) {
        // recupera il nome della regola dal nome del file XML (eliminando l'estensione)
        push_back(Rule(dir, FileManager::getStem(filelist[i])));
    }
	// carica i parametri dai file XML per tutte le istanze Rule aggiunte
    for(unsigned i = 0; i < filelist.size(); i++) {
        (*this)[i].load();
		(*this)[i].init();
    }
	wait_until = hsrv::gettime();
}

int RuleSet::match(EventCounterSet* ec, BooleanConditionSet* cnd, setof<MMessage>& action, vector<string>& active, bool program_status) {
    int counter = 0;
    
    for (unsigned i = 0; i < size(); i++) {
	 if((*this)[i].match(active, program_status, ec, cnd, action)) counter++;
    }
    return counter;
}

bool RuleSet::isON(Rule& r, vector<string>& active, bool program_status) {
	if(!r.isActive()) {
		return false;
	}
    
	if(r.get("status") != "ON") return false;
	if(r.get("mode") == "always") return true;
    if(r.get("mode") == "program") return program_status;
	if(!program_status ||(r.get("mode") == "program_wait" && hsrv::gettime()<wait_until)) return false;
	if(active.size()==0) return false;
	string alarm = r.get("alarm");
	for(unsigned i=0; i<active.size(); i++)
		if(active[i]=="ALL" || active[i]=="All" || active[i]=="all") return true;
    if (isNotMember(alarm, active)) {
        return false;
    }
	return isAntecedentOn(r, active);
}

bool RuleSet::isAntecedentOn(Rule& r, std::vector<std::string> & active) {
    NameList antecedent;
    antecedent.init(r.get("clause"),'+');
    for (size_t i=0; i<antecedent.size(); i++) {
        if (isNotMember(antecedent[i], active)) {
            return false;
        }
    }
    return true;
}

bool RuleSet::isNotMember(string& s, std::vector<std::string> & active) {
    for(size_t i=0; i<active.size(); i++)
        if (s == active[i]) {
            return false;
        }
    return true;
}



double RuleSet::setWait() {
    wait_until = hsrv::gettime();
    wforbid = wait_until;
    return wait_until;
}

double RuleSet::setWait(string wt) {
    wait_until = hsrv::a2double(wt) + hsrv::gettime();
    wforbid = wait_until;
    return wait_until;
}

double RuleSet::setWait(string wt, string wf) {
    if(hsrv::gettime()<wforbid) return 0;
    wait_until = hsrv::a2double(wt) + hsrv::gettime();
    wforbid = wait_until;
    if(wf != "" && wf != "-1") wforbid+=hsrv::a2double(wf);
    else wforbid+=INFINITY;
    return wait_until;
}
bool RuleSet::subjects(MParams &subj) {
    for(size_t i=0; i<this->size(); i++) (*this)[i].subjects(subj);
    return true;
}

bool RuleSet::setStatus(string name, string rstatus) {
   for(unsigned i = 0; i<size(); i++) 
	if((*this)[i].name == name||(*this)[i].alias == name) return (*this)[i].setStatus(rstatus);
   return false;
}

bool RuleSet::setValidityTime(string name, string from, string to) {
   for(unsigned i = 0; i<size(); i++) 
	if((*this)[i].name == name||(*this)[i].alias == name) return (*this)[i].setValidityTime(from, to);
   return false;
}

bool RuleSet::timeUpdate() {
  for(size_t i=0; i<size(); i++) {
     (*this)[i].timeUpdate();
  }
  return true;
}
