#include "RawInput.h"

using namespace std;

RawInput::RawInput(string dir, string n): MParams(dir, n) {
    load();
    rawtype = get("type");
    name = "V"+get("capename");
    out = get("capename");
    if(get("status")!="") status = get("status");
	else status = "ON";
}

bool RawInput::execcmd(MMessage& msg) {
    double tt = hsrv::gettime();
    string value = msg.getString("value");
    if(msg.msubtype == name) {
        currentTime = tt;
        currentValue = value;
        string tvalue = remap(value);
        MMessage m;
        m.mtype = "local_event";
        m.msubtype = out;
        m.add("value", tvalue);
        m.add("time", hsrv::double2a(currentTime));
	SubjectSet::notify(m);
	hsrv::publish(name, tvalue);
	return true;
    }
    return false;
}

setof<MAttribute> RawInput::input() {
    setof<MAttribute> tmp;
    return tmp;
}

setof<MAttribute> RawInput::output() {
   setof<MAttribute> tmp;
   return tmp;
}

string RawInput::getValue() {
   return currentValue;
}

bool RawInput::getSubjects(MParams& subj) {
   subj.add(get("capename"),"subject", "local_event");
   return true;
}

bool RawInput::getObservations(MParams& obs) {
   obs.add(name,"observe", "cmd");
   return true;
}

string RawInput::remap(string& value) {
    if(rawtype == "booleanin" && value == "ON") return "1";
    if(rawtype == "booleanin" && value == "OFF") return "0";
    if(rawtype == "booleanin" && value == "1") return "1";
    if(rawtype == "booleanin" && value == "0") return "0";
    if(rawtype != "booleanin") {
       int tvalue = hsrv::a2int(value);
       return hsrv::int2a(tvalue);
    }
    return "0";
}
