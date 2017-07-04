using namespace std;

#include "AlarmManager.h"


bool AlarmManager::isDigit(char c) {
  return (c=='0' || c=='1' || c=='2' || c=='3' || c=='4' || c=='5' || c=='6' || c=='7' || c=='8' || c=='9');
}

string AlarmManager::mkExternalName(int mode, string sname, string ecname) {
   string ext_name;
   NameList sensor_name;
   string rnumber = "";
   sensor_name.init(sname, '_');
   if(mode == 0&&sensor_name.size()>1) {
	ext_name = sensor_name[0]+"_000"+sensor_name[1];
	return ext_name;
   }
   if(mode >0 && sensor_name.size()>1) {
	ext_name = sensor_name[0]+ "_";
        for(size_t i=ecname.size(); i>0; i--) if(isDigit(ecname[i])) rnumber += ecname[i];
	for(size_t i=0; i<3-rnumber.size(); i++) ext_name += '0';
	ext_name += rnumber;
	return ext_name;
   }
   return "";
}

string AlarmManager::getBody(string message) {
   string body = message.substr(message.find("\r\n\r\n")+4);
   return body;
}

string AlarmManager::getAlarmFile(string url) {
   string answer;
   MMessage mreq;
   mreq.add("url", url);
   answer = hsrv::archive->getFile(mreq);
   return answer;
}

string AlarmManager::getProgramStatus(string file) {
    boost::property_tree::ptree pt;
    boost::property_tree::ptree data;
    stringstream ss;

    string answer, uri, response, filetype="";
    uri = par.get("get_prgstatus");
    
    response = fileDownload(uri);
    ss <<response;
    read_json(ss, pt);
    data = pt.get_child("prgstatus");
    string program = data.get<string>("program");
    string active = data.get<string>("active");
    answer = par.get("prg_answer");
    hsrv::strReplace(answer,"$PROGRAM", program);
    hsrv::strReplace(answer,"$PROGRAM_ACTIVE", active);
    return hsrv::filterChar(answer);
}

bool AlarmManager::checkAndMakePath(string& path) {
   NameList plist;
   plist.init(path, '/');
   if(plist.size() < 1 || plist[0] == "") return false;
   string fullpath = hsrv::homedir +"/" + plist[0];
   if(!FileManager::isDir(fullpath)) return false;
   for(size_t i=1; i<plist.size(); i++) {
     fullpath += ("/"+plist[i]);
     if(!FileManager::isDir(fullpath)) FileManager::makeDir(fullpath,false);
   }
   return true;
}

string AlarmManager::makeGetAnswer(string& answer) {
   string res = par.get("get_answer");
   string tmp = answer;
   size_t i;
   if(tmp.size() == 0) return "";
   for(i=0; i<tmp.size()&&tmp[i] != '{'; i++);
   if(tmp[i] == '{') tmp.erase(0, i+1);
   for(i=tmp.size()-1; i>0 && tmp[i] != '}'; i--);
   if(i>0) tmp.erase(i, string::npos);
   hsrv::strReplace(res,"$BODY", tmp);
   return res;
}

bool AlarmManager::checkAndInitPrograms(bool mode) {
   string autoxml, zonesxml, autojson, request;
   if(init_called && mode) return false;
   MMessage ag = NameTable::getAgentDescription(par.get("alarmag"));
   if(ag.getString("name")=="none") return false;
   autoxml = fileDownload("/programs/AUTO");
   zonesxml = fileDownload("/programs/ZONE");
   if(autoxml.find("AUTO")==string::npos) initAuto();
   if(zonesxml.find("ZONE")==string::npos) initZones();
   string exq = hsrv::homedir + "/program/programs/Alarm/Auto/Activation.json";
   string exq1 = hsrv::homedir + "/program/programs/Alarm/Auto/Deactivation.json";
   string exq2 = hsrv::homedir + "/program/programs/Alarm/Manual/Activation.json";
   string exq3 = hsrv::homedir + "/program/programs/Alarm/Manual/Deactivation.json";
   if(!FileManager::isFile(exq) || !FileManager::isFile(exq1) || !FileManager::isFile(exq2) || !FileManager::isFile(exq3))
	  initExecutionQueues();
   init_called = true;
   return true;
}

bool AlarmManager::initAuto() {
   string path = "program/programs/Alarm/Schedule.json";
   string src = hsrv::configdir+ "/templates/scheduleproto.json";
   hsrv::archive->addResource(path, src, true);  
   return true;
}

bool AlarmManager::initZones() {
   MMessage mreq("httprequest");
   string uri = "/program/programs/Alarm/sensors.json";
   mreq.add("url", uri);
   string sensors = hsrv::archive->getFile(mreq);
   string clearcmd = "rm -f "+ hsrv::homedir + "/program/programs/Alarm/Zones/*";
   hsrv::cmdExec(clearcmd);
   uri = "/program/programs/Alarm/actions.json";
   mreq.add("url", uri);
   string actions = hsrv::archive->getFile(mreq);
   string globalzone = hsrv::configdir + "/templates/defaultzone.json";
   string prg = loadTemplate(globalzone);
   hsrv::strReplace(prg, "$SENSORS", extractString(sensors, '[', ']'));
   hsrv::strReplace(prg, "$ACTIONS", extractString(actions, '[', ']'));
   uri = "/program/programs/Alarm/Zones/default.json";
   string tmp = mkResource(prg);
   hsrv::archive->addResource(uri, tmp, true);
   FileManager::deleteFile(tmp);
   tmp = hsrv::configdir+"/templates/zonesproto.json";
   uri = "/program/programs/Alarm/Zones.json";
   hsrv::archive->addResource(uri, tmp, true);
   return true;
}

string AlarmManager::loadTemplate(string file) {
   string res;
   ifstream in(file.c_str());
   if(!in.is_open()) return "";
   char ch;
   while((ch=in.get())&&!in.eof()) {
      res += ch;
   }
   in.close();
   return res;
}

string AlarmManager::mkResource(string& file) {
   ofstream out;
   string tmp = hsrv::homedir + "/_tmp.Alarm";
   out.open(tmp.c_str());
   out<<file<<endl;
   out.close();
   return tmp;
}

string AlarmManager::extractString(string& src, char first, char last) {
   string tmp = src;
   size_t i;
   if(tmp.size() == 0) return "";
   for(i=0; i<tmp.size()&&tmp[i] != first; i++);
   if(tmp[i] == first) tmp.erase(0, i+1);
   for(i=tmp.size()-1; i>0 && tmp[i] != last; i--);
   if(i>0) tmp.erase(i, string::npos);
   return tmp;
}

bool AlarmManager::zoneUpdate(string newz) {
   MMessage mreq("httprequest");
   size_t i, np;
   string zonepath = "/program/programs/Alarm/Zones.json";
   string zones;
   string newname = "\"name\":\"" + FileManager::getStem(newz) + "\"";
   stringstream newzones;
   mreq.add("url", zonepath);
   zones = hsrv::archive->getFile(mreq);
   if(zones == "Not Found") return false;
   if(zones.find(newname) != string::npos) return true;
   for(i=0; i<zones.size()&&zones[i]!=']'; i++) newzones << zones[i];
   if(i>1 && zones[i-1] != '[' && zones[i-2] != '[') newzones << ',';
   newzones << "{\"name\":\"" << FileManager::getStem(newz) <<"\",\"enable_status\":\"false\"}";
   for(; i<zones.size(); i++) newzones << zones[i];
   string tzone = newzones.str();
   string tmp = mkResource(tzone);
   hsrv::archive->addResource(zonepath, tmp, true);
   FileManager::deleteFile(tmp);
   return true;
}

string AlarmManager::sendCmd(string file) {
   string request = "POST / HTTP/1.1\r\nContent-Length: "+hsrv::unsigned2a(file.size())+"\r\n\r\n"+file+"\r\n\r\n";
   string answer = NameTable::forward(request, par.get("alarmag"));
   return answer;
}

bool AlarmManager::sendFile(string url, string file) {
   string request = "POST "+url+" HTTP/1.1\r\nContent-Length: "+hsrv::unsigned2a(file.size())+"\r\n\r\n"+file+"\r\n\r\n";
   string answer = NameTable::forward(request, par.get("alarmag"));
   if(answer.find("Not Found") == string::npos &&
	(answer.find("list") != string::npos || answer.find("done") != string::npos || answer.find("OK") != string::npos)) return true;
   return false;
}

string AlarmManager::fileDownload(string url) {
   string request = "GET " + url + " HTTP/1.1\r\n\r\n\r\n";
   string answer = NameTable::forward(request, par.get("alarmag"));
   return getBody(answer);
}
