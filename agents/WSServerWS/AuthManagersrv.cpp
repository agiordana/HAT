using namespace std;

#include "AuthManager.h"

bool AuthManager::addUser(string name, string body, MMessage& res) {
   boost::unique_lock<boost::mutex> lock(mutexclusion);
   boost::property_tree::ptree pt, ptu;
   stringstream ss;
   string answer = par.get("add_user_answer");
   if(users.count(name) > 0) {
      hsrv::strReplace(answer,"$UPLOAD", "false");
      hsrv::strReplace(answer,"$UNIVOCALNAME", "false");
      hsrv::strReplace(answer,"$UNIVOCALPASSWD", "false");
      res.add("body", hsrv::filterChar(answer));
      return false;
   }
   users[name] = User("users", name);
   for(size_t i=0; i<user_template.size(); i++)
      users[name].add(user_template[i].name, user_template[i].category, user_template[i].value);
   users[name].add("username", name);
   ss << body;
   read_json(ss, pt);
   users[name].add("passwd", pt.get<string>("password"));
   users[name].add("Administration", pt.get<string>("administration_access"));
   ptu = pt.get_child("alarm_operations");
   JParser tree_parse("name", "selection_status", ptu);
   for(size_t i=0; i<tree_parse.size(); i++) {
      string opname = tree_parse[i].first;
      string selstatus = tree_parse[i].second;
      users[name].setField(opname, selstatus);
   }
   users[name].save();
   hsrv::strReplace(answer,"$UPLOAD", "true");
   hsrv::strReplace(answer,"$UNIVOCALNAME", "true");
   hsrv::strReplace(answer,"$UNIVOCALPASSWD", (isUniquePasswd(name, users[name].get("passwd")) ? "true" : "false"));
   res.add("body", hsrv::filterChar(answer));
   
   return true;
}

bool AuthManager::isUniquePasswd(string name, string passwd) {
   map<string, User>::iterator it;
   for(it = users.begin(); it != users.end(); it++)
      if(it->first != name && it->second.get("passwd") == passwd) return false;
   return true;
}

