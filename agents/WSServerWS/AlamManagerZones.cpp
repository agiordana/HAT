bool AlarmManager::cleanZones(string zonesjs) {
   stringstream ss;
   string zoneid;
   string zonepath = hsrv::homedir+"/program/programs/Alarm/Zones";
   NameList zlist;
   NameList zlistfull;
   boost::property_tree::ptree pt, ptu;
   ss << zonesjs;
   read_json(ss, pt);
   ptu = pt.get_child("zones");
   JParser tree_parse("name", ptu);
   for(size_t i=0; i<tree_parse.size(); i++) {
        zoneid = tree_parse[i].first;
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
