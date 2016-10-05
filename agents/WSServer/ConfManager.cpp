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
cout<<"CONFIGURING "<<target<<endl;
// cout<<body<<endl;
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
     BOOST_FOREACH(boost::property_tree::ptree::value_type &v, pt) {
      string dev = v.second.get<string>("name");
      string area = v.second.get<string>("area");
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
     hsrv::strReplace(gui, "$DEVLIST", description);
     description = mkAreaList(barholder);
     hsrv::strReplace(gui, "$AREALIST", description);
     string destination = hsrv::configdir + "/doc/gui.html";
     FileManager::saveFile(destination, gui);
     answer.add("body", par.get("mkgui_answer"));
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
  map<string, BarHolder*>::iterator it;
  size_t i;
  for(it = areas.begin(); it!=areas.end(); it++)
    for(i=0; i<it->second->size(); i++)
       if(devlist == "") devlist += ("\""+(*it->second)[i].device+"\"");
          else devlist += (",\""+(*it->second)[i].device+"\"");
  return devlist;
}
