#include "AnalogInput.h"
#define INITIALSILENCE 5

using namespace std;

AnalogInput::AnalogInput(string dir, string n): MParams(dir, n) {
    load();
    name = n;
    in = get("input","input");
    currentTime = hsrv::gettime();
    if(get("status")!="") status = get("status");
    string sf = get("scale_factor");
    scale_factor = hsrv::a2double(sf);
}

bool AnalogInput::execcmd(MMessage& m) {
    return update(m.msubtype, m.getString("value"));
}

bool AnalogInput::update(string channel, string value) {
    double delta = 0;
    MMessage m;
    double tt = hsrv::gettime();
    if(channel == in) {
        string tvalue = remap(value);
        currentValue = tvalue;
        currentTime = tt;
        m.mtype = "event";
        m.msubtype = name;
        m.add("value", currentValue);
        m.add("time", hsrv::double2a(currentTime) );
        SubjectSet::notify(m);
        if(hsrv::mqtt == "ON") {
            MMessage mqtt_mess = mkMqttmessage("event", name, "value="+currentValue,"");
            SubjectSet::notify(mqtt_mess);
        }
        publish();
	return true;
    }
    return false;
}


setof<MAttribute> AnalogInput::input() {
    setof<MAttribute> tmp;
    tmp.push_back(MAttribute(in, "input", "active"));
    return tmp;
}

setof<MAttribute> AnalogInput::output() {
   setof<MAttribute> tmp;
   return tmp;
}

string AnalogInput::getValue() {
   return currentValue;
}

bool AnalogInput::getSubjects(MParams& subj) {
   subj.add(name,"subject", "event");
   return true;
}

string AnalogInput::remap(string& value) {
    double rvalue = hsrv::a2double(value);
    rvalue *= scale_factor;
    string tvalue = hsrv::double2a(rvalue);
    if(tvalue == "") return value;
    else return tvalue;
}

bool AnalogInput::publish() {
   hsrv::publish(name, "type=\"source\" value=\""+currentValue+"\" status=\""+get("status") + "\"");
   return true;
}

