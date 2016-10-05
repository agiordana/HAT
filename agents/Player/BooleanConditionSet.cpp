//
//  BooleanConditionSet.cpp
//  mngagent
//
//  Created by Attilio Giordana on 1/1/15.
//  Copyright (c) 2015 Università del Piemonte Orientale. All rights reserved.
//

#include "BooleanConditionSet.h"

using namespace std;

BooleanConditionSet::BooleanConditionSet(string dir) {
    vector<string> filelist;
    string config_dir = hsrv::configdir+"/"+dir;
    FileManager::fileList(config_dir, filelist);
    for(unsigned i = 0; i < filelist.size(); i++) {
        // recupera il nome dell'eventcounter dal nome del file XML (eliminando l'estensione)
        push_back(BooleanCondition(dir, FileManager::getStem(filelist[i])));
    }
}

bool BooleanConditionSet::eventUpdate(MMessage& m) {
    double counter = 0;
    
    for (unsigned i=0; i<size(); i++) {
        counter += (*this)[i].eventUpdate(m);
    }
    return counter > 0;
}

bool BooleanConditionSet::observations(MParams &obs) {
    for(size_t i=0; i<this->size(); i++) (*this)[i].observations(obs);
    return true;
}