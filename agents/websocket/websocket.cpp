#include "agentlib.h"
#define CONFIG "/home/horus/CONFIG"
#define BIN "/home/horus/bin/websocketd"
using namespace std;

string mkPort(string p) {
  unsigned wsport = hsrv::a2unsigned(p);
  wsport++;
  return hsrv::unsigned2a(wsport);
}

int main() {
   string conf = CONFIG;
   string cmd;
   string port;
   string wsport;
   string home1;
   string home2;
   MParams global;
   MParams wsglobal;
   MParams wsnet;
   string path = conf + "/websocket/global.xml";
   string wsservernet;
   string wsserverglobal;
   string wsdir = conf + "/wsserver";
   string wsdirws = conf + "/wsserverws";
   if(FileManager::isDir(wsdir)) {
	wsservernet = conf + "/wsserver/agentnet.xml";
        wsserverglobal = conf + "/wsserver/global.xml";
   }
   else if(FileManager::isDir(wsdirws)) {
        wsservernet = conf + "/wsserverws/agentnet.xml";
        wsserverglobal = conf + "/wsserverws/global.xml";
   }
   else return 0;
   global.xmlLoad(path);
   wsnet.xmlLoad(wsservernet);
   wsglobal.xmlLoad(wsserverglobal);
   cmd = global.get("cmd") + "&";
   home1 = wsglobal.get("homedir");
   home2 = wsglobal.get("emergency_homedir");
   if(FileManager::isDir(home1)) hsrv::strReplace(cmd,"$HOME",home1);
     else hsrv::strReplace(cmd,"$HOME",home2);
   wsport = mkPort(wsnet.get("port"));
   hsrv::strReplace(cmd, "$PORT", wsport);
   system(cmd.c_str());
   return 0;
 }
