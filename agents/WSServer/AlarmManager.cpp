using namespace std;

#include "AlarmManager.h"

boost::mutex AlarmManager::mutexclusion;
MParams AlarmManager::par;
bool AlarmManager::mksensors_called = false;
bool AlarmManager::init_called = false;
TransTab* AlarmManager::trans_tab = NULL;

bool AlarmManager::init(string file) {
   par = MParams(file);
   par.load();
   AlarmManager::trans_tab = new TransTab("trans_tab");
   return true;
}

MMessage AlarmManager::execGet(vector<string>& params, string& method, string body) {
   MMessage res;
   string url;
   string answer;
   boost::unique_lock<boost::mutex> lock(mutexclusion);
   mkSensorsAndActions(true);
   checkAndInitPrograms(true);
   if(params.size() == 3 && params[2] == "Alarm.json") {
      answer = getProgramStatus(params[2]);
      res.add("body", answer);
   }
   else if(params.size() == 3 && params[2] == "Schedule.json") {
      url = "/program/programs/Alarm/" + params.back();
      answer = getAlarmFile(url);
      res.add("body", makeGetAnswer(answer));
   }
   else if(params.size() == 3 && params[2] == "Zones.json") {
      url = "/program/programs/Alarm/" + params.back();
      answer = getAlarmFile(url);
      res.add("body", makeGetAnswer(answer));
   }
   else if(params.size() == 4 && params[2] == "Zones") {
      url = "/program/programs/Alarm/Zones/" + params.back();
      answer = getAlarmFile(url);
      if(answer == "Not Found") {
	 url = "/program/programs/Alarm/Zones/default.json";
	 answer = getAlarmFile(url);
      }
      res.add("body", makeGetAnswer(answer));
   }
   else if(params.size() == 4 && params[2] == "Manual") {
      url = "/program/programs/Alarm/Manual/" + params.back();
      answer = getAlarmFile(url);
      res.add("body", makeGetAnswer(answer));
   }
   else if(params.size() == 4 && params[2] == "Auto") {
      url = "/program/programs/Alarm/Auto/" + params.back();
      answer = getAlarmFile(url);
      res.add("body", makeGetAnswer(answer));
   }

   res.add("content", "application/json");
   return res;
}

MMessage AlarmManager::execPut(vector<string>& params, string& method, string msgstring) {
   MMessage res;
   boost::unique_lock<boost::mutex> lock(mutexclusion);
   mkSensorsAndActions(true);
   checkAndInitPrograms(true);
   MMessage answer;
   TransEntry call;
   if(params.back() == "reset_queue.json") {
	initExecutionQueues();
        mkSensorsAndActions();
        initZones();
        initAuto();
        answer.add("content", "application/json");
        answer.add("body", par.get("put_answer"));
	return answer;
   }
   if(params.back() == "ZONE.json") {
     if(!isZoneDefined()) {
        MMessage complain;
        complain.add("body",par.get("zone_complain"));
        complain.add("content", "application/json");
        return complain;
     }
   }
   call = trans_tab->get(params, method);
   if(call.isnoentry()) {
        MMessage complain;
        complain.add("body", "Not Supported");
        return complain;
   }
   MMessage msg = call.rpc2cmd(params,method,msgstring);
   string message = msg.xmlEncode(1);
   string body = sendCmd(message);
   answer = mkAnswer(body, call, "PUT");
   return answer;
}

MMessage AlarmManager::mkAnswer(string& body, TransEntry& call, string method) {
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

MMessage AlarmManager::execPost(vector<string>& params, string& method, string body) {
   MMessage res;
   bool done = true;
   string alarm_answer;
   boost::unique_lock<boost::mutex> lock(mutexclusion);
   mkSensorsAndActions(true);
   checkAndInitPrograms(true);
   string target = FileManager::getStem(params.back());
   if(params.size() == 3 && (target == "Schedule" || target == "Zones")) {
      string path = "program/programs/Alarm";
      if(checkAndMakePath(path)) {
	 string tmp = mkResource(body);
	 path = "/"+path+"/"+params.back();
	 hsrv::archive->addResource(path, tmp, true);
	 FileManager::deleteFile(tmp);
         // mando il file all'agente Alarm
         if(target == "Schedule") {
	    string xmlauto = schedulej2x(body);
	    done = sendFile(par.get("sendAUTOurl"), xmlauto);
         }
         if(target == "Zones") {
	    string xmlzones = zonesj2x(body);
	    done = sendFile(par.get("sendZONEurl"), xmlzones);
         }
	 string answer = par.get("post_answer");
	 if(done == true) hsrv::strReplace(answer, "$TRUEFALSE", "true");
	   else hsrv::strReplace(answer, "$TRUEFALSE", "false");
	 res.add("body", answer);
      }
      else {
	 res.add("Body", "Forbidden");
     }
   }
   if(params.size() == 4 && (params[2] == "Zones" ||params[2] == "Manual"||params[2] == "Auto")) {
      string path = "program/programs/Alarm/" + params[2] + "/";
      string result;
      string answer = par.get("post_answer");
      if(checkAndMakePath(path)) {
         string tmp = mkResource(body);
         path = "/"+path+"/"+params.back();
         hsrv::archive->addResource(path, tmp, true);
         FileManager::deleteFile(tmp);
	 if(params[2] == "Zones") {
	    zoneUpdate(params.back());
            result = "true";
	 }
	 else if(params[2] == "Manual"||params[2] == "Auto") {
	    result = setExecQueue(params[2], params.back(), body);
	 }
         hsrv::strReplace(answer, "$TRUEFALSE", result);
         res.add("body", answer);
      }
      else {
         res.add("Body", "Forbidden");
     }
   }
   res.add("content", "application/json");
   return res;
}

bool AlarmManager::mkSensorsAndActions(bool mode) {
   size_t pos=0;
   size_t np=0;
   MMessage ag = NameTable::getAgentDescription(par.get("alarmag"));
   if(ag.getString("name")=="none") return false;
   if(mksensors_called && mode) return true;

   ofstream sensors;
   ofstream actions;
   string path = "/program/programs/Alarm/sensors.json";
   string tmp = hsrv::homedir + "/_tmp";
   string evclist = "<\?xml version=\"1.0\" \?>"+fileDownload("/status/sensordevices.xml");
   MParams slist;
   slist.xmlDecode(evclist);
   sensors.open(tmp.c_str());
   sensors << "{\"sensors\": [";
   for(np = 0; np<slist.size(); np++) {
       sensors << "{\"name\":\"" << slist[np].name << "\",\"selection_status\":\"";
//	 sensors << (slist[np].value == "ON" ? "true" : "false") <<"\"}";
       sensors << "false"<<"\"}";
       if(slist.size()>1 && np<slist.size()-1) sensors<<",";
   }
   sensors<<"]}";
   sensors.close();
   hsrv::archive->addResource(path, tmp, true);
   FileManager::deleteFile(tmp);
      // Load Actions
   string aclist = "<\?xml version=\"1.0\" \?>"+fileDownload("/status/controldevices.xml");
   MParams alist;
   alist.xmlDecode(aclist);
   tmp = hsrv::homedir + "/_tmp_";
   actions.open(tmp.c_str());
   actions << "{\"actions\": [";
   for(np=0; np<alist.size(); np++) {
      actions<<"{\"name\":\""<<alist[np].name<<"\",\"selection_status\":\"";
//    actions<<(alist[np].value == "ON" ? "true" : "false") <<"\"}";
      actions<<"false"<<"\"}";
      if(alist.size()>1 && np<alist.size()-1) actions<<",";
   }
   actions <<"]}";
   actions.close();
   path = "/program/programs/Alarm/actions.json";
   hsrv::archive->addResource(path, tmp, true);
   FileManager::deleteFile(tmp);

   mksensors_called = true;
   return true;
}

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

bool AlarmManager::initExecutionQueues() {
   stringstream exq;
   NameList controldevice;
   NameList tmp;
   string pathname;
   string optcontrol = "<\?xml version=\"1.0\" \?>" + fileDownload("/status/optcontroldevices.xml");
   MParams paropt;
   paropt.xmlDecode(optcontrol);
   for(size_t i=0; i<paropt.size(); i++) controldevice.push_back(paropt[i].name);
   exq << "{\"devices\": [";
   for(size_t i=0; i<controldevice.size(); i++) {
      if(i>0) exq << ",";
      exq <<"{\"name\":\"" << controldevice[i] <<"\",\"selection_status\":\"false\",\"action\":\"false\"}"; 
   }
   exq << "]}";
   string exqstr = exq.str();
   string restmp = mkResource(exqstr);
   pathname = "/program/programs/Alarm/Auto/Activation.json";
   hsrv::archive->addResource(pathname, restmp, true);
   pathname = "/program/programs/Alarm/Auto/Deactivation.json";
   hsrv::archive->addResource(pathname, restmp, true);
   pathname = "/program/programs/Alarm/Manual/Activation.json";
   hsrv::archive->addResource(pathname, restmp, true);
   pathname = "/program/programs/Alarm/Manual/Deactivation.json";
   hsrv::archive->addResource(pathname, restmp, true);
   FileManager::deleteFile(restmp);
   return true;
}


string AlarmManager::schedulej2x(std::string& jschedule) {
   boost::property_tree::ptree pt;
   string scheduletemp = hsrv::configdir + "/templates/Auto.xml";
   string xschedule = loadTemplate(scheduletemp);
   string tt;
   stringstream ss;
   ss<<jschedule;
   read_json(ss, pt);
   BOOST_FOREACH(boost::property_tree::ptree::value_type &v, pt.get_child("schedule_data")) {
      tt = v.second.get<string>("start");
      hsrv::strReplace(xschedule, "$TT", tt); 
      tt = v.second.get<string>("end");
      hsrv::strReplace(xschedule, "$TT", tt); 
   }
   return xschedule;
}

string AlarmManager::zonesj2x(string& zonesjs) {
   map<string,bool> lookup;
   NameList zlist;
   NameList alist;
   MMessage mreq("httprequest");
   stringstream ss;
   string zoneid;
   string xmlactionlist = "";
   boost::property_tree::ptree pt;
   string zonestemp = hsrv::configdir + "/templates/Zone.xml";
   string zonesxml = loadTemplate(zonestemp);
   ss << zonesjs;
   read_json(ss, pt);
   BOOST_FOREACH(boost::property_tree::ptree::value_type &v, pt.get_child("zones")) {
	zoneid = v.second.get<string>("name");
	if(v.second.get<string>("enable_status") == "true") {
	   zlist.push_back(zoneid);
        }
   }
   for(size_t i=0; i<zlist.size(); i++) {
      boost::property_tree::ptree tzpt;
      stringstream tzss;
      string url = "/program/programs/Alarm/Zones/" + zlist[i] + ".json";
      mreq.add("url", url);
      tzss << hsrv::archive->getFile(mreq);
      read_json(tzss, tzpt);
      BOOST_FOREACH(boost::property_tree::ptree::value_type &v, tzpt.get_child("sensors")) {
	string aname = v.second.get<string>("name");
	if(v.second.get<string>("selection_status") == "true") {
	   if(lookup.count(aname) == 0) {
	       alist.push_back(aname);
	       lookup[aname] = true;
	   }
        }
      }
      BOOST_FOREACH(boost::property_tree::ptree::value_type &v, tzpt.get_child("actions")) {
	string curaction = v.second.get<string>("name");
	if(v.second.get<string>("selection_status") == "true") {
	   if(lookup.count(curaction) == 0) {
	      alist.push_back(curaction + "_on");
	      alist.push_back(curaction + "_off");
	      lookup[curaction] = true;
           }
        }
      }
   }
   for(size_t j=0; j<alist.size(); j++) {
      xmlactionlist += "<alarm name=\"";
      xmlactionlist += alist[j];
      xmlactionlist += "\"/>";
   }
   hsrv::strReplace(zonesxml, "$ALARMS", xmlactionlist);
   cleanZones(zonesjs);
   return zonesxml;
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

string AlarmManager::setExecQueue(string& program, string& qq, string& jsbody) {
   string actionname, actionstatus, actionmode;
   string alist = "";
   boost::property_tree::ptree pt;
   string templatepath = hsrv::configdir + "/templates/settractions.xml";
   string message = loadTemplate(templatepath);
   stringstream ss;
   ss<<jsbody;
   read_json(ss, pt);
   BOOST_FOREACH(boost::property_tree::ptree::value_type &v, pt.get_child("devices")) { 
       actionname = v.second.get<string>("name");
       actionstatus = v.second.get<string>("selection_status");
       actionmode = v.second.get<string>("action");
       if(actionstatus == "true") {
	  if(actionname.find("SH_") != string::npos) {
	     alist == "" ? alist = actionname : alist += ("+"+actionname);
	     if(actionmode == "true") alist += "_pup";
		else alist += "_pdown";
	  }
          else {
	     alist == "" ? alist = actionname : alist += ("+"+actionname);
	     if(actionmode == "true") alist += "_on"; 
	        else alist += "_off";
	  }
       }
   }
   if(alist == "") alist = "none";
   if(program == "Auto") hsrv::strReplace(message, "$NAME", "AUTOtriggered");
	else hsrv::strReplace(message, "$NAME", "MANUALtriggered");
   if(FileManager::getStem(qq) == "Activation") {
	hsrv::strReplace(message, "$ENABLE", alist);
	hsrv::strReplace(message, "$DISABLE", "");
   }
   else {
	hsrv::strReplace(message, "$ENABLE", "");
	hsrv::strReplace(message, "$DISABLE", alist);
   }
   bool res = sendFile("/", message);
   if(res) return "true";
	else return "false";
}

bool AlarmManager::cleanZones(string zonesjs) {
   stringstream ss;
   string zoneid;
   string zonepath = hsrv::homedir+"/program/programs/Alarm/Zones";
   NameList zlist;
   NameList zlistfull;
   boost::property_tree::ptree pt;
   ss << zonesjs;
   read_json(ss, pt);
   BOOST_FOREACH(boost::property_tree::ptree::value_type &v, pt.get_child("zones")) {
        zoneid = v.second.get<string>("name");
        zlist.push_back(zoneid);
   }
   FileManager::fileList(zonepath,zlistfull);
   for(size_t i=0; i<zlistfull.size(); i++)
     if(!zlist.member(FileManager::getStem(zlistfull[i]))&&zlistfull[i] != "default.json") {
	FileManager::deleteFile(zonepath,zlistfull[i]);
     }
   return true;
}

bool AlarmManager::isZoneDefined() {
   string zonepath = "/program/programs/Alarm/Zones.json";
   string zones;
   MMessage mreq;
   mreq.add("url", zonepath);
   zones = hsrv::archive->getFile(mreq);
   if(zones == "Not Found") return false;
   if(zones.find("true") != string::npos) return true;
   return false;
}
