using namespace std;

#include "DeviceManager.h"

boost::mutex DeviceManager::mutexclusion;
MParams DeviceManager::par;

TransTab* DeviceManager::trans_tab = NULL;

bool DeviceManager::init(string file) {
   par = MParams(file);
   par.load();
   DeviceManager::trans_tab = new TransTab("trans_tab");
   return true;
}

MMessage DeviceManager::execGetSet(std::vector<std::string>& params, std::string& method, std::string msgstring) {
   MMessage answer;
   MMessage request;
   NameList items;
   string body="", itembody, response, itemstatus;
   boost::unique_lock<boost::mutex> lock(mutexclusion);
   answer.add("body", "Not Found");
   if(params.size()<4) return answer;
   items = NameTable::getCategoryDevices(params[3].substr(0,2));
   response = par.get("cat_status_answer");
   for(size_t i=0; i<items.size(); i++) {
      request.mtype="cmd";
      request.msubtype = items[i];
      request.add("url", "/DynamicPage/"+items[i]+".xml");
      itemstatus=NameTable::getstatus(request);
      itembody = par.get("dev_status_proto");
      hsrv::strReplace(itembody,"$NAME", extractValue(itemstatus, "name"));
      hsrv::strReplace(itembody,"$STATUS", remapValue(extractValue(itemstatus, "status")));
      if(body=="") body+=itembody;
        else body+=(","+itembody);
   }
   hsrv::strReplace(response,"$BODY", body);
   answer.add("body", response);
   return answer;
}

MMessage DeviceManager::execGet(std::vector<std::string>& params, std::string& method, std::string msgstring) {
   MMessage answer;
   boost::unique_lock<boost::mutex> lock(mutexclusion);
   TransEntry call = trans_tab->get(params, method);
   if(call.isnoentry()) {
        MMessage complain;
        complain.add("body", "Not Supported");
        return complain;
   }
   MMessage msg = call.rpc2cmd(params,method,msgstring);
   string body = NameTable::getstatus(msg);
   answer = mkAnswer(body, call, "GET");
   return answer;
}

MMessage DeviceManager::execPut(std::vector<std::string>& params, std::string& method, std::string msgstring) {
   MMessage answer;
   boost::unique_lock<boost::mutex> lock(mutexclusion);
   TransEntry call = trans_tab->get(params, method);
   if(call.isnoentry()) {
        MMessage complain;
        complain.add("body", "Not Supported");
        return complain;
   }
   MMessage msg = call.rpc2cmd(params,method,msgstring);
   string body = NameTable::notify(msg);
   answer = mkAnswer(body, call, "PUT");
   return answer;
}

MMessage DeviceManager::execPost(std::vector<std::string>& params, std::string& method, std::string body) {
   MMessage answer;
   boost::unique_lock<boost::mutex> lock(mutexclusion);
   TransEntry call = trans_tab->get(params, method);
   if(call.isnoentry()) {
        MMessage complain;
        complain.add("body", "Not Supported");
        return complain;
   }
   MMessage msg = call.rpc2cmd(params,method,body);
   answer.add("body", "Not Supported");
   return answer;
}

MMessage DeviceManager::execPostSet(std::vector<std::string>& params, std::string& method, std::string body) {
   boost::unique_lock<boost::mutex> lock(mutexclusion);
   boost::property_tree::ptree pt, ptdev;
   string url;
   int done = 0;
   int success = 0;
   stringstream ss;
   MMessage res;
   if(params.size() < 3) {
      res.add("body", "Not supported");
      return res;
   }
   ss << body;
   read_json(ss, pt);

   MMessage msg;
   msg.mtype = "cmd";

   ptdev = pt.get_child("devices");
   JParser tree_parse("enable_status", "name", ptdev);
   for(size_t i=0; i<tree_parse.size(); i++) {
      string status = tree_parse[i].first;
      string name = tree_parse[i].second;
      string body, itembody;
      msg.msubtype = name;
      url = par.get("set_device_status");
      hsrv::strReplace(url, "$STATUS",(status == "true" ? "ON" :"OFF")); 
      hsrv::strReplace(url, "$NAME",name); 
      msg.add("url", url);
      itembody = NameTable::notify(msg);
      done++;
      if(itembody.find("done:") != string::npos) success++;
   }
   body = par.get("set_device_cat_answer");
   if(success == done) hsrv::strReplace(body, "$TRUEFALSE", "true");
     else hsrv::strReplace(body, "$TRUEFALSE", "false");
   res.add("content", "application/json");
   res.add("body",body);
   return res;
}

MMessage DeviceManager::mkAnswer(string& body, TransEntry& call, string method) {
    MMessage answer;
    if(method == "PUT" ||method == "POST") {
       answer.add("content", "application/json");
       answer.add("body", call.xml2jsonBody(body));
    }
    else {
       answer.add("content", "application/json");
       answer.add("body", call.xml2jsonStatus(body));
    }
    return answer;
}

string DeviceManager::extractValue(string& body, string keyword) {
   size_t pos = body.find(keyword);
   if(pos == string::npos) return "";
   while(pos<body.size() && body[pos]!='=') pos++;
   while(pos<body.size() && body[pos]!='"') pos++;
   pos++;
   string res="";
   while(pos<body.size() && body[pos] != '"' && body[pos] != ',') res+=body[pos++];
   return res;
}

string DeviceManager::remapValue(string value) {
   if(value == "ON") return "true";
   if(value == "OFF") return "false";
   return value;
}
