#include "NameService.h"

using namespace std;

vector<string> NameService::getNames(std::string domain, std::string category) {
   vector<string> tmp;
   map<string, bool> name_tab;
   tmp.clear();
   name_tab.clear();
   string request;
   request = "GET /DynamicPage.xml HTTP/1.1\r\n\r\n\r\n";
   string wsserver = AgentNet::getWSServer();
   string tofind;
   string name = "";
   size_t pos = 0;
   if(wsserver == "") return tmp;
   NameList addr;
   addr.init(wsserver, ':');
   RpcCall call(addr[1], addr[0]);
   string answer = call.sendStringMessage(request);
   if(domain=="devices") tofind = domain;
   else tofind = "agents";
   while((name = getName(answer, tofind, category, pos)) != "") 
      if(name_tab.count(name) == 0) {
	  tmp.push_back(name);
	  name_tab[name] == true;
      }
   return tmp;
}
 
string NameService::getName(string& src, string domain, string category, size_t& pos) {
   size_t newpos;
   string name;
   string namecat;
   if(src == ""||pos>src.size()) return "";
   while((newpos = src.find("item", pos)) != string::npos) {
     pos = newpos+4;
     name = extractValue(src, "name", pos);
     if(domain == "devices" && name.size()>2 && name[2]=='_') {
        namecat = name.substr(0,2);
        if(category == "" || category == "*" || category == namecat) return name;
     }
     else {
       if(domain != "devices" && (name.size()<3 || name[2] != '_')) return name;
     }
   }
   return "";
}

bool NameService::checkName(string domain, string name) {
   string request = "GET /DynamicPage/" + name +".xml HTTP/1.1\r\n\r\n\r\n";
   string wsserver = AgentNet::getWSServer();
   if(wsserver == "") return false;
   NameList addr;
   addr.init(wsserver, ':');
   RpcCall call(addr[1], addr[0]);
   string answer = call.sendStringMessage(request);
   if(answer.find(name, 0)!=string::npos) return true;
   else return false;
}

string NameService::extractValue(string& body, string keyword, size_t sp) {
   size_t pos = body.find(keyword, sp);
   if(pos == string::npos) return "";
   while(pos<body.size() && body[pos]!='=') pos++;
   while(pos<body.size() && body[pos]!='"') pos++;
   pos++;
   string res="";
   while(pos<body.size() && body[pos] != '"' && body[pos] != ',') res+=body[pos++];
   return res;
}

