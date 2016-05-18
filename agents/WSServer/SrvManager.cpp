using namespace std;

#include "SrvManager.h"

boost::mutex SrvManager::mutexclusion;
MParams SrvManager::par;

bool SrvManager::init(string file) {
   par = MParams(file);
   par.load();
   return true;
}

MMessage SrvManager::execGet(vector<string>& params, string& method, string body) {
   stringstream ss;
   stringstream answerss;
   string macaddr = FileManager::getStem(params.back());
   string current_time = "20" + hsrv::getasciitimecompact();
   string last_time = macLastTime(macaddr);
   MMessage answer;
   if(last_time=="") {
        answer.add("body", par.get("answer_complain"));
        answer.add("content","application/json");
	return answer;
   }
   string requrl = "/logs/"+par.get("push_log");
   string request = mkRequest(last_time,current_time);
   MMessage req("httprequest");
   boost::unique_lock<boost::mutex> lock(mutexclusion);
   req.add("url", requrl);
   req.add("body", request);
   answerss<<par.get("answer_prefix");
   answerss<<hsrv::archive->getSubLog(req);
   answerss<<par.get("answer_postfix");
   answer.add("body",answerss.str());
   answer.add("content","application/json");
   return answer;
}

MMessage SrvManager::execPut(vector<string>& params, string& method, string body) {
   string macaddr = FileManager::getStem(params.back());
   boost::unique_lock<boost::mutex> lock(mutexclusion);
   string time = "20" + hsrv::getasciitimecompact();
   macTimeUpdate(macaddr);
   MMessage answer;
   answer.add("body", par.get("mac_post_answer"));
   answer.add("content", "application/json");
   return answer;
}

string SrvManager::getBody(string message) {
   string body = message.substr(message.find("\r\n\r\n")+4);
   return body;
}

string SrvManager::mkRequest(string& starttime, string& endtime) {
   string request;
   string startdate, enddate, lstarttime, lendtime;
   request = par.get("log_request");
   startdate = starttime.substr(6,2)+starttime.substr(4,2)+starttime.substr(0,4);
   enddate = endtime.substr(6,2)+endtime.substr(4,2)+endtime.substr(0,4);
   lstarttime = starttime.substr(8,2) +":"+starttime.substr(10,2);
   lendtime = endtime.substr(8,2) +":"+endtime.substr(10,2);
   hsrv::strReplace(request,"$start_date",startdate);
   hsrv::strReplace(request,"$end_date",enddate);
   hsrv::strReplace(request,"$start_time",lstarttime);
   hsrv::strReplace(request,"$end_time",lendtime);
   return request;
}

bool SrvManager::macTimeUpdate(string mac) {
   string time = "20" + hsrv::getasciitimecompact();
   string file = hsrv::homedir+"/logs/"+mac;
   FileManager::makeDir(file, true);
   file += "/data";
   ofstream out(file.c_str());
   out<<"{\"time\":\""<<time<<"\"}"<<endl;
   out.close();
   return true;
}

string SrvManager::macLastTime(string mac) {
   boost::property_tree::ptree pt;
   string file = hsrv::homedir+"/logs/"+mac+"/data";
   if(!FileManager::isFile(file)) return "";
   ifstream in(file.c_str());
   read_json(in,pt);
   return pt.get<string>("time");
}
