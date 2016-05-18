using namespace std;

#include "NameTable.h"

vector<AgentDesc> NameTable::agent;
map<string, size_t> NameTable::namelookup;
vector<string> NameTable::category;
boost::mutex NameTable::mutexclusion;

bool NameTable::reset() {
   string cmd;
   string devdir = hsrv::homedir + "/devices";
   boost::unique_lock<boost::mutex> lock(mutexclusion);
   for(size_t i=0; i<category.size(); i++) {
     string file = category[i];
     string destination = devdir+"/"+category[i];
     cmd = "rm -rf "+destination+"/*";
     hsrv::cmdExec(cmd);
   }
   return true;
}

bool NameTable::update(MMessage& m) {
    AgentDesc tmp;
    bool first_announce = false;
    boost::unique_lock<boost::mutex> lock(mutexclusion);
    if(m.getString("status") == "waiting") return false;
    tmp.name = m.getString("agentname");
    tmp.address = m.getString("address");
    tmp.port = m.getString("port");
    tmp.time = m.getString("time");
    if(m.getString("first_announce") == "TRUE" || m.getString("first_announce") == "true") first_announce = true;
      else first_announce = false;
    if(addifnew(tmp)||first_announce) {
        string body = getPage("/subjects/imported", tmp);
	if(body.find("unknown")!= string::npos) return false;
	   else extractCommand(agent[agent.size()-1], body);
        body = getPage("/subjects/exported", tmp);
	if(body.find("unknown")!= string::npos) return false;
	   else extractEvent(agent[agent.size()-1], body);
	mkTable();
	printTable(true);
        return true;
    }
    return false;
}

bool NameTable::extractCommand(AgentDesc& ag, string& body) {
    size_t pos = 0;
    string cmd;
    NameList subtype_type;
    while((cmd = getWord(body, "name", pos))!="" && pos != string::npos) {
	subtype_type.clear();
	subtype_type.init(cmd, '.');
	if(subtype_type.size()==2 && subtype_type[1] == "cmd") {
	   ag.command.push_back(subtype_type[0]);
	}
    } 
    return true;
}

bool NameTable::extractEvent(AgentDesc& ag, string& body) {
    size_t pos = 0;
    string cmd;
    NameList subtype_type;
    while((cmd = getWord(body, "name", pos))!="" && pos != string::npos) {
	subtype_type.clear();
	subtype_type.init(cmd, '.');
	if(subtype_type.size()==2 && (subtype_type[1] == "event"||subtype_type[1] == "program")) {
	   ag.event.push_back(subtype_type[0]);
	}
    } 
    return true;
}

string NameTable::getWord(string& body, string word, size_t& i) {
   string tmp = "";
   size_t pos=body.find(word, i);
   if(pos == string::npos) {i = pos; return "";}
   pos+= word.size();
   if(pos >= body.size()) {i = pos; return "";}
   while(pos<body.size() && body[pos] != '"') pos++;
   pos++;
   while(pos<body.size() && body[pos] != '"') tmp += body[pos++];
   i = pos;
   return tmp;
}  

bool NameTable::addifnew(AgentDesc& ag) {
   for(size_t i=0; i<agent.size(); i++) 
     if(agent[i].name == ag.name && agent[i].port == ag.port && agent[i].address == ag.address) return false;
   agent.push_back(ag);
   return true;
}

string NameTable::getBody(string& message) {
   string body = "";
   size_t pos = message.find("\r\n\r\n");
   if(pos != string::npos) body = message.substr(message.find("\r\n\r\n")+4);
   else {
      hsrv::logger->info("WRONG MESSAGE FORMAT: "+message);
   }
   return body;
}

bool NameTable::mkTable() {
   string dp = "";
   string dynamic_page;
   string name;
   size_t pos = 0;
   namelookup.clear();
   for(size_t i = 0; i<agent.size(); i++) {
      dp = getPage("/DynamicPage.xml", agent[i]);
      pos = 0;
      dynamic_page = cleanUp(dp);
      namelookup[agent[i].name] = i;
      if(isDeviceAgent(agent[i])) {
        name = getWord(dynamic_page, "name", pos);
        while((name = getWord(dynamic_page, "name", pos))!="" && pos != string::npos) {
           if(getCategory(name)!="") namelookup[name] = i;
	}
      }
   }
   mkDeviceNames();
   return true;
}

bool NameTable::printTable(bool dp) {
  map<string, size_t>::iterator it;
  if(!dp) {
   cout<<"__________"<<endl;
   for(it = namelookup.begin(); it!=namelookup.end(); it++) {
	cout<<it->first<<" "<<agent[it->second].name;
	cout<<endl;
   }
   cout<<"__________"<<endl;
   return true;
  }
  else {
    for(it = namelookup.begin(); it!=namelookup.end(); it++) hsrv::publish(it->first, agent[it->second].name);
    return true;
  }
}

bool NameTable::setDevice(MMessage& msg, string ag) {
    boost::unique_lock<boost::mutex> lock(mutexclusion);
    map<string, bool> done;
    string device;
    MMessage::iterator it;
/*
    for(it = msg.begin(); it!=msg.end(); it++) {
      device = it->first;
      if(done.count(device)==0) {
	 done[device] = true;
         if(namelookup.count(device) == 0) return false;
         vector<size_t> aglist = namelookup[device];
         string message = msg.xmlEncode(1);
         for(size_t i=0; i<aglist.size(); i++)
	    if(matchAgent(ag, i)) {
               string request = "POST / HTTP/1.1\r\nContent-Length: "+hsrv::unsigned2a(message.size())+"\r\n\r\n"+message+"\r\n\r\n";
               RpcCall call(agent[aglist[i]].port, agent[aglist[i]].address);
               string answer = call.sendStringMessage(request);
               string body = getBody(answer);
	       if(body.find("done")==string::npos) {
		  hsrv::logger->info("CMD: "+message+" fail!!!!");
		  return false;
	       }
	    }
         setDeviceStatus(device, msg.getString("device"));
      }
    }
*/
    return done.size()>0;
}

string NameTable::notify(MMessage& msg) {
    if(namelookup.count(msg.msubtype) == 0) return "Not Found";
    string message = msg.xmlEncode(1);
    string url = msg.getString("url");
    size_t agindx = namelookup[msg.msubtype];
    string request; 
    if(url == "") request = "POST / HTTP/1.1\r\nContent-Length: "+hsrv::unsigned2a(message.size())+"\r\n\r\n"+message+"\r\n\r\n";
	else request = "POST "+url+" HTTP/1.1\r\nContent-Length: "+hsrv::unsigned2a(msg.getString("body").size())+"\r\n\r\n"+msg.getString("body")+"\r\n\r\n";
    RpcCall call(agent[agindx].port, agent[agindx].address);
    string answer = call.sendStringMessage(request);
    string body = getBody(answer);
    if(body.find("done")==string::npos) hsrv::logger->info("CMD: "+message+" fail!!!!");
    return body;
}

string NameTable::getstatus(MMessage& msg) {
    if(namelookup.count(msg.msubtype) == 0) return "Not Found";
    string message = msg.xmlEncode(1);
    string url = msg.getString("url");
    size_t agindx = namelookup[msg.msubtype];
    string request; 
    if(url == "") request = "GET / HTTP/1.1\r\nContent-Length: "+hsrv::unsigned2a(message.size())+"\r\n\r\n"+message+"\r\n\r\n";
	else request = "GET "+url+" HTTP/1.1\r\nContent-Length: "+hsrv::unsigned2a(msg.getString("body").size())+"\r\n\r\n\r\n\r\n";
    RpcCall call(agent[agindx].port, agent[agindx].address);
    //cout<<"NT: "<<agent[agindx].address<<":"<<agent[agindx].port<<request<<endl;
    string answer = call.sendStringMessage(request);
    string body = getBody(answer);
    return body;
}

bool NameTable::matchAgent(string ag, size_t index) {
    if(ag == "") return true;
    if(agent[index].name == ag) return true;
    return false;
}

bool NameTable::loadCategories() {
   MParams cat("categories");
   string devdir = hsrv::homedir+"/devices";
   if(!FileManager::makeDir(devdir, true)) return false;
   cat.load();
   MParams::iterator it;
   for(it=cat.begin(); it!=cat.end(); it++) {
     string name = it->name;
     if(it->value == "ON") {
        category.push_back(it->name);
        string category = devdir+"/"+name;
        if(!FileManager::makeDir(category, true)) return false;
     }
   }
   return true;
}
      

bool NameTable::mkDeviceNames() {
   map<string, size_t >::iterator it;
   string devdir = "devices";
   NameList nametuple;
   for(size_t i=0; i<category.size(); i++) {
     string file = category[i];
     string destination = devdir+"/"+category[i];
     FileManager::makeDir(destination, true);
     for(it = namelookup.begin(); it!=namelookup.end(); it++) {
	if(getCategory(it->first) == category[i]) {
	   string devpath = destination +"/"+it->first;
	   FileManager::makeDir(devpath, true);
	   string respath = devpath+"/data";
	   string content = "{\"enable_status\":\"" + getDeviceStatus(it->first, "json") +"\"}";
	   hsrv::archive->updateResource(respath, content);
	}
     }
   }
   return true;
}

bool NameTable::setDeviceStatus(string device, string status) {
   NameList nametuple;
   string mode;
   nametuple.init(device, '_');
   if(nametuple.size()<2) return false;
   if(status == "ON" || status == "On" || status == "on" || status == "true") mode = "true";
	else mode = "false";
   string devpath = "devices/"+nametuple[0]+ "/" + device;
   if(!FileManager::isDir(devpath)) return false;
   string respath = devpath + "/data";
   string content = "{\"enable_status\":\"" + mode + "\"}";
   hsrv::archive->updateResource(respath, content);
   return true;
}

MMessage NameTable::getAgentDescription(string ag) {
   MMessage answer;
   size_t i;
   for(i=0; i<agent.size() && (agent[i].name != ag); i++);
   if(i == agent.size()) {
      answer.add("name", "none");
      return answer;
   }
   answer.add("name", agent[i].name);  
   answer.add("port", agent[i].port);  
   answer.add("address", agent[i].address);  
   return answer;
}

string NameTable::getPage(string url, AgentDesc& agent) {
   string message = "GET "+url+" HTTP/1.1\r\n\r\n\r\n";
   RpcCall call(agent.port, agent.address);
   string answer = call.sendStringMessage(message);
   string body = getBody(answer);
   return body;
}

string NameTable::getCategory(string name) {
   if(name.size() >3 && name[2]=='_') return name.substr(0,2);
   else return "";
}

string NameTable::getDeviceStatus(string name, string language) {
  string url = "/DynamicPage/"+name+"."+language;
  size_t pos = 0;
  string page = getPage(url, agent[namelookup[name]]);
  string status = getWord(page, "status", pos);
  if(language == "json") return (status == "ON" ? "true" : "false");
  else return status;
}

string NameTable::cleanUp(string& s) {
   string tmp = "";
   for(size_t i=0; i<s.size(); i++) if(s[i] != '\0' && s[i] != '\r' && s[i] != '\n') tmp += s[i];
   return tmp;
}

NameList NameTable::getCategoryDevices(string category) {
   NameList answer;
   map<string, size_t>::iterator it;
   for(it=namelookup.begin(); it!=namelookup.end(); it++) 
      if(getCategory(it->first) == category) answer.push_back(it->first);
   return answer;
}

string NameTable::forward(string& message, string ag) {
   size_t index;
   if(namelookup.count(ag)>0) index = namelookup[ag];
	else return "Not Found";
   RpcCall call(agent[index].port, agent[index].address);
   string answer = call.sendStringMessage(message);
   return answer;
}

bool NameTable::isDeviceAgent(AgentDesc& agent) {
  string name = agent.name;
  if(name.find("device") != string::npos) return true;
  else return false;
}
