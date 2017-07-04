using namespace std;

#include "agentlib.h"
#include <boost/thread.hpp>
#include <sys/wait.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include "Translator.h"


int main (int argc, char * const argv[]) {
  int status = 0;
  boost::property_tree::ptree pt;
  stringstream ss;
  string url = "/DynamicPage.json";
  pid_t process;
  if(argc < 2) {
    cout << "Usage: test <device>" << endl;
    return 0;
  }
  hsrv::homedir = "/home/horus/horus/horus_home/wsserver";
  hsrv::configdir = "/home/horus/CONFIG/wsserver";
  read_json(ss, pt);
  Translator translator("transtab");
  
  BOOST_FOREACH(boost::property_tree::ptree::value_type &v, pt) {
     string body=translator.log2text(v);
     cout<<body<<endl;
  }
  return 0;
}
