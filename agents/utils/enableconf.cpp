#define CONFFILE "/home/horus/CONFIGURE"
#include "agentlib.h"

using namespace std;

int main() {
  string time;
  ofstream out;
  out.open(CONFFILE, ios::trunc);
  out<<hsrv::double2a(hsrv::gettime())<<endl;
  out.close();
  return 0;
}
