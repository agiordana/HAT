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

