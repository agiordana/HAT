using namespace std;

#include "PwrManager.h"

boost::mutex PwrManager::mutexclusion;
MParams PwrManager::par;
NameList PwrManager::devCategories;

bool PwrManager::init(string file) {
   par = MParams(file);
   par.load();
   devCategories.init(par.get("devCategories"),'+');
   return true;
}

MMessage PwrManager::execGet(vector<string>& params, string& method, string body) {
   vector<DeviceDescriptor> devices;
   string answerbody;
   MMessage answer;
   string device_queue;
   boost::property_tree::ptree rpt, pt;
   stringstream ss;
   stringstream answerss;
   boost::unique_lock<boost::mutex> lock(mutexclusion);
   string category=FileManager::getStem(params.back());
   device_queue = fileDownload(par.get("device_queue"));
   ss<<device_queue;
   read_json(ss,rpt);
   pt = rpt.get_child("list");
   getDevices(pt, devices, category);
   sort(devices.begin(), devices.end(), sortEval);
   answerbody=mkAnswer(devices);
   answer.add("body", answerbody);
   answer.add("content", "application/json");
   return answer;
}

bool PwrManager::sortEval(const DeviceDescriptor& a, const DeviceDescriptor& b){
   return a.priority < b.priority;
}

MMessage PwrManager::execPost(vector<string>& params, string& method, string body) {
   boost::property_tree::ptree rpt, pt;
   stringstream ss;
   stringstream answerss;
   string category = FileManager::getStem(params.back());
   MMessage answer;
   int ncmd, ndone;
   ncmd = ndone = 0;
   boost::unique_lock<boost::mutex> lock(mutexclusion);
   answer.add("content", "application/json");
   if(!devCategories.member(category)) {
      answer.add("body",par.get("post_answer_fail"));
      return answer;
   }
   ss<<body;
   read_json(ss,rpt);
   pt = rpt.get_child("devices");
   int priority=0;
   JParser tree_parse("name", "enable_status", pt);
   for(size_t i=0; i<tree_parse.size(); i++) {
      string name = tree_parse[i].first;
      string status = tree_parse[i].second;
      priority++;
      string apriority = hsrv::int2a(priority);
      ncmd++;
      if(setDeviceStatus(name, status, apriority)) ndone++;
   }
   if(ndone == ncmd) answer.add("body",par.get("post_answer_ok"));
	else answer.add("body",par.get("post_answer_fail"));
   return answer;
}
