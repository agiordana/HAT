#include "TriStateOutput.h"
#define INITIALSILENCE 5

using namespace std;

TriStateOutput::TriStateOutput(string dir, string n): MParams(dir, n) {
    load();
    name = n;
    duration = get("duration");
    if(duration == ""||duration == "0" || duration == "-1") duration = "0";
    default_status = get("default");
    if(duration != "0") wait_condition = get("duration") + "_" + get("name");
	else wait_condition = "";
    out1 = get("up_output", "output");
    out2 = get("down_output", "output");
    ing1 = get("up_input", "input");
    ing2 = get("down_input", "input");
    ing1v = get("up_input_def_value");
    if(ing1v == "") ing1v = "1";
    ing2v = get("down_input_def_value");
    if(ing2v == "") ing2v = "1";
    if(get("status")!="") status = get("status");
    currentStatus = get("currentStatus");
    publish();
}

bool TriStateOutput::execcmd(MMessage& msg) {
    double tt = hsrv::gettime();
    bool res;
    currentTime = tt;
    checkUpcase(msg);
    string newstate = msg.getString("value");
    string value;
    string tvalue;
    if(status == "OFF") {
	hsrv::rpcNotify(msg.getString("tag"), "value", ("value=none"));
	return false;
    }
    MMessage m;
    if(msg.mtype == "cmd" && msg.msubtype == name) {
        bool res = docmdaction(msg.getString("value"));
	if(msg.getString("tag") != "") hsrv::rpcNotify(msg.getString("tag"), "value", ("value="+newstate));
	return res;
    }
    if(msg.mtype == "local_event") {
       if(msg.msubtype == ing1) ing1v = msg.getString("value");
       else if(msg.msubtype == ing2) ing2v = msg.getString("value");
       if(ing1v == "0" && ing2v == "1") docmdaction("UP");
       else if(ing1v == "1" && ing2v == "0") docmdaction("DOWN");
       else if(ing1v == "1" && ing2v == "1") docmdaction("OFF");
       else if(ing1v == "0" && ing2v == "0") docmdaction("UP");
       return true;
    }
    if(msg.mtype == "wait" && msg.msubtype == "end") {
	return dowaitaction();
    }
    return false;
}

setof<MAttribute> TriStateOutput::input() {
    setof<MAttribute> tmp;
    tmp.push_back(MAttribute(ing1, "input", "active"));
    tmp.push_back(MAttribute(ing2, "input", "active"));
    return tmp;
}

setof<MAttribute> TriStateOutput::output() {
   setof<MAttribute> tmp;
   tmp.push_back(MAttribute(out1, "output", "active"));
   tmp.push_back(MAttribute(out2, "output", "active"));
   return tmp;
}

string TriStateOutput::getValue() {
   return currentStatus;
}

bool TriStateOutput::getSubjects(MParams& subj) {
   subj.add(get("eventname"),"subject", "event");
   subj.add(get("output1","output"),"subject", "devcmd");
   subj.add(get("output2","output"),"subject", "devcmd");
   return true;
}

bool TriStateOutput::getObservations(MParams& obs) {
   obs.add(name,"observe", "cmd");
   return true;
}

string TriStateOutput::remap(string& value) {
    string tvalue = get(value,"remap");
    if(tvalue == "") return value;
    else return tvalue;
}

bool TriStateOutput::docmdaction(string newstate) {
    string value;
    string tvalue;
    MMessage m;
    m.mtype = "devcmd";
    m.add("time", hsrv::double2a(currentTime));
    if(currentStatus != newstate) {
       if(newstate == "UP") {
          value = "OFF";
          tvalue = remap(value);
          m.msubtype = out2;
          m.add("value", tvalue);
          SubjectSet::notify(m);
          value = "ON";
          tvalue = remap(value);
          m.msubtype = out1;
          m.add("value", tvalue);
          SubjectSet::notify(m);
       }
       else if(newstate == "DOWN") {
          value = "OFF";
          tvalue = remap(value);
          m.msubtype = out1;
          m.add("value", tvalue);
          SubjectSet::notify(m);
          value = "ON";
          tvalue = remap(value);
          m.msubtype = out2;
          m.add("value", tvalue);
          SubjectSet::notify(m);
       }
       else if(newstate == "OFF") {
          value = "OFF";
          tvalue = remap(value);
          m.msubtype = out1;
          m.add("value", tvalue);
          SubjectSet::notify(m);
          m.msubtype = out2;
          SubjectSet::notify(m);
      }
      currentStatus = newstate;
      if(duration != "0" && currentStatus != default_status) {
         m.mtype = "wait";
         m.msubtype = "set";
         m.add("value", wait_condition);
         SubjectSet::notify(m);
      }
   }
   notifyEvent(newstate);
   return true;
}

bool TriStateOutput::dowaitaction() {
   string value;
   string tvalue;
   MMessage m;
   if(currentStatus != default_status) {
      value = default_status;
      tvalue = remap(value);
      m.mtype = "devcmd";
      m.msubtype = out1;
      m.add("value", tvalue);
      m.add("time", hsrv::double2a(currentTime));
      SubjectSet::notify(m);
      m.msubtype = out2;
      SubjectSet::notify(m);
      notifyEvent(default_status);
      currentStatus = default_status;
   }
   return true;
}

bool TriStateOutput::notifyEvent(string value) {
   MMessage m;
   m.mtype = "event";
   m.msubtype = get("eventname");
   m.add("value", value);
   m.add("time", hsrv::double2a(currentTime));
   SubjectSet::notify(m);
   save();
   publish();
   return true;
}

bool TriStateOutput::publish() {
   hsrv::publish(name, "type=\"target\" value=\""+currentStatus+"\" status=\"" +get("status")+"\"");
   return true;
}

bool TriStateOutput::save() {
   add("status", status);
   add("currentStatus", currentStatus);
   MParams::save();
   return true;
}

bool TriStateOutput::checkUpcase(MMessage& m) {
    string astate = m.getString("value");
    string newstate;
    if(astate == "off") newstate="OFF";
    else if(astate == "up") newstate="UP";
    else if(astate == "down") newstate="DOWN";
    else newstate = astate;
    m.add("value", newstate);
    return true;
}

