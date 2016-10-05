#include "Gpio.h"

using namespace std;

NameList Gpio::initDone;

Gpio::Gpio(string dir, string file): MParams::MParams(dir, file) {
    load();
    name = get("capename");
    gpioname = get("gpioname");
    gpiotype = get("type");
    gpiopath = get("gpiopath");
    gpiovalue = gpiopath+"/value";
    pathname = "";
    wait_until = 0;
    string waitfor = get("waitfor");
    if(waitfor == "") w = 1;
    else w = hsrv::a2double(waitfor);
    currentValue=get("currentValue");
    if(currentValue=="") currentValue = get("default_value");
    dutyCycle=get("dutyCycle");
    if(dutyCycle=="") dutyCycle = "0";
}


bool Gpio::initialise() {
    setof<MAttribute> init;
    string gpiopath = get("gpiopath");
    string cmd;
    string res;
    setof<MAttribute>::iterator it;
    init = extractCategory("init");
    if(!FileManager::isDir(gpiopath) && init.size()>0) {
       for(it=init.begin(); it!=init.end(); it++) {
          cmd = it->value;
          if(!initDone.member(cmd)) {
             res = hsrv::cmdExec(cmd);
             initDone.add(cmd);
	     hsrv::logger->info(cmd);
             sleep(1);
          }
       }
    }
    return true;
}

bool Gpio::program() {
    setof<MAttribute> prg;
    string cmd;
    string res;
    setof<MAttribute>::iterator it;
    prg = extractCategory("script");
    for(it=prg.begin(); it!=prg.end(); it++) {
        cmd = it->value;
        res = hsrv::cmdExec(cmd);
	hsrv::logger->info("cmd: "+cmd);
	sleep(1);
    }
    return true;
}


string Gpio::getpath(string value) {
  if(gpiotype=="booleanout") return gpiovalue;
  if(gpiotype=="pwmout" && (value == "ON" || value == "on" || value == "OFF" || value == "off")) return get("duty");
  return get("duty");
}

string Gpio::get_and_set_value(string value) {
  if(value=="") return currentValue;
  if(gpiotype == "pwmout") {
     if(value == "ON" || value == "on") {
        set("mode", "ON");
	save();
	return currentValue;
     }
     if(value == "OFF" || value == "off") {
        set("mode", "OFF");
	save();
	return get("default_cycle");
     }
     double v = 1-hsrv::a2double(value)/100;
     dutyCycle = value;
     string cycle = get("default_cycle");
     double duty = hsrv::a2double(cycle)*v;
     string res = hsrv::int2a((int)duty);
     currentValue = res;
     add("currentValue", currentValue);
     add("dutyCycle", dutyCycle);
     save();
     return currentValue;
  }
  if(value == "ON" || value == "on") currentValue = "1";
  else if(value == "OFF" || value == "off") currentValue = "0";
  else currentValue = value;
  add("currentValue", currentValue);
  save();
  return currentValue;
}

bool Gpio::setInitialValue() {
  setCurrentValue("");
  if(get("mode") == "OFF") setCurrentValue("OFF");
  return true;
}

bool Gpio::setCurrentValue(std::string value) {
   string v = get_and_set_value(value);
   string out = getpath(value);
   FILE *chout;
   if(hsrv::debug >= 1) cout<<"WRITING: "<<out<<"="<<v<<endl;
   chout = fopen(out.c_str(), "w");
   if(chout == NULL) return false;
   fprintf(chout, "%s", v.c_str());
   fclose(chout);
   if(gpiotype=="pwmout") hsrv::publish(name, dutyCycle);
   else hsrv::publish(name, currentValue);
   return true;
}

