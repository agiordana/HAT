/*
 *  eventcounterset.cpp
 *  mngagent
 *
 *  Created by Attilio Giordana on 1/13/12.
 *  Copyright 2012 Università del Piemonte Orientale. All rights reserved.
 *
 */

using namespace std;

#include "eventcounterset.h"

EventCounterSet::EventCounterSet(string dir) {
	vector<string> filelist;
    string config_dir = hsrv::configdir+"/"+dir;
	
    FileManager::fileList(config_dir, filelist);
	
    for(unsigned i = 0; i < filelist.size(); i++) {
        // recupera il nome dell'eventcounter dal nome del file XML (eliminando l'estensione)
        push_back(EventCounter(dir, FileManager::getStem(filelist[i])));
    }
}

bool EventCounterSet::eventUpdate(MMessage& m) {
    double counter = 0;
    timeUpdate(); 
    for (unsigned i=0; i<size(); i++) {
        counter += (*this)[i].eventUpdate(m);
    }
	
    return counter > 0;
}

bool EventCounterSet::timeUpdate() {
	double counter = 0;
    
	for(unsigned i = 0; i < size(); i++) 
        counter += (*this)[i].timeUpdate();
	
    return counter > 0;
}

bool EventCounterSet::clearDQ(){
    for(size_t i=0; i<size(); i++)
        (*this)[i].clearDQ();
    return true;
}

bool EventCounterSet::observations(MParams &obs) {
    for(size_t i=0; i<this->size(); i++) (*this)[i].observations(obs);
    return true;
}
