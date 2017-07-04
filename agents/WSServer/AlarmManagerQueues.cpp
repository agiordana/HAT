using namespace std;

#include "AlarmManager.h"


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

string AlarmManager::setExecQueue(string& program, string& qq, string& jsbody) {
   string actionname, actionstatus, actionmode;
   string alist = "";
   boost::property_tree::ptree pt, ptu;
   string templatepath = hsrv::configdir + "/templates/settractions.xml";
   string message = loadTemplate(templatepath);
   stringstream ss;
   ss<<jsbody;
   read_json(ss, pt);
   ptu = pt.get_child("devices");
   JParser tree_parse("name", "selection_status", "action", ptu);
   for(size_t i=0; i<tree_parse.size(); i++) {
       actionname = tree_parse[i].first;
       actionstatus = tree_parse[i].second;
       actionmode = tree_parse[i].third;
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

