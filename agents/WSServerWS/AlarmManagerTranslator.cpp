using namespace std;

#include "AlarmManager.h"

string AlarmManager::schedulej2x(std::string& jschedule) {
   boost::property_tree::ptree pt, ptu;
   string scheduletemp = hsrv::configdir + "/templates/Auto.xml";
   string xschedule = loadTemplate(scheduletemp);
   string tt;
   stringstream ss;
   ss<<jschedule;
   read_json(ss, pt);
   ptu = pt.get_child("schedule_data");
   JParser tree_parse("start", "end", ptu);
   for(size_t i=0; i<tree_parse.size(); i++) {
      tt = tree_parse[i].first;;
      hsrv::strReplace(xschedule, "$TT", tt); 
      tt = tree_parse[i].second;
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
   boost::property_tree::ptree pt, ptu;
   string zonestemp = hsrv::configdir + "/templates/Zone.xml";
   string zonesxml = loadTemplate(zonestemp);
   ss << zonesjs;
   read_json(ss, pt);
   ptu = pt.get_child("zones");
   JParser tree_parse("name", "enable_status", ptu);
   for(size_t i=0; i<tree_parse.size(); i++) {
	zoneid = tree_parse[i].first;
	if(tree_parse[i].second == "true") {
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
      ptu = tzpt.get_child("sensors");
      JParser sensor_parse("name", "selection_status", ptu);
      for(size_t j=0; j<sensor_parse.size(); j++) {
	string aname = sensor_parse[j].first;
	if(sensor_parse[j].second == "true") {
	   if(lookup.count(aname) == 0) {
	       alist.push_back(aname);
	       lookup[aname] = true;
	   }
        }
      }
      ptu = tzpt.get_child("actions");
      JParser action_parse("name", "selection_status", ptu);
      for(size_t j=0; j<action_parse.size(); j++) {
	string curaction = action_parse[j].first;
	if(action_parse[j].second == "true") {
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

