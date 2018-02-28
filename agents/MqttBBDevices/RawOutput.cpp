#include "RawOutput.h"
#define INITIALSILENCE 5

using namespace std;

RawOutput::RawOutput(string dir, string n): MParams(dir, n) {
    load();
    rawtype = get("type");
    name = get("capename");
    out = get("capename");
    if(get("status")!="") status = get("status");
	else status = "ON";
}

bool RawOutput::execcmd(MMessage& msg) {
    double tt = hsrv::gettime();
    string value = msg.getString("value");
    if(msg.msubtype == name) {
        currentTime = tt;
        currentValue = value;
        string tvalue = remap(value);
        MMessage m;
        m.mtype = "devcmd";
        m.msubtype = out;
        m.add("value", tvalue);
        m.add("time", hsrv::double2a(currentTime));
	SubjectSet::notify(m);
	return true;
    }
    return false;
}

setof<MAttribute> RawOutput::input() {
    setof<MAttribute> tmp;
    return tmp;
}

setof<MAttribute> RawOutput::output() {
   setof<MAttribute> tmp;
   return tmp;
}

string RawOutput::getValue() {
   return currentValue;
}

bool RawOutput::getSubjects(MParams& subj) {
   subj.add(name,"subject", "devcmd");
   return true;
}

bool RawOutput::getObservations(MParams& obs) {
   obs.add(name,"observe", "cmd");
   return true;
}

string RawOutput::remap(string& value) {
    if(rawtype == "booleanout" && value == "ON") return "1";
    if(rawtype == "booleanout" && value == "OFF") return "0";
    if(rawtype == "booleanout" && value == "1") return "1";
    if(rawtype == "booleanout" && value == "0") return "0";
    if(rawtype != "booleanout") {
       int tvalue = hsrv::a2int(value);
       return hsrv::int2a(tvalue);
    }
    return "0";
}
