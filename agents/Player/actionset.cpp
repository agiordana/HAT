//
//  actionset.cpp
//  mngagent
//
//  Created by Attilio Giordana on 3/24/13.
//  Copyright (c) 2013 Università del Piemonte Orientale. All rights reserved.
//

#include "actionset.h"

using namespace std;

ActionSet::ActionSet(string dir) {
    NameList filelist;
    NameList trfilelist;
    string config_dir = hsrv::configdir+"/"+dir;
	
    FileManager::fileList(config_dir, filelist);
    for(unsigned i = 0; i < filelist.size(); i++) {
        // recupera il nome della regola dal nome del file XML (eliminando l'estensione)
        push_back(Action(dir, FileManager::getStem(filelist[i])));
	(*this).back().load();
    }
    
}

bool ActionSet::setStatus(string name, string value) {
    for(size_t i=0; i<this->size(); i++)
        if((*this)[i].get("name")==name) {
            return (*this)[i].setStatus(value);
        }
    return false;
}

bool ActionSet::isActive(string n) {
    for(size_t i=0; i<this->size(); i++)
        if ((*this)[i].get("name")==n) {
            if((*this)[i].get("status")=="ON") return true;
        }
    return false;
}

bool ActionSet::exec(string name, string d) {
    for(size_t i=0; i<this->size(); i++) 
        if((*this)[i].get("name")==name) return (*this)[i].exec(d);
    return false;
}

bool ActionSet::purge_expired() {
   double tt=hsrv::gettime();
   for(size_t i=0; i<this->size(); i++) while((*this)[i].purge_expired(tt));
   return true;
}

bool ActionSet::subjects(MParams &subj) {
    for(size_t i=0; i<this->size(); i++)
        (*this)[i].subjects(subj);
    return true;
}

bool ActionSet::observations(MParams &obs) {
    for(size_t i=0; i<this->size(); i++)
        (*this)[i].observations(obs);
    return true;
}
