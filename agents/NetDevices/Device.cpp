/*
 * Device.cpp
 *
 *  Created on: Jun 23, 2017
 *      Author: Attilio
 */
#include "Device.h"

using namespace std;

Device::Device(string dir, string nm): MParams::MParams(dir, nm) {
        name = nm;
	MParams::load();
	load(dir, nm);
	scount = 0;
	turn = 0;
	cycle = 1;
}

Device::~Device() {

}

void Device::start_service_threads() {
    this->service_thread.push_back(new Thread(&Device::do_work, this));
}

void Device::do_work(Device* obj) {
    MMessage msg;
    string res;
    NameList action;
    string nextstatus="";
    publish();
    while(true) {
       msg = obj->receive_message();
       if(msg.mtype == "trigger" && msg.msubtype == "sync") {
	  string currentStatus = status();
	  sync();
          if(status() != currentStatus) {
		eventNotify();
	  	publish();
	  }
       }
       else {
	   string value = valueNormalize(msg.mtype, msg.msubtype, msg.getString("value"));
    	   action = control->actions(msg.mtype, msg.msubtype, value, obj->status());
    	   for(size_t i=0; i<action.size(); i++) {
    		   res = component[i].setValue(action[i]);
    		   if(nextstatus!="") nextstatus += "+";
    		   nextstatus += res;
    	   }
    	   obj->status(nextstatus);
	   nextstatus = "";
	   eventNotify();
	   publish();
           if(msg.getString("tag") != "") hsrv::rpcNotify(msg.getString("tag"), "value", ("value="+obj->status()));
      }
   }
}

bool Device::getObservations(MParams &obs) {
	ObserverThread::getObservations(obs);
	obs.add("sync", "observe", "trigger");
	for(size_t i=0; i<component.size(); i++) component[i].getObservations(obs);
	control->getObservations(obs);
	return true;
}

bool Device::getSubjects(MParams &subj) {
	ObserverThread::getSubjects(subj);
	subj.add(this->get("eventname", "generic"), "subject", "event");
	for(size_t i=0; i<component.size(); i++) component[i].getSubjects(subj);
	return true;
}

bool Device::load(string& dir, string& nm) {
	NameList devlist;
	string config_dir = hsrv::configdir+"/"+dir;
	FileManager::fileList(config_dir, devlist);
	for(size_t i=0; i<devlist.size(); i++) {
		string dname = FileManager::getStem(devlist[i]);
		if(dname=="control") {
			control = new Automaton(dir,dname);
		} else if(dname!=name && dname!="") {
			component.push_back(Component(dir,dname));
		}
	}
}

string Device::status() {
   string s="";
   for(size_t i=0; i<component.size(); i++) {
      if(s != "") s += "+";
      s += component[i].status();
   }
   return s;
}

string Device::status(string s) {
   NameList st;
   st.init(s);
   for(size_t i=0; i<st.size() && i<component.size(); i++) {
	component[i].add("status", "generic", st[i]);
	component[i].save();
   }
   return s;
}
  
bool Device::sync() {
  scount++;
  if(scount>=cycle) scount = 0;
  if(scount != turn) return false;
  for(size_t i=0; i<component.size(); i++) component[i].sync();
  return true;
}

bool Device::publish() {
  hsrv::publish(get("name"), "type=\"target\" value=\""+status()+"\" status=\"" +get("active", "generic")+"\"");
}

string Device::aggregateValue(string value) {
   string res = "";
   for(size_t i=0; i<value.length(); i++)
     if(value[i]==' ') res+='+';
	else res+=value[i];
   return res;
}

bool Device::eventNotify() {
  double currentTime = hsrv::gettime();
  MMessage m;
  m.add("value", status());
  m.add("time", hsrv::double2a(currentTime));
  m.mtype = "event";
  m.msubtype = get("eventname");
  SubjectSet::notify(m);
  return true;
}

string Device::valueNormalize(string& mtype, string& msubtype, string value) {
  if(mtype == "cmd") return aggregateValue(value);
  if(mtype == "event" && msubtype.substr(0,2)=="LS") return "ON";
  return value;
} 
