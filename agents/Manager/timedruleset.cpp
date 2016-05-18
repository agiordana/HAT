//
//  timeruleset.cpp
//  mngagent
//
//  Created by Attilio Giordana on 3/28/12.
//  Copyright (c) 2012 Università del Piemonte Orientale. All rights reserved.
//

using namespace std;

#include "timedruleset.h"

TimedRuleSet::TimedRuleSet(string dir) {
    vector<string> filelist;
    string config_dir = hsrv::configdir+"/"+dir;
    
    FileManager::fileList(config_dir, filelist);
    
    for(unsigned i = 0; i < filelist.size(); i++) {
        // recupera il nome della regola dal nome del file XML (eliminando l'estensione)
        push_back(TimedRule(dir, FileManager::getStem(filelist[i])));
    }
}

bool TimedRuleSet::timeUpdate(string program_name, int program, BooleanConditionSet* cnd, setof<MMessage>& res) {
    size_t n = res.size();
    if(size() == 0) return false;
    for (unsigned i = 0; i<size(); i++) {
        (*this)[i].timeUpdate(program_name, program, cnd, res);
    }
    return res.size()>n;
}

bool TimedRuleSet::setStatus(string name, string rstatus) {
   for(unsigned i = 0; i<size(); i++)
        if((*this)[i].name == name||(*this)[i].alias == name) return (*this)[i].setStatus(rstatus);
   return false;
}

bool TimedRuleSet::setValidityTime(string name, string from, string to) {
   for(unsigned i = 0; i<size(); i++)
        if((*this)[i].name == name||(*this)[i].alias == name) return (*this)[i].setValidityTime(from, to);
   return false;
}

bool TimedRuleSet::subjects(MParams& subj) {
    for(size_t i=0; i<this->size(); i++) (*this)[i].subjects(subj);
    return true;
}

bool TimedRuleSet::setActionList(string name, string setlist, string resetlist) {
   for(size_t i = 0; i<size(); i++)
        if((*this)[i].name == name) return (*this)[i].setActionList(setlist, resetlist);
   return false;
}
