//
//  controlset.cpp
//  engagent
//
//  Created by Attilio Giordana on 7/14/14.
//  Copyright (c) 2014 Università del Piemonte Orientale. All rights reserved.
//

#include "SensorSet.h"

using namespace std;

SensorSet::SensorSet(string dir) {
    vector<string> filelist;
    string config_dir = hsrv::configdir+"/"+dir;
	
    FileManager::fileList(config_dir, filelist);
	
    for(size_t i = 0; i < filelist.size(); i++) {
        
        push_back(Sensor(dir, FileManager::getStem(filelist[i])));
    }
	// carica i parametri dai file XML per tutte le istanze Rule aggiunte
    for(size_t i = 0; i < filelist.size(); i++) {
        (*this)[i].load();
    }
}

bool SensorSet::observations(MParams &obs) {
    for(size_t i=0; i<this->size(); i++) (*this)[i].observations(obs);
    return true;
}


bool SensorSet::setSensor(MMessage& m) {
    string name = m.getString("sensorname");
    string pathc; 
    if(name=="") name = m.getString("name");
    if(name=="") name = m.getString("value");
    bool done = false;
    for (size_t i=0; i<size(); i++) 
      if(name == (*this)[i].get("name")) {
          (*this)[i].setStatus(m.getString("status"));
          (*this)[i].setMeasure(m.getString("control_measure"));
          (*this)[i].setThreshold(m.getString("threshold"));
          done = (*this)[i].ismeasure();
	  if(m.getString("configure") == "YES") {
	     pathc = hsrv::configdir + "/sensor/" + name + ".xml";
	     (*this)[i].xmlSave(pathc);
	  }
      }
    if(done) {
	for (size_t i=0; i<size(); i++)
	   if(name != (*this)[i].get("name")) {
	      (*this)[i].setMeasure("NO");
	      if(m.getString("configure") == "YES") {
	         pathc = hsrv::configdir + "/sensor/" + (*this)[i].get("name") + ".xml";
	         (*this)[i].xmlSave(pathc);
	      }
	  }
    }
    return true;
}

bool SensorSet::resetSensor(MMessage& m) {
    string name = m.getString("controlname");
    if(name=="") name = m.getString("name");
    if(name=="") name = m.getString("value");
    if (name =="") {
        for(size_t i=0; i<size(); i++) {
            (*this)[i].setMeasure("NO");
            (*this)[i].setThreshold("3000");
        }
        return true;
    }
    else 
        for(size_t i=0; i<size(); i++) {
            if((*this)[i].get("name") == name) {
               (*this)[i].setMeasure("NO");
               (*this)[i].setThreshold("3000");
               return true;
            }
        }
    return true;
}

string SensorSet::getControlMeasure() {
   for(size_t i=0; i<this->size(); i++)
     if((*this)[i].get("control_measure") == "YES") return (*this)[i].get("name");
   return "";
}

string SensorSet::getThreshold() {
   for(size_t i=0; i<this->size(); i++)
     if((*this)[i].get("control_measure") == "YES") return (*this)[i].get("threshold");
   return "";
}
