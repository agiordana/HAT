//
//  actionset.cpp
//  mngagent
//
//  Created by Attilio Giordana on 3/24/13.
//  Copyright (c) 2013 Università del Piemonte Orientale. All rights reserved.
//

#include "actionset.h"

using namespace std;

ActionSet::ActionSet(string dir, string tr_dir) {
    NameList filelist;
    NameList trfilelist;
    string config_dir = hsrv::configdir+"/"+dir;
    string config_tr_dir = hsrv::configdir+"/"+tr_dir;
	
    FileManager::fileList(config_dir, filelist);
    for(unsigned i = 0; i < filelist.size(); i++) {
        // recupera il nome della regola dal nome del file XML (eliminando l'estensione)
        push_back(Action(dir, FileManager::getStem(filelist[i])));
	(*this).back().load();
    }
    
    FileManager::fileList(config_tr_dir, trfilelist);
    for(size_t i = 0; i < trfilelist.size(); i++) {
        // recupera il nome della regola dal nome del file XML (eliminando l'estensione)
	string trname = FileManager::getStem(trfilelist[i]); 
        if(!filelist.member(trname)) {
	   push_back(Action(tr_dir, trname));
	   (*this).back().load();
	}
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

bool ActionSet::exec(string name) {
    for(size_t i=0; i<this->size(); i++) 
        if((*this)[i].get("name")==name) return (*this)[i].exec();
    return false;
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
