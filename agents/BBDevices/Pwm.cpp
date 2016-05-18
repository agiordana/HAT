#include "Pwm.h"
#define INITIALSILENCE 5

using namespace std;

Pwm::Pwm(string dir, string n): MParams(dir, n) {
    load();
    name = n;
    out = get("output");
    if(out=="") out = get("output", "output");
    if(get("status")!="") status = get("status");
    currentValue = get("value");
    if(currentValue == "") currentValue = get("default_value");
    if(currentValue == "") currentValue = "0";
    publish(get("value"));
}

setof<MAttribute> Pwm::input() {
    setof<MAttribute> tmp;
    return tmp;
}

bool Pwm::execcmd(MMessage& msg) {
    double tt = hsrv::gettime();
    MMessage m;
    string value = msg.getString("value");
    if(hsrv::debug>0) cout<<msg.xmlEncode(0)<<", NAME: "<<name<<", CV: "<<currentValue<<endl;
    if(msg.msubtype == name) {
        if(currentValue != value) {
            currentTime = tt;
            string tvalue = remap(value);
            m.mtype = "devcmd";
            m.msubtype = out;
            m.add("value", tvalue);
            m.add("time", hsrv::double2a(currentTime));
            SubjectSet::notify(m);
            if(hsrv::debug>0) cout<<m.xmlEncode(0)<<endl;
        }
        eventNotify(value);
        if(msg.getString("tag") != "") hsrv::rpcNotify(msg.getString("tag"), "value", ("value="+(value=="OFF"?"0":currentValue)));
        return true;
    }
    return false;
}


setof<MAttribute> Pwm::output() {
   setof<MAttribute> tmp;
   tmp.push_back(out);
   return tmp;
}

string Pwm::getValue() {
   if(currentValue=="") return "0";
   else return currentValue;
}

bool Pwm::getSubjects(MParams& subj) {
   subj.add(get("eventname"),"subject", "event");
   subj.add(get("output", "output"),"subject", "devcmd");
   return true;
}

bool Pwm::getObservations(MParams& subj) {
   subj.add(name,"observe", "cmd");
   return true;
}

string Pwm::remap(string& value) {
    string tvalue = get(value);
    if(tvalue == "") return value;
    else return tvalue;
}

bool Pwm::save() {
    add("value", currentValue);
    MParams::save();
    return true;
}

bool Pwm::eventNotify(string value) {
   MMessage m;
   m.mtype = "event";
   m.msubtype = get("eventname");
   if(value=="OFF") m.add("value", "0");
   else if(value=="ON") m.add("value", currentValue);
   else m.add("value", value);
   SubjectSet::notify(m);
   if(hsrv::debug>0) cout<<m.xmlEncode(0)<<endl;
   if(value!="OFF" && value!="ON") currentValue = value;
   save();
   publish(value);
   return true;
}

bool Pwm::publish(string value) {
   string val;
   if(value == "OFF") val = "0";
   else val = currentValue;
   hsrv::publish(name, "type=\"target\" value=\""+val+"\" status=\"" +get("status")+"\"");
   return true;
}

