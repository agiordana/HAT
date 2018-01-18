using namespace std;

#include "AuthManager.h"

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
