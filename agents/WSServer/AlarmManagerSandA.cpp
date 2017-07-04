using namespace std;

#include "AlarmManager.h"

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

