using namespace std;

#include "AuthManager.h"

bool AuthManager::init(string file) {
   par = MParams(file);
   par.load();
   vector<string> filelist;
   string username;
   string confpath = hsrv::homedir +"/users";
   if(FileManager::isDir(confpath)) {
      FileManager::fileList(confpath, filelist);
      for(size_t i=0; i<filelist.size(); i++) {
         username = FileManager::getStem(filelist[i]);
         users[username] = User("users", username);
      }
   }

   confpath = hsrv::configdir + "/users";
   if(FileManager::isDir(confpath)) {
      FileManager::fileList(confpath, filelist);
      for(size_t i=0; i<filelist.size(); i++) {
         username = FileManager::getStem(filelist[i]);
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

