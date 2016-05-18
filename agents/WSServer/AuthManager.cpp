using namespace std;

#include "AuthManager.h"

boost::mutex AuthManager::mutexclusion;
MParams AuthManager::par;
MParams AuthManager::user_template;
map<string, User> AuthManager::users;

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

bool AuthManager::init(string file) {
   par = MParams(file);
   par.load();
   vector<string> filelist;
   string username;
   string confpath = hsrv::homedir +"/users";
cout<<confpath<<endl;
   if(FileManager::isDir(confpath)) {
      FileManager::fileList(confpath, filelist);
      for(size_t i=0; i<filelist.size(); i++) {
         username = FileManager::getStem(filelist[i]);
         users[username] = User("users", username);
      }
   }

   confpath = hsrv::configdir + "/users";
cout<<confpath<<endl;
   if(FileManager::isDir(confpath)) {
      FileManager::fileList(confpath, filelist);
      for(size_t i=0; i<filelist.size(); i++) {
         username = FileManager::getStem(filelist[i]);
cout<<"username: "<<username<<endl;
	 if(username == "user_template") user_template = User("users", username);
         else if(users.count(username) == 0)
            users[username] = User("users", username);
      }
   }
   printUsers();  
   map<string, User>::iterator it;
   for(it=users.begin(); it!=users.end(); it++) {
      it->second.save();
   }
   return true;
}

MMessage AuthManager::execSet(vector<string>& params, string& method, string body) {
   MMessage res;
   
   map<string, bool> current_users;

   boost::unique_lock<boost::mutex> lock(mutexclusion);
   boost::property_tree::ptree pt;
   stringstream ss;

   if(params.size()<3) {
      res.add("body", "Not found");
      return res;
   }
   if(params[2] == "Users.json" && body != "") {
      ss << body;
      read_json(ss, pt);

      BOOST_FOREACH(boost::property_tree::ptree::value_type &v, pt.get_child("Users")) {
	 string name = v.second.get<string>("name");
         string status = v.second.get<string>("enable_status");
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

bool AuthManager::deleteUser(string name) {
   if(name == "admin") return false;
   map<string, User>::iterator it;
   it = users.find(name);
   users.erase(it);
   return true;
}

bool AuthManager::printUsers() {
   map<string, User>::iterator it;
   for(it = users.begin(); it!=users.end(); it++) it->second.print();
   return true;
}

bool AuthManager::addUser(string name, string body, MMessage& res) {
   boost::unique_lock<boost::mutex> lock(mutexclusion);
   boost::property_tree::ptree pt;
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
   BOOST_FOREACH(boost::property_tree::ptree::value_type &v, pt.get_child("alarm_operations")) {
      string opname = v.second.get<string>("name");
      string selstatus = v.second.get<string>("selection_status");
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

MMessage AuthManager::execCheck(vector<string>& params, string& method, string bdy) {
   MMessage res;
   
   boost::unique_lock<boost::mutex> lock(mutexclusion);

   if(params.size() != 4) {
      res.add("body", "Not found");
      return res;
   }
   string answer = getUserOf(params[2], params[3]);
   string body = par.get("passwd_check_answer");

   if(answer != "") hsrv::strReplace(body, "$USERNAME", answer);
     else body = "Forbidden";
   res.add("body", hsrv::filterChar(body));
   return res;
}

string AuthManager::getUserOf(string capability, string pwd) {
   map<string, User>::iterator it;
   NameList passwd;
   passwd.init(pwd,'.');
   for(it=users.begin(); it!=users.end(); it++)
     if(it->second.get("passwd") == passwd[0] && it->second.getField(capability) == "true")
	return it->first;
   return "";
}
