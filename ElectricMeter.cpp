#include "ElectricMeter.h"
#define INITIALSILENCE 5

using namespace std;

ElectricMeter::ElectricMeter(string dir, string n): MParams(dir, n) {
    load();
    name = n;
    in = get("input","input");
    if(in=="") in = get("input");
    string tmp = get("alpha");
    if(tmp=="") alpha = 0;
    else alpha = hsrv::a2double(tmp);
    tmp = get("quantum");
    if(tmp=="") quantum = 1;
    else quantum = hsrv::a2double(tmp);
    tmp = get("unit");
    if(tmp=="") unit = "Wh";
    else unit = tmp;
    energy = Ipower = Apower = Wpower = 0;
    if((tmp = get("energy"))!="") energy = hsrv::a2double(tmp);
    if((tmp = get("deltaEnergy"))!="") deltaEnergy = hsrv::a2double(tmp);
    currentTime = origin = hsrv::gettime();
    if(get("status")!="") status = get("status");
	else status = "ON";
    deltaEnergy = 0;
    publish();
}

bool ElectricMeter::execcmd(MMessage& m) {
    return update(m.msubtype, m.getString("value"));
}

bool ElectricMeter::update(string channel, string value) {
    double delta = 0;
    double tt = hsrv::gettime();
    if(channel == in && value == "1") {
	double n = hsrv::a2double(value);
	delta = quantum * n;
	if(tt<origin+INITIALSILENCE) return false;
        energy += delta;
	deltaEnergy += delta;
	add("energy", hsrv::double2a(energy));
	add("deltaEnergy", hsrv::double2a(deltaEnergy));
	save();
	return true;
    }
    return false;
}

bool ElectricMeter::doMeasure() {
    double tt = hsrv::gettime();
    Apower = (energy/(tt-origin))*3600;
    Ipower = Ipower * (1 - alpha) + alpha * (deltaEnergy/(tt - currentTime)) * 3600;
    currentTime = tt;
    deltaEnergy = 0;
    return true;
}

bool ElectricMeter::reset(string field) {
    if(field == "energy") {
	 energy = 0;
	 origin = hsrv::gettime()-1;
	 currentTime = hsrv::gettime();
	 deltaEnergy = 0;
	 Ipower = Apower = 0;
         add("energy", hsrv::double2a(energy));
         add("deltaEnergy", hsrv::double2a(deltaEnergy));
	 save();
         return true;
    }
    return false;
}

bool ElectricMeter::publish() {
    MMessage m;
    m.mtype = "event";
    m.msubtype = name;
    m.add("energy", hsrv::double2a(energy));
    m.add("Ipower", hsrv::double2a(Ipower));
    m.add("Apower", hsrv::double2a(Apower));
    m.add("unit", unit);
    m.add("timeofday", hsrv::getasciitimecompact());
    SubjectSet::notify(m);
//    cout<<m.xmlEncode(0)<<endl;
    hsrv::publish(name, "type=\"source\" energy=\""+hsrv::double2a(energy)+"\" power=\""+hsrv::double2a(Ipower)+"\" status=\""+get("status")+"\"");
    return true;
}

setof<MAttribute> ElectricMeter::input() {
    setof<MAttribute> tmp;
    tmp.push_back(MAttribute(in, "input", "active"));
    return tmp;
}

setof<MAttribute> ElectricMeter::output() {
   setof<MAttribute> tmp;
   return tmp;
}

string ElectricMeter::getValue(string field) {
   if(field == "energy") return hsrv::double2a(energy);
   if(field == "Ipower") return hsrv::double2a(Ipower);
   if(field == "Apower") return hsrv::double2a(Apower);
   return "0";
}

bool ElectricMeter::getSubjects(MParams& subj) {
   subj.add(name,"subject", "event");
   return true;
}
