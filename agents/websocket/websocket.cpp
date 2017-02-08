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
   MParams global;
   MParams wsglobal;
   string path = conf + "/websocket/global.xml";
   string wsserverpath = conf + "/wsserver/agentnet.xml";
   global.xmlLoad(path);
   wsglobal.xmlLoad(wsserverpath);
   cmd = global.get("cmd") + "&";
   wsport = mkPort(wsglobal.get("port"));
   hsrv::strReplace(cmd, "$PORT", wsport);
cout<<cmd<<endl;
   system(cmd.c_str());
   return 0;
 }
