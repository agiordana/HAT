using namespace std;

#include "AuthManager.h"

User::User(string dir, string file): MParams::MParams(dir, file) {
   load();
}

bool User::print() {
   cout<<"_____________________________________"<<endl;
   cout<<"usernaname: "<<get("username")<<endl;
   cout<<"passwd: "<<get("passwd")<<endl;
   cout<<"enable_status: "<<get("enable_status")<<endl;
   cout<<"Administration: "<<get("Administration")<<endl;
   cout<<"Alarm_manual: "<<get("Alarm_manual")<<endl;
   cout<<"Alarm_auto: "<<get("Alarm_auto")<<endl;
   cout<<"Alarm_Zones: "<<get("Alarm_Zones")<<endl;
   cout<<"Alarm_OFF: "<<get("Alarm_OFF")<<endl;
   return true;
}

string User::getField(string field) {
   string val = get(field);
   if(val != "") return val;
   else return get(get(field, "alias"));
}

bool User::setField(string field, string value) {
   if(value != "true" && value != "false") return false;
   if(get(field) == "") {
      set(field, value);
      return true;
   }
   else {
      string name = get(field, "alias");
      if(name != "") {
	 set(name, value);
	 return true;
      }
      return false;
   }
}

