using namespace std;

#include "TransTab.h"

TransEntry::TransEntry(std::string dir, std::string file): MParams::MParams(dir, file) {
    load();
    prefix.init(get("prefix"), '/');
    method = get("method");
    category = get("class");
    nameindex = 0;
    agentindex = 0;
    for(size_t i=0; i<prefix.size(); i++)
       if(prefix[i]=="$NAME"||prefix[i]=="$NAME.json") nameindex = i;
    for(size_t i=0; i<prefix.size(); i++)
       if(prefix[i]=="$AGENT") agentindex = i;
    for(size_t i=0; i<prefix.size(); i++)
       if(prefix[i]=="$VALUE"||prefix[i]=="$VALUE.json") valueindex = i;
}

bool TransEntry::match(vector<string>& params, string& mtd) {
    if(params.size()!=prefix.size()) return false;
    if(method!=mtd) return false;
    for(size_t i=0; i<prefix.size(); i++){
       if((params[i] != prefix[i]) && (prefix[i] != "$NAME") && (prefix[i] != "$AGENT")
		&& (prefix[i] != "$VALUE") && (prefix[i] != "$VALUE.json") && (prefix[i] != "$NAME.json")) return false;
       if(prefix[i] == "$NAME" ||prefix[i] == "$NAME.json") {
           size_t pos = params[i].find(category);
           if(pos != 0) return false;
       }
    }
    return true;
}

string TransEntry::getName(vector<string>& params) {
   if(params.size() > nameindex && nameindex >0) {
	string val = "";
	for(size_t i=0; i<params[nameindex][i] && params[nameindex][i] != '.'; i++) val += params[nameindex][i];
	return val;
   }
   else return "";
}

string TransEntry::getAgent(vector<string>& params) {
   if(params.size() > agentindex && agentindex >0) return params[agentindex];
   else return "";
}

string TransEntry::getValue(vector<string>& params) {
   if(params.size() > valueindex && valueindex >0) {
        string val = "";
        for(size_t i=0; i<params[valueindex][i] && params[valueindex][i] != '.'; i++) val += params[valueindex][i];
        return replaceSpaces(val);
   }
   else return "";
}

string TransEntry::replaceSpaces(string& v) {
   string res = "";
   for(size_t i=0; i<v.length(); i++) 
      if(v[i] == ' ') res += '+';
        else res += v[i];
   return res;
}

string TransEntry::convertValue(string& v) {
   NameList value;
   string res;
   value.init(v,'+');
   for(size_t i=0; i<value.size(); i++) {
      if(res != "") res+= '+';
      if(value[i] == "ON" || value[i] == "1") res+="true";
      else if(value[i] == "OFF" || value[i] == "0") res+="false";
      else res+=value[i];
   }
   return res;
}


string TransEntry::getEvent(vector<string>& params) {
   if(params.size() <= nameindex) return ""; 
   string name = params[nameindex];
   string event = get("event");
   if(event == "") return name;
   hsrv::strReplace(event,"$NAME",name);
   return event;
}

MMessage TransEntry::rpc2cmd(vector<string>& params, string& method, string& body) {
    if(isnoentry()) return MMessage("NULL");
    MMessage tmp;
    NameList todo;
    string url = get("url");
    if(url != "") {
       hsrv::strReplace(url, "$VALUE", getValue(params));
       hsrv::strReplace(url, "$NAME", getName(params));
       tmp.add("url", url);
       if(body!="") tmp.add("body", body);
    }
    else {
      todo.init(get("cmd"),'=');
      if(todo.size()<2)return MMessage("NULL");
      tmp.add(todo[0],nv2av(params, todo[1]));
    }
    tmp.mtype = "cmd";
    tmp.msubtype = getName(params);
    if(tmp.msubtype == "") tmp.msubtype = get("cmdname");
    return tmp;
}

string TransEntry::nv2av(vector<string>& params, string nv) {
    if(nv=="$VALUE") return getValue(params);
    else return nv;
}

string TransEntry::xml2jsonBody(string& msg) {
   string body = get("answer");
   if(msg.find("done") != string::npos) hsrv::strReplace(body,"$TRUEFALSE","true");
      else hsrv::strReplace(body,"$TRUEFALSE","false");
   string status = extractValue(msg,"done:value");
   hsrv::strReplace(body,"$STATUS",status);
   string color = extractValue(msg, "color");
   hsrv::strReplace(body,"$COLOR",color);
   return body;
}

string TransEntry::xml2jsonStatus(string& msg) {
   string body = get("answer");
   hsrv::strReplace(body,"$NAME",extractValue(msg,"name"));
   string value = extractValue(msg,"value");
   string category = get("class");
   if(category != "SH" && category != "DI" && category != "RG") {
      string newval = convertValue(value);
      hsrv::strReplace(body,"$TRUEFALSE",newval);
   }
   else {
      if(category == "SH") {
 	if(value == "UP") hsrv::strReplace(body,"$STATUS","up");
	else if(value == "DOWN") hsrv::strReplace(body,"$STATUS","down");
	else hsrv::strReplace(body,"$STATUS","off");
      }
      else {
	hsrv::strReplace(body,"$STATUS",value);
      }
   }
   string color = extractValue(msg, "color");
   hsrv::strReplace(body,"$COLOR",color);
   return body;
}

string TransEntry::extractValue(string& body, string keyword) {
   size_t pos = body.find(keyword);
   if(pos == string::npos) return "";
   while(pos<body.size() && body[pos]!='=') pos++;
   while(pos<body.size() && (body[pos]=='"'||body[pos]==' '||body[pos]=='=')) pos++;
   string res="";
   while(pos<body.size() && body[pos] != '"' && body[pos] != ',') res+=body[pos++];
   return res;
}

bool TransEntry::print() {
   cout<<"____________"<<endl;
   for(size_t i=0; i<prefix.size(); i++) cout<<" "<<prefix[i];
   cout<<endl<<method<<" "<<nameindex<<endl;
   cout<<"____________"<<endl;
   return true;
}

TransTab::TransTab(string dir) {
    string path = hsrv::configdir + "/"+dir;
    if(!FileManager::isDir(path)) return;
    vector<string> filelist;
    FileManager::fileList(path,filelist);
    for(size_t i=0; i<filelist.size(); i++) {
        TransEntry tmp(dir, FileManager::getStem(filelist[i]));
        push_back(tmp);
    }
    
}

TransEntry TransTab::get(vector<string>& params, string& method) {
    TransEntry noentry;
    for(size_t i=0; i<size(); i++)
       if(at(i).match(params, method)) return at(i);
    return noentry;
}

