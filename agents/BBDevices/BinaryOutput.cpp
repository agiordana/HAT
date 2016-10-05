#include "BinaryOutput.h"
#define INITIALSILENCE 5

using namespace std;

BinaryOutput::BinaryOutput(string dir, string n): MParams(dir, n) {
    load();
    name = n;
    duration = get("duration");
    if(duration == ""||duration == "0" || duration == "-1") duration = "0";
    default_value = get("default");
    if(duration != "0") wait_condition = get("duration") + "_" + get("name");
	else wait_condition = "";
    out = get("output", "output");
    if(get("status")!="") status = get("status");
    currentValue = get("value");
    if(currentValue == "") currentValue = default_value;
    if(currentValue == "") currentValue = "OFF";
    publish();
    dDuration = hsrv::a2double(duration);
    if(currentValue != "OFF" && duration != "0") deadline = hsrv::gettime();
      else deadline = 0;
}

bool BinaryOutput::execcmd(MMessage& msg) {
    double tt = hsrv::gettime();
    MMessage m;
    string value = msg.getString("value");
    if(msg.msubtype == name) {
	if(currentValue != value) {
            currentTime = tt;
            currentValue = value;
            string tvalue = remap(value);
            m.mtype = "devcmd";
            m.msubtype = out;
            m.add("value", tvalue);
            m.add("time", hsrv::double2a(currentTime));
            SubjectSet::notify(m);
            m.mtype = "event";
	    m.msubtype = get("eventname");
	    m.add("value", msg.getString("value"));
            SubjectSet::notify(m);
	    save();
	    publish();
	    if(duration != "0" && currentValue != default_value) {
	       deadline = tt+dDuration;
	       string info = name+" "+hsrv::double2a(tt)+", Deadline: "+hsrv::double2a(deadline);
	       hsrv::logger->info(info);
	    }
	    else if(duration != "0" && currentValue == default_value) {
	       deadline = 0;
	       string info = name+" deadline reset "+", Deadline: "+hsrv::double2a(deadline);
	       hsrv::logger->info(info);
	    }
        }
	else if(currentValue != default_value && duration != "0") {
	       deadline = hsrv::gettime()+dDuration;
	       string info = name+" "+hsrv::double2a(tt)+", Deadline: "+hsrv::double2a(deadline);
	       hsrv::logger->info(info);
	}
	if(msg.getString("tag") != "") hsrv::rpcNotify(msg.getString("tag"), "value", ("value="+value));
        return true;
    }
    else if(msg.mtype == "trigger" && msg.msubtype == "poll") {
	currentTime = tt;
        if(deadline >0 && currentTime >= deadline) {
	   deadline = 0;
           if(currentValue != default_value) {
              currentValue = default_value;
              string tvalue = remap(default_value);
              MMessage m;
              m.mtype = "devcmd";
              m.msubtype = out;
              m.add("value", tvalue);
              m.add("time", hsrv::double2a(currentTime));
              SubjectSet::notify(m);
              m.mtype = "event";
              m.msubtype = get("eventname");
              m.add("value", msg.getString("value"));
              SubjectSet::notify(m);
	      save();
	      publish();
	      string info = name+" deadline expired, Deadline: "+hsrv::double2a(deadline);
	      hsrv::logger->info(info);
          }
	  return true;
       }
       return false;
   }
   return false;
}

setof<MAttribute> BinaryOutput::input() {
    setof<MAttribute> tmp;
    return tmp;
}

setof<MAttribute> BinaryOutput::output() {
   setof<MAttribute> tmp;
   tmp.push_back(MAttribute(out, "output", "active"));
   return tmp;
}

string BinaryOutput::getValue() {
   return currentValue;
}

bool BinaryOutput::getSubjects(MParams& subj) {
   subj.add(get("eventname"),"subject", "event");
   subj.add(get("output","output"),"subject", "devcmd");
   return true;
}

bool BinaryOutput::is_deadline_waiting() {
   return deadline > 0;
}

bool BinaryOutput::getObservations(MParams& obs) {
   obs.add(name,"observe", "timer");
   obs.add(name,"observe", "cmd");
   return true;
}

string BinaryOutput::remap(string& value) {
    string tvalue = get(value);
    if(tvalue == "") return value;
    else return tvalue;
}

bool BinaryOutput::save() {
   add("value", currentValue);
   MParams::save();
   return true;
}

bool BinaryOutput::publish() {
   hsrv::publish(name, "type=\"target\" value=\""+currentValue+"\" status=\"" +get("status")+"\"");
   return true;
}
