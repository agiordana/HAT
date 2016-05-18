//
//  controlset.cpp
//  engagent
//
//  Created by Attilio Giordana on 7/14/14.
//  Copyright (c) 2014 Università del Piemonte Orientale. All rights reserved.
//

#include "ControlSet.h"

using namespace std;

ControlSet::ControlSet(string dir) {
    vector<string> filelist;
    string config_dir = hsrv::configdir+"/"+dir;
	
    FileManager::fileList(config_dir, filelist);
	
    for(unsigned i = 0; i < filelist.size(); i++) {
        
        push_back(Control(dir, FileManager::getStem(filelist[i])));
    }
	// carica i parametri dai file XML per tutte le istanze Rule aggiunte
    for(size_t i = 0; i < filelist.size(); i++) {
        (*this)[i].load();
    }
}

bool ControlSet::subjects(MParams &subj) {
    for(size_t i=0; i<this->size(); i++) (*this)[i].subjects(subj);
    return true;
}

bool ControlSet::observations(MParams &obs) {
    for(size_t i=0; i<this->size(); i++) (*this)[i].observations(obs);
    return true;
}


bool ControlSet::setControl(MMessage& m) {
    string name = m.getString("controlname");
    if(name=="") name = m.getString("name");
    if(name=="") name = m.getString("value");
    for (size_t i=0; i<size(); i++) 
      if(name == (*this)[i].get("name")) {
          if(m.getString("status")!="")(*this)[i].setStatus(m.getString("status"));
          if(m.getString("mode")!="")(*this)[i].setMode(m.getString("mode"));
          if(m.getString("priority")!="")(*this)[i].setPriority(m.getString("priority"));
          if(m.getString("nominalload")!="")(*this)[i].setLoad(m.getString("nominalload"));
	  if(m.getString("configure")=="YES") {
	     string pathc = hsrv::configdir + "/control/" + name + ".xml";
	     (*this)[i].xmlSave(pathc);
	  }
          return true;
      }
    return false;
}

bool ControlSet::resetControl(MMessage& m) {
    string name = m.getString("controlname");
    if(name=="") name = m.getString("name");
    if(name=="") name = m.getString("value");
    if (name =="") {
        for(size_t i=0; i<size(); i++) {
            (*this)[i].setMode("OFF");
            (*this)[i].setPriority("0");
        }
        return true;
    }
    else 
        for(size_t i=0; i<size(); i++) {
            if((*this)[i].get("name") == name) {
                (*this)[i].setMode("OFF");
                (*this)[i].setPriority("0");
                return true;
            }
        }
    return true;
}


bool ControlSet::handleMode(MMessage& m) {
    string mode = m.getString("value");
    if(mode=="1") mode = "ON";
    else if(mode == "0") mode = "OFF";
    for(size_t i=0; i<size(); i++)
        if((*this)[i].get("name")==m.msubtype) {
            (*this)[i].setMode(mode);
        }
    return true;
}
