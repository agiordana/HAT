#include "agentlib.h"
#define BIN "/home/horus/bin/websocketd"
using namespace std;

string mkPort(string p) {
  unsigned wsport = hsrv::a2unsigned(p);
  wsport++;
  return hsrv::unsigned2a(wsport);
}

int main(int argc, char *argv[]) {
   string conf, tconf;
   string cmd;
   string lport;
   string lwsport;
   string rport;
   string rwsport;
   string host;
   string user;
   MParams wsglobal;
   if(argc < 2) {
      cout << "Usage: sshtunnel <confdir> [start|halt|#port] [host] [user]" << endl;
      return 0;
   }
   if(!hsrv::load(argv[0],argv[1])) {
       cout << "directory: " << argv[1] << " not found" << endl;
       return 0;
   }
   if(hsrv::configdir[hsrv::configdir.length()-1] == '/') hsrv::configdir.erase(hsrv::configdir.end());
   if(!FileManager::makeDir(hsrv::homedir, true)) {
        return 1;
   }
   conf = FileManager::getRoot(hsrv::configdir);
   string wsserverpath = conf + "/wsserver/agentnet.xml";
   wsglobal.xmlLoad(wsserverpath);
   MParams tunnel("tunnel");
   tunnel.load();
   MParams global("global");
   global.load();
   cmd = global.get("cmd");
   lport = wsglobal.get("port");
   lwsport = mkPort(lport);
   if(argc>2){
	if(string(argv[2]) == "halt") {
	   cmd = global.get("haltAutossh");
	   hsrv::cmdExec(cmd);
	   cmd = global.get("haltSsh");
	   hsrv::cmdExec(cmd);
	   tunnel.add("status", "OFF");
	   tunnel.save();
	   return 0;
	}
	if(string(argv[2]) == "start" && tunnel.get("status") == "OFF") {
	   tunnel.add("status", "ON");
	   tunnel.save();
	   return 0;
	}
   }
   if(tunnel.get("status") == "OFF") return 0;
   if(argc>2) rport = string(argv[2]);
   else rport = tunnel.get("rport");
   if(rport=="") rport = lport;
   tunnel.add("rport", rport);
   rwsport = mkPort(rport);
   if(argc>3) host = string(argv[3]);
   else host = tunnel.get("host");
   if(argc>4) user = string(argv[4]);
   else user = tunnel.get("user");
   tunnel.add("host", host);
   tunnel.add("user", user);
   tunnel.save();
   hsrv::strReplace(cmd, "$LPORT", lport);
   hsrv::strReplace(cmd, "$RPORT", rport);
   hsrv::strReplace(cmd, "$HOST", host);
   hsrv::strReplace(cmd, "$USER", user);
   hsrv::cmdExec(cmd);
   cmd = global.get("cmd");
   hsrv::strReplace(cmd, "$LPORT", lwsport);
   hsrv::strReplace(cmd, "$RPORT", rwsport);
   hsrv::strReplace(cmd, "$HOST", host);
   hsrv::strReplace(cmd, "$USER", user);
   hsrv::cmdExec(cmd);
   return 0;
 }
