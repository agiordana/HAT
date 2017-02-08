#include "RGBPwm.h"
#define INITIALSILENCE 5

using namespace std;

RGBPwm::RGBPwm(string dir, string n): MParams(dir, n) {
    load();
    name = n;
    Rout = get("red","output");
    Gout = get("green","output");
    Bout = get("blue","output");
    R = 255;
    G = 255;
    B = 255;
    currentColor = get("color");
    if(currentColor == "") currentColor = get("default_color");
    if(currentColor == "") currentColor = "ffffff";
    currentIntensity = get("intensity");
    if(currentIntensity == "") currentIntensity = "0";
    intensity = hsrv::a2int(currentIntensity);
    outputStatus = get("outputStatus");
    if(outputStatus == "") outputStatus = get("default");
    if(outputStatus == "") outputStatus = "OFF";
    if(get("status")!="") status = get("status");
    publish();
}

setof<MAttribute> RGBPwm::input() {
    setof<MAttribute> tmp;
    return tmp;
}

bool RGBPwm::execcmd(MMessage& msg) {
    double tt = hsrv::gettime();
    string value;
    string tvalue;
    string color;
    if(msg.msubtype == name) {
        currentTime = tt;
	NameList values;
        values.init(msg.getString("value"),'.');
	if(values.size()>1) {
	   value = values[0];
	   color = values[1];
        }
	else {
          value = msg.getString("value");
          color = msg.getString("color");
	}
	if(value=="OFF"||value=="ON") {
	    outputStatus = value;
	    sendCmd(value);
	}
        else if(currentIntensity != value && value!="") {
            tvalue = remap(value);
	    currentIntensity = value;
	    intensity = hsrv::a2int(tvalue);
	    sendCmd(tvalue);
        }
	else if(color!="" && color!=currentColor) {
	    currentColor = color;
	    R = ha2int(color.substr(0,2));
	    G = ha2int(color.substr(2,2));
	    B = ha2int(color.substr(4,2));
            if(hsrv::debug>0) cout<<"R: "<<R<<", G: "<<G<<", B: "<<B<<endl;
	    sendCmd();
        }
	eventNotify(tvalue);
	if(msg.getString("tag")!="") {
	   if(outputStatus=="ON") hsrv::rpcNotify(msg.getString("tag"), "value", ("value="+currentIntensity+",color="+currentColor));
	   else hsrv::rpcNotify(msg.getString("tag"), "value", ("value=0,color="+currentColor));
	}
        return true;
    }
    return false;
}

bool RGBPwm::sendCmd(string value) {
   MMessage m;
   double nf = 255;
   if(hsrv::debug>0) cout<<"NF: "<<nf<<", Intensity: "<<intensity<<endl;
   string tvalue;
   m.mtype = "devcmd";
   // Red
   m.msubtype = Rout;
   tvalue = hsrv::int2a((int)R*intensity/nf);
   if(value=="ON"||value=="OFF") m.add("value", value);
       else m.add("value", tvalue);
   m.add("time", hsrv::double2a(currentTime));
   SubjectSet::notify(m);
   if(hsrv::debug>0) cout<<"R: "<<m.xmlEncode(0)<<endl;
   // Green
   m.msubtype = Gout;
   tvalue = hsrv::int2a((int)G*intensity/nf);
   if(value=="ON"||value=="OFF") m.add("value", value);
       else m.add("value", tvalue);
   m.add("time", hsrv::double2a(currentTime));
   SubjectSet::notify(m);
   if(hsrv::debug>0) cout<<"G: "<<m.xmlEncode(0)<<endl;
   // Blue
   m.msubtype = Bout;
   tvalue = hsrv::int2a((int)B*intensity/nf);
   if(value=="ON"||value=="OFF") m.add("value", value);
       else m.add("value", tvalue);
   m.add("time", hsrv::double2a(currentTime));
   SubjectSet::notify(m);
   if(hsrv::debug>0) cout<<"B: "<<m.xmlEncode(0)<<endl;
}

bool RGBPwm::eventNotify(string value) {
   MMessage m;
   m.mtype = "event";
   m.msubtype = get("eventname");
   if(outputStatus=="OFF") {
	m.add("value", "0");
   }
   else {
	m.add("value", currentIntensity);
   }
   m.add("intensity", hsrv::int2a(intensity));
   m.add("color", currentColor);
   SubjectSet::notify(m);
   save();
   publish();
   return true;
}

setof<MAttribute> RGBPwm::output() {
   setof<MAttribute> tmp;
   tmp.push_back(Rout);
   tmp.push_back(Gout);
   tmp.push_back(Bout);
   return tmp;
}

string RGBPwm::getValue() {
   if(currentIntensity=="") return "0";
   else return currentIntensity;
}

bool RGBPwm::getSubjects(MParams& subj) {
   subj.add(get("eventname"),"subject", "event");
   subj.add(get("red", "output"),"subject", "devcmd");
   subj.add(get("green", "output"),"subject", "devcmd");
   subj.add(get("blue", "output"),"subject", "devcmd");
   return true;
}

bool RGBPwm::getObservations(MParams& subj) {
   subj.add(name,"observe", "cmd");
   return true;
}

string RGBPwm::remap(string& value) {
    string tvalue = get(value,"remap");
    if(tvalue == "") return value;
    else return tvalue;
}

int RGBPwm::ha2int(string hnum) {
   int res;
   stringstream s;
   s<<std::hex<<hnum;
   s >> res;
   return res;
}

bool RGBPwm::save() {
   add("intensity", hsrv::int2a(intensity));
   add("color", currentColor);
   add("outputStatus", outputStatus);
   MParams::save();
   return true;
}

bool RGBPwm:: publish() {
   if(outputStatus == "ON") hsrv::publish(name, "type=\"target\" value=\""+currentIntensity+"\" color=\""+currentColor+"\" status=\"" +status+"\"");
	else hsrv::publish(name, "type=\"target\" value=\"0\" color=\""+currentColor+"\" status=\"" +status+"\"");
   return true;
}

