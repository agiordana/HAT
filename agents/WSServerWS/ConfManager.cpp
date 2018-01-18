using namespace std;

#include "ConfManager.h"

boost::mutex ConfManager::mutexclusion;
MParams ConfManager::par;

bool ConfManager::init(string file) {
   par = MParams(file);
   par.load();
   return true;
}

MMessage ConfManager::execPut(vector<string>& params, string& method, string body) {
   string target = FileManager::getStem(params.back());
   boost::unique_lock<boost::mutex> lock(mutexclusion);
   MMessage answer;
   if(target == "GUI") {
     map< string, BarHolder* > barholder;
     string guitemplates = hsrv::configdir + "/guitemplates";
     string gui_proto = guitemplates + "/gui-proto.html";
     string gui=getProto(gui_proto);
     boost::property_tree::ptree pt, rpt;
     std::stringstream ss;
     ss<<body;
     read_json(ss,rpt);
     pt = rpt.get_child("areas");
     JParser tree_parse("name", "area", pt);
     for(size_t i; i<tree_parse.size(); i++) {
       string dev = tree_parse[i].first;
       string area = tree_parse[i].second;
       if(barholder.count(area) == 0) barholder[area] = new BarHolder(area);
       barholder[area]->add(dev);
     }
     string description = "";
     map<string, BarHolder*>::iterator it;
     for(it=barholder.begin(); it!=barholder.end(); it++) description += it->second->describeHTML();
     hsrv::strReplace(gui, "$AREAS", description);
     description = mkButtons(barholder);
     hsrv::strReplace(gui, "$HBUTTONS", description);
     description = mkShowfun(barholder);
     hsrv::strReplace(gui, "$SHOWFUNCTION", description);
     description = mkDevList(barholder);
     hsrv::strReplace(gui, "$DEVDESCRIPTION", description);
     description = mkAreaList(barholder);
     hsrv::strReplace(gui, "$AREALIST", description);
     string destination = hsrv::configdir + "/doc/gui.html";
     FileManager::saveFile(destination, gui);
     answer.add("body", par.get("mkgui_answer"));
   }
   else if(target=="TunnelSTART") {
     // the ssh tunnel starts
     // first the tunnel is halted if active
     string cmd = par.get("tunnelhalt");
     hsrv::cmdExec(cmd);
     // then is enabled again
     cmd = par.get("tunnelenable");
     hsrv::cmdExec(cmd);
     sleep(3);
     // finally it is started with the new parameters
     boost::property_tree::ptree pt;
     std::stringstream ss;
     ss<<body;
     read_json(ss,pt);
     cmd = par.get("tunnelstart");
     string port = pt.get<string>("rport");
     string host = pt.get<string>("host");
     string user = pt.get<string>("user");
     if(port != "") {
        cmd += (" "+port);
        if(host != "") {
           cmd += (" "+host);
           if(user != "") cmd+= (" "+user);
         }
     }
     hsrv::cmdExec(cmd);
     answer.add("body", par.get("mkok_answer"));
     answer.add("content", "application/json");
   }
   else if(target=="TunnelHALT") {
     // the ssh tunnel stops
     string cmd = par.get("tunnelhalt");
     hsrv::cmdExec(cmd);
     answer.add("body", par.get("mkok_answer"));
     answer.add("content", "application/json");
   }
   else {
     answer.add("body", par.get("fail_answer"));
     answer.add("content", "application/json");
   }
   return answer;
}

string ConfManager::getBody(string message) {
   string body = message.substr(message.find("\r\n\r\n")+4);
   return body;
}

string ConfManager::getProto(string source) {
   string proto = "";
   ifstream protosource;
   protosource.open(source.c_str());
   if(!protosource.is_open()) return proto;
   char ch;
   while((ch=protosource.get())&&!protosource.eof()) proto+=ch;
   protosource.close();
   return proto;
}

string ConfManager::mkButtons(map<string, BarHolder*>& areas) {
  string buttons = "";
  map<string, BarHolder*>::iterator it;
  for(it = areas.begin(); it!=areas.end(); it++) buttons+="<button id=\"show_"+it->first+"\">"+it->first+"</button>";
  return buttons;
}
  
string ConfManager::mkShowfun(map<string, BarHolder*>& areas) {
  string showfun = "";
  map<string, BarHolder*>::iterator it;
  for(it = areas.begin(); it!=areas.end(); it++) showfun += "$(\"#show_"+it->first+"\").click(function(){showOnly(\""+it->first+"\");});\n";
  return showfun;
}

string ConfManager::mkAreaList(map<string, BarHolder*>& areas) {
  string arealist = "";
  map<string, BarHolder*>::iterator it;
  for(it = areas.begin(); it!=areas.end(); it++)
    if(arealist == "") arealist += ("\""+it->first+"\"");
       else arealist += (", \""+it->first+"\"");
  return arealist;
}

string ConfManager::mkDevList(map<string, BarHolder*>& areas) {
  string devlist = "";
  string devDescription;
  string components = "";
  map<string, BarHolder*>::iterator it;
  size_t i;
  for(it = areas.begin(); it!=areas.end(); it++) 
    for(i=0; i<it->second->size(); i++) {
       components = getComponentOf((*it->second)[i].device);
       devDescription = it->first+"."+(*it->second)[i].device;
       if(components != "") devDescription += ("+"+components);
       if(devlist == "") devlist += ("\""+devDescription+"\"");
          else devlist += (",\""+devDescription+"\"");
    }
  return devlist;
}

string ConfManager::getComponentOf(string name) {
   string res = "";
   size_t pos;
   string url = "/devicemanifest/"+name+".xml";
   string request = "GET " + url + " HTTP/1.1\r\n\r\n\r\n";
   string answer = NameTable::forward(request, "netdevices");
   if((pos = answer.find("$COMPONENT"))== string::npos) return res;
   pos = answer.find("value", pos);
   while(answer[pos] != '"') pos++;
   pos++;
   while(answer[pos] != '"') res+=answer[pos++];
   return res;
}

string ConfManager::mkWSport(string port) {
  unsigned p = hsrv::a2unsigned(port);
  p++;
  return hsrv::unsigned2a(p);
}

MMessage ConfManager::execGet(vector<string>& params, string& method, string body) {
   string target = FileManager::getStem(params.back());
   boost::unique_lock<boost::mutex> lock(mutexclusion);
   MMessage answer;
   if(target == "Tunnel") {
      MParams tunnel;
      string homer = FileManager::getRoot(hsrv::homedir);
      string confr = FileManager::getRoot(hsrv::configdir);
      string tunnelh = homer+"/sshtunnel/tunnel.xml";
      string tunnelc = confr+"/sshtunnel/tunnel.xml";
      if(FileManager::isFile(tunnelh)) tunnel.xmlLoad(tunnelh);
      else tunnel.xmlLoad(tunnelc);
      string body = par.get("tunnel_answer");
      string rport = tunnel.get("rport");
      string host = tunnel.get("host");
      string user = tunnel.get("user");
      string status = tunnel.get("status");
      hsrv::strReplace(body, "$RPORT", rport);
      hsrv::strReplace(body, "$HOST", host);
      hsrv::strReplace(body, "$USER", user);
      hsrv::strReplace(body, "$STATUS", status);
      answer.add("body", body);
   }
   else {
        answer.add("body", par.get("fail_answer"));
   }
   answer.add("content", "application/json");
   return answer;
}

