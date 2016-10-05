#include "GuiLib.h"

using namespace std;

DeviceHolder::DeviceHolder(string name) {
  device = name;
  NameList catname;
  catname.init(name, '_');
  string file = hsrv::configdir + "/guitemplates/" + catname[0] + "_bholder.html";
  description = getProto(file);
  while(hsrv::strReplace(description,"$CAT", catname[0]));
  string devname = "";
  for(size_t i=1; i<catname.size(); i++) {
	if(i==1) devname = catname[i];
        else {
	   devname += "_";
           devname += catname[i];
	}
  }
  while(hsrv::strReplace(description,"$NAME",devname));
}

string DeviceHolder::describeHTML() {
  return description;
}

string DeviceHolder::getProto(string source) {
   string proto = "";
   ifstream protosource;
   protosource.open(source.c_str());
   if(!protosource.is_open()) return proto;
   char ch;
   while((ch=protosource.get())&&!protosource.eof()) proto+=ch;
   protosource.close();
   return proto;
}


BarHolder::BarHolder(string area) {
   string file = hsrv::configdir +"/guitemplates/barholder.html";
   description = getProto(file);
   while(hsrv::strReplace(description, "$AREA", area));
}

bool BarHolder::add(string device) {
   (*this).push_back(DeviceHolder(device));
   return true;
}

string BarHolder::describeHTML() {
   string dev = "";
   for(size_t i=0; i<size(); i++) dev+=(*this)[i].describeHTML();
   hsrv::strReplace(description,"$DEVICES", dev);
   return description;
}

string BarHolder::getProto(string source) {
   string proto = "";
   ifstream protosource;
   protosource.open(source.c_str());
   if(!protosource.is_open()) return proto;
   char ch;
   while((ch=protosource.get())&&!protosource.eof()) proto+=ch;
   protosource.close();
   return proto;
}

