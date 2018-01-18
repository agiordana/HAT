using namespace std;

#include "AutomatismManager.h"

boost::mutex AutomatismManager::mutexclusion;
MParams AutomatismManager::par;

bool AutomatismManager::init(string file) {
   par = MParams(file);
   par.load();
   return true;
}

MMessage AutomatismManager::execGet(vector<string>& params, string& method, string body) {
   MMessage res;
   string category;
   string rulestatus;
   string url;
   string answerbody;
   string answer;
   boost::unique_lock<boost::mutex> lock(mutexclusion);
   if(params.size()>3 && params[2] == "Programmed" && params[3].size()>3) {
     // returns the programmed automation rules status
     // expected request: /horus-ws/Automatisms/Programmed/device-category.json 
     category = params[3].substr(0,2);
     url = par.get("rule_status_req");
     rulestatus = fileDownload(url);
     answerbody = mkAnswerBody(rulestatus, category, '_');
     answer = par.get("rule_status_answer");
     hsrv::strReplace(answer,"$BODY", answerbody);
   }
   if(params.size()>3 && params[2] == "SelfManaged" && params[3].size()>3) {
     // returns the self-managed automation rules status
     // expected request: /horus-ws/Automatisms/SelfManaged/device-category.json 
     category = params[3].substr(0,2);
     url = par.get("rule_status_req");
     rulestatus = fileDownload(url);
     answerbody = mkAnswerBody(rulestatus, category, 'R');
     answer = par.get("rule_status_answer");
     hsrv::strReplace(answer,"$BODY", answerbody);
   }
   res.add("body", answer);
   res.add("content", "application/json");
   return res;
}

MMessage AutomatismManager::execPut(vector<string>& params, string& method, string msgstring) {
   MMessage res;
   res.add("content", "application/json");
   return res;
}

MMessage AutomatismManager::execPost(vector<string>& params, string& method, string body) {
   boost::unique_lock<boost::mutex> lock(mutexclusion);
   boost::property_tree::ptree pt, pta;
   string url;
   int done = 0;
   int success = 0;
   stringstream ss;
   MMessage res;
   if(params.size() < 3) {
      res.add("body", "Not supported");
      return res;
   }
   if(params.size()>3 && (params[2] == "Programmed"||params[2] == "SelfManaged") && params[3].size()>3) {
    // expected request: /horus-ws/Automatisms/Programmed/device-category.json + 'Body'
   ss << body;
   read_json(ss, pt);

   string msg;
   string itembody;
   string status, name, from, to;
   pta = pt.get_child("automatisms");
   JParser tree_parse("enable_status", "name", "start", "end",  pta);
   for(size_t i=0; i<tree_parse.size(); i++) {
      status = tree_parse[i].first;
      name = tree_parse[i].second;
      from = cvtimej2x(tree_parse[i].third);
      to = cvtimej2x(tree_parse[i].fourth);
      if(params[2] == "Programmed") msg=par.get("set_trrule_status");
	else msg=par.get("set_rule_status");
      hsrv::strReplace(msg, "$NAME",name);
      hsrv::strReplace(msg, "$STATUS",(status == "true" ? "ON" :"OFF"));
      hsrv::strReplace(msg, "$FROM", from);
      hsrv::strReplace(msg, "$TO", to);
      itembody = sendCmd(msg);
      done++;
      if(itembody.find("done") != string::npos) success++;
   }
   body = par.get("set_rule_status_answer");
   if(success == done) hsrv::strReplace(body, "$TRUEFALSE", "true");
     else hsrv::strReplace(body, "$TRUEFALSE", "false");
  }
  res.add("body",body);
  res.add("content", "application/json");
  return res;
}

