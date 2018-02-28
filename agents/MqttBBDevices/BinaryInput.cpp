#include "BinaryInput.h"
#define INITIALSILENCE 5

using namespace std;

BinaryInput::BinaryInput(string dir, string n): MParams(dir, n) {
    load();
    name = n;
    in = get("input","input");
    currentTime = hsrv::gettime();
    if(get("status")!="") status = get("status");
    if(get("delay")!="") delay = get("delay");
	else delay = "0";
    currentValue = "0";
    publish();
}

bool BinaryInput::setstatus(MMessage& m) {
   string newstatus = m.getString("value");
   if((newstatus == "ON" || newstatus == "OFF")&& get("status") != newstatus) {
      add("status", newstatus);
      status = newstatus;
      save();
      publish();
   }
// invio della notifica dello status alla rpc
   if(m.getString("tag") != "") hsrv::rpcNotify(m.getString("tag"), "value", ("status="+status));
   if(hsrv::mqtt == "ON") {
       MMessage mqtt_mess = mkMqttmessage("event", name, "status="+status, m.getString("mqtttag"));
       SubjectSet::notify(mqtt_mess); 
   }

   return true;
}

bool BinaryInput::execcmd(MMessage& m) {
    if(get("status") == "OFF") return false;
    if(m.mtype == "wait" && m.msubtype == "end") {
	string delay_key = m.getString("value");
	if(delay_table.count(delay_key) > 0) {
	   SubjectSet::notify(delay_table[delay_key]);
	   map<string, MMessage>::iterator it;
	   it = delay_table.find(delay_key);
	   delay_table.erase(it);
	}
    }
    else return update(m.msubtype, m.getString("value"));
}

bool BinaryInput::update(string channel, string value) {
    double delta = 0;
    MMessage m;
    double tt = hsrv::gettime();
    if(get("status") == "OFF") return false;
    if(channel == in) {
        string tvalue = remap(value);
        if(tvalue != currentValue) {
            currentValue = tvalue;
            currentTime = tt;
            m.mtype = "event";
            m.msubtype = name;
            m.add("value", currentValue);
            m.add("time", hsrv::double2a(currentTime) );
            if(delay == "0" || delay == "") {
		SubjectSet::notify(m);
		publish();
	    }
	    else {
	      string delay_key = delay + "_" + name+SEPARATOR+ hsrv::getasciitimecompact();
	      if(delay_table.count(delay_key) == 0) {
		 delay_table[delay_key] = m;
		 m.mtype = "wait";
		 m.msubtype = "set";
		 m.add("value", delay_key);
		 SubjectSet::notify(m);
	      }
	   } 
        }
	return true;
    }
    return false;
}


setof<MAttribute> BinaryInput::input() {
    setof<MAttribute> tmp;
    tmp.push_back(MAttribute(in, "input", "active"));
    return tmp;
}

setof<MAttribute> BinaryInput::output() {
   setof<MAttribute> tmp;
   return tmp;
}

string BinaryInput::getValue() {
   return currentValue;
}

bool BinaryInput::getObservations(MParams& obs) {
   obs.add(name,"observe", "set");
   return true;
}

bool BinaryInput::getSubjects(MParams& subj) {
   subj.add(name,"subject", "event");
   return true;
}

string BinaryInput::remap(string& value) {
    string tvalue = get(value,"remap");
    if(tvalue == "") return value;
    else return tvalue;
}

bool BinaryInput::publish() {
   hsrv::publish(name, "type=\"source\" value=\""+currentValue+"\" status=\"" +get("status")+"\"");
   return true;
}

