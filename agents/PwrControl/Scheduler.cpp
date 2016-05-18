//
//  Scheduler.cpp
//  engagent
//
//  Created by Attilio Giordana on 7/15/14.
//  Copyright (c) 2014 Università del Piemonte Orientale. All rights reserved.
//

#include "Scheduler.h"
#define MAXTHRESHOLD 10000

using namespace std;

Scheduler::Scheduler(string file): MParams::MParams(file) {
    load();
    string control_dir = hsrv::configdir+"/control";
    string sensor_dir = hsrv::configdir+"/sensor";
    if (FileManager::isDir(control_dir)) {
        control = new ControlSet("control");
    }
    else control = NULL;
    if (FileManager::isDir(sensor_dir)) {
        sensor = new SensorSet("sensor");
    }
    else sensor = NULL;
    if(sensor != NULL) {
	control_measure = sensor->getControlMeasure();
	threshold = sensor->getThreshold();
    }
    stop_time = 0;
}


bool Scheduler::makePage() {
    ofstream cnt;
    cnt.open("cnt.xml");
    cnt<<"<scheduler>"<<endl;
    cnt<<"<set type=\"loads\">"<<endl;
    for(size_t i=0; i<control->size(); i++) {
       //creates file
        cnt<< "<load name=\""<<(*control)[i].get("name");
        cnt<<"\" status=\""<<(*control)[i].get("status");
        cnt <<"\" mode=\""<< (*control)[i].get("mode");
        cnt<<"\" priority=\""<<(*control)[i].get("priority");
        cnt<<"\"/>"<<endl;
      // publics the same item on the DynamicPage
	hsrv::publish((*control)[i].get("name"), "type=\"load\" device=\""+(*control)[i].get("name")+"\" status=\""+(*control)[i].get("status")+"\" priority=\""+(*control)[i].get("priority")+"\"");
    }
    cnt<<"</set>"<<endl;
    cnt<<"<target name=\""<<control_measure<<"\" value=\""<<hsrv::double2a(current)<<"\" limit=\""<<threshold<<"\"/>"<<endl;
    cnt<<"</scheduler>"<<endl;
    cnt.close();
    //publish the same item on DynamicPage
    if(control_measure!="")
	hsrv::publish(control_measure, "type=\"target\" device=\""+control_measure+"\" value=\""+hsrv::double2a(current)+"\" limit=\""+threshold+"\"");
    string pathname = hsrv::homedir+"/cnt.xml";
    string localurl = "scheduler/queue.xml";
    hsrv::archive->addResource(localurl, pathname, true);
    FileManager::deleteFile(hsrv::homedir, "cnt.xml");
    return true;
}

bool Scheduler::checkControl(MMessage& m, setof<MMessage>& action) {
    if(control == NULL) return false;
    if(get("status") != "ON") return false;
    if(m.msubtype != control_measure) return control->handleMode(m);
    double target;
    if(threshold!="") target = hsrv::a2double(threshold);
    else target = MAXTHRESHOLD;
    current = m.getDouble("Ipower");
    if(stop_time>0) {
        double delta = targetvalue_atstop - current;
        if(delta > 0) {
            (*control)[last_stopped].loadestimateUpdate(delta);
        }
        stop_time = 0;
    }
    if(current > target) reduceLoad(current, action);
    else if(current < target) tryIncreaseLoad(target - current, action);
    return true;
}

bool Scheduler::aligneControl(setof<MMessage>& action) {
    if(control == NULL) return false;
    if(get("status") != "ON") return false;
    for(size_t i=0; i<control->size(); i++) {
	if((*control)[i].get("mode")=="OFF") (*control)[i].stop(action, true);
	else if((*control)[i].get("mode")=="ON") (*control)[i].start(action, true);
    }
    return true;
}

bool Scheduler::setControl(MMessage& m) {
    if(control == NULL) return false;
    return control->setControl(m);
}

bool Scheduler::resetControl(MMessage& m) {
    if(control == NULL) return false;
    return control->resetControl(m);
}

bool Scheduler::resetTarget(MMessage& m) {
    if(sensor == NULL) return false;
    sensor->resetSensor(m);
    control_measure = sensor->getControlMeasure();
    threshold = sensor->getThreshold();
    return true;
}

bool Scheduler::setTarget(MMessage& m) {
    if(sensor == NULL) return false;
    sensor->setSensor(m);
    control_measure = sensor->getControlMeasure();
    threshold = sensor->getThreshold();
    return true;
}


bool Scheduler::reduceLoad(double current, setof<MMessage>& action) {
    int min_priority=1000;
    size_t m=control->size()+1;
    for(size_t i=0; i<control->size(); i++) {
        if((*control)[i].mayStop()&&(*control)[i].priority()<min_priority) {
            min_priority = (*control)[i].priority();
            m = i;
        }
    }
    if(m < control->size()) {
        (*control)[m].stop(action);
        last_stopped = m;
        stop_time = hsrv::gettime();
        targetvalue_atstop = current;
    }
    return true;
}

bool Scheduler::tryIncreaseLoad(double available, setof<MMessage>& action) {
    int max_priority=0;
    size_t m=control->size()+1;
    for(size_t i=0; i<control->size(); i++) {
        if((*control)[i].mayStart(available)&&(*control)[i].priority() >= max_priority) {
            max_priority = (*control)[i].priority();
            m = i;
        }
    }
    if(m < control->size()) {
        (*control)[m].start(action);
    }
    return true;
}

bool Scheduler::makeControlDevices() {
   vector<string> devname;
   string devbody;
   string c_template;
   ifstream in;
   string devtemplate = hsrv::configdir+"/control_template.xml";
   string dpathc = hsrv::configdir +"/control";
   string dpathh = hsrv::homedir +"/control";
   ofstream outc;
   ofstream outh;
   string res = hsrv::cmdExec("rm " + dpathc + "/*");
   res = hsrv::cmdExec("rm " + dpathh + "/*");
   in.open(devtemplate.c_str());
   char ch;
   while((ch=in.get())&&!in.eof()) {
      c_template+=ch;
   }
   in.close();
   devname = NameService::getNames("devices", get("control_category"));
   hsrv::saveItemList("controldevices", "status", devname);
   for(size_t i=0; i<devname.size(); i++) {
	devbody = c_template;
	hsrv::strReplace(devbody,"$NAME",devname[i]);
	hsrv::strReplace(devbody,"$NAME",devname[i]);
	hsrv::strReplace(devbody,"$NAME",devname[i]);
	string devpathc = dpathc+"/"+devname[i]+".xml";
	string devpathh = dpathh+"/"+devname[i]+".xml";
	outc.open(devpathc.c_str());
	for(size_t i=0; i<devbody.size(); i++) outc.put(devbody[i]);
	outc.close();
	outc.clear();
	outh.open(devpathh.c_str());
	for(size_t i=0; i<devbody.size(); i++) outh.put(devbody[i]);
	outh.close();
	outh.clear();
   }
   return true;
}

bool Scheduler::makeSensorDevices() {
   vector<string> devname;
   string devbody;
   string c_template;
   ifstream in;
   string devtemplate = hsrv::configdir+"/sensor_template.xml";
   string dpathc = hsrv::configdir +"/sensor";
   string dpathh = hsrv::homedir +"/sensor";
   ofstream outc;
   ofstream outh;
   string res = hsrv::cmdExec("rm " + dpathc + "/*");
   res = hsrv::cmdExec("rm " + dpathh + "/*");
   in.open(devtemplate.c_str());
   char ch;
   while((ch=in.get())&&!in.eof()) {
      c_template+=ch;
   }
   in.close();
   devname = NameService::getNames("devices", get("sensor_category"));
   hsrv::saveItemList("sensordevices", "status", devname);
   for(size_t i=0; i<devname.size(); i++) {
	devbody = c_template;
	hsrv::strReplace(devbody,"$NAME",devname[i]);
	hsrv::strReplace(devbody,"$NAME",devname[i]);
	string devpathc = dpathc+"/"+devname[i]+".xml";
	string devpathh = dpathh+"/"+devname[i]+".xml";
	outc.open(devpathc.c_str());
	for(size_t i=0; i<devbody.size(); i++) outc.put(devbody[i]);
	outc.close();
	outc.clear();
	outh.open(devpathh.c_str());
	for(size_t i=0; i<devbody.size(); i++) outh.put(devbody[i]);
	outh.close();
	outh.clear();
   }
   return true;
}

bool Scheduler::observations(MParams& obs) {
   sensor->observations(obs);
   return true;
}
