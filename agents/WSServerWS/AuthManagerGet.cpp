using namespace std;

#include "AuthManager.h"

MMessage AuthManager::execGet(vector<string>& params, string& method, string body) {
   MMessage res;
   boost::unique_lock<boost::mutex> lock(mutexclusion);
   string answer = "{\"response\":\n\t{\"message\":\"ok\",";
   if(params.size() <3) res.add("body", "Not found");
   else if(params.size() == 3) {
      map<string, User>::iterator it;
      for(it=users.begin(); it!=users.end(); it++) {
	 if(it==users.begin()) answer += "\"users\": [\n\t\t{\"name\":\"";
	    else answer += ",\n\t\t{\"name\":\"";
	 answer += it->first;
	 answer += "\",\"enable_status\":\"";
	 answer += it->second.get("enable_status");
	 answer += "\"}";
      }
      answer += "\n\t\t]\n\t}\n}\n";
      res.add("body", answer);
   }
   else if(params.size() == 4) {
      NameList username;
      username.init(params[3],'.');
      string name=username[0];
      answer += "\"password\":\"";
      answer += users[name].get("passwd");
      answer += "\",\n\t\t\"administration_access\":\"";
      answer += users[name].get("Administration");
      answer += "\",\n\t\t\"Alarm_operations\": [\n\t\t\t";
      answer += "{\"Manual\":\"";
      answer += users[name].get("Alarm_manual");
      answer += "\"},\n\t\t\t{\"Auto\":\"";
      answer += users[name].get("Alarm_auto");
      answer += "\"},\n\t\t\t{\"Zones\":\"";
      answer += users[name].get("Alarm_Zones");
      answer += "\"},\n\t\t\t{\"On/OFF\":\"";
      answer += users[name].get("Alarm_OFF");
      answer += "\"}\n\t\t\t]\n\t}\n}\n";
      res.add("body", answer);
   }
   return res;
}

