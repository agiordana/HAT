#include "agentlib.h"
#define CONFIG "/home/horus/CONFIG"
#define HOME "/home/horus/horus/horus_home"
#define HOMEBU "/home/horus/horus_home"
#define BIN "/home/horus/bin"
using namespace std;

int main() {
   string conf = CONFIG;
   string home = HOME;
   string homebu = HOMEBU;
   string cmd;
   NameList filelist;
   NameList cmdlist;
   MParams init;
   setof<MAttribute>::iterator it;
   setof<MAttribute> initprg;
   string initpath = conf + "/bbdevices/init.xml";
   if(FileManager::isDir(home)) {
      cmd = "rm -f " + home + "/*/.lock";
      hsrv::cmdExec(cmd);
   }
   if(FileManager::isDir(homebu)) {
      cmd = "rm -f " + homebu + "/*/.lock";
      hsrv::cmdExec(cmd);
   }
   FileManager::dirList(conf, filelist);
   for(size_t i=0; i<filelist.size(); i++) {
      MParams global;
      string path = conf + "/" + filelist[i] + "/global.xml";
      global.xmlLoad(path);
      string cmd = string(BIN) + "/" + global.get("agenttype") + " " + conf + "/" + filelist[i] + "&";
      cmdlist.push_back(cmd);
   }
   sleep(60);
   init.xmlLoad(initpath);
   initprg = init.extractCategory("script");
   for(it=initprg.begin(); it!=initprg.end(); it++) {
      cout<< it->value<<endl;
      hsrv::cmdExec(it->value);
      sleep(1);
   }
   for(size_t i=0; i<cmdlist.size(); i++) {
      cout<<cmdlist[i]<<endl;
      system(cmdlist[i].c_str());
      sleep(1);
   }
   return 0;
 }
