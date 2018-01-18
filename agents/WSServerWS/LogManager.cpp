using namespace std;

#include "LogManager.h"

boost::mutex LogManager::mutexclusion;
MParams LogManager::par;
Translator* LogManager::translator = NULL;

bool LogManager::init(string file) {
   par = MParams(file);
   par.load();
   translator = new Translator("translator");
   return true;
}

MMessage LogManager::execPost(vector<string>& params, string& method, string body) {
   boost::property_tree::ptree pt;
   stringstream ss;
   vector<string> logData;
   stringstream answerss;
   boost::unique_lock<boost::mutex> lock(mutexclusion);
   string requrl = "/logs/"+FileManager::getStem(params.back());
   MMessage req("httprequest");
   req.add("url", requrl);
   req.add("body", mkRequest(body, params.back()));
   ss<<hsrv::archive->getSubLog(req);
   read_json(ss, pt);
   answerss<<par.get("answer_prefix");
   BOOST_FOREACH(boost::property_tree::ptree::value_type &v, pt) {
      string body=translator->log2text(v);
      logData.push_back(body);
   }
   for(size_t i=logData.size(); i>0; i--) {
      answerss<<logData[i-1]<<"\\n\\n";
   }
   answerss<<par.get("answer_suffix");
   MMessage answer;
   answer.add("body", answerss.str());
   answer.add("content", "application/json");
   return answer;
}

string LogManager::getBody(string message) {
   string body = message.substr(message.find("\r\n\r\n")+4);
   return body;
}

string LogManager::mkRequest(string& body, string& logcategory) {
   string fulllog;
   string request;
   string startdate, enddate, starttime, endtime;
   string lstartdate, lenddate;
   string lc = FileManager::getStem(logcategory);
   boost::property_tree::ptree pt;
   stringstream ss;
   ss<<body;
   read_json(ss,pt);
   startdate = pt.get<string>("start_date");
   enddate = pt.get<string>("end_date");
   starttime = pt.get<string>("start_time");
   endtime = pt.get<string>("end_time");
   fulllog = pt.get<string>("full_log");
   if(fulllog=="true") request = par.get("full_log",lc);
       else request = par.get("short_log",lc);
   lstartdate = startdate.substr(6,2)+startdate.substr(4,2)+startdate.substr(0,4);
   lenddate = enddate.substr(6,2)+enddate.substr(4,2)+enddate.substr(0,4);
   hsrv::strReplace(request,"$start_date",lstartdate);
   hsrv::strReplace(request,"$end_date",lenddate);
   hsrv::strReplace(request,"$start_time",starttime);
   hsrv::strReplace(request,"$end_time",endtime);
   return request;
}

