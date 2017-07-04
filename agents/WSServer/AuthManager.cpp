using namespace std;

#include "AuthManager.h"

boost::mutex AuthManager::mutexclusion;
MParams AuthManager::par;
MParams AuthManager::user_template;
map<string, User> AuthManager::users;

MMessage AuthManager::execSet(vector<string>& params, string& method, string body) {
   MMessage res;
   
   map<string, bool> current_users;

   boost::unique_lock<boost::mutex> lock(mutexclusion);
   boost::property_tree::ptree pt, ptu;
   stringstream ss;

   if(params.size()<3) {
      res.add("body", "Not found");
      return res;
   }
   if(params[2] == "Users.json" && body != "") {
      ss << body;
      read_json(ss, pt);

      ptu = pt.get_child("Users");
      JParser tree_parse("name", "enable_status", ptu);
      for(size_t i=0; i<tree_parse.size(); i++) {
	 string name = tree_parse[i].first;
         string status = tree_parse[i].second;
	 current_users[name] = true;
         if(users.count(name) > 0) {
	    users[name].set("enable_status", status);
	    users[name].save();
	    current_users[name] = true;
         }
	 map<string, User>::iterator it;
	 for(it = users.begin(); it!=users.end(); it++)
	   if(current_users.count(it->second.get("username")) == 0) {
		current_users[it->second.get("username")] = false;
	   }
	 map<string, bool>::iterator nit;
         for(nit = current_users.begin(); nit!=current_users.end(); nit++)
	    if(!nit->second) deleteUser(nit->first);
      }
      res.add("body", hsrv::filterChar(par.get("wr_users_answer")));
   }
   if(params.size() == 4 && params[2] == "Users") {
      NameList name;
      name.init(params[3],'.');
      addUser(name[0], body, res);
   }
   else res.add("body", "Not found");
   return res;
}

