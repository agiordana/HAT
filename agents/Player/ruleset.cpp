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
}

int RuleSet::match(EventCounterSet* ec, BooleanConditionSet* cnd, setof<MMessage>& action) {
    int counter = 0;
    
    for (unsigned i = 0; i < size(); i++) {
	 if((*this)[i].match(ec, cnd, action)) counter++;
    }
    return counter;
}

bool RuleSet::isON(Rule& r) {
	if(r.get("status") != "ON") return false;
        return true;
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
