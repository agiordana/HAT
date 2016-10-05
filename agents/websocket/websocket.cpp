#include "agentlib.h"
#define CONFIG "/home/horus/CONFIG"
#define BIN "/home/horus/bin/websocketd"
using namespace std;

int main() {
   string conf = CONFIG;
   string cmd;
   MParams global;
   string path = conf + "/websocket/global.xml";
   global.xmlLoad(path);
   cmd = global.get("cmd") + "&";
   system(cmd.c_str());
   return 0;
 }
