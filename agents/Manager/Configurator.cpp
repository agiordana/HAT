//
//  Configurator.cpp
//  mngagent
//
//  Created by Attilio Giordana on 1/10/15.
//  Copyright (c) 2015 Università del Piemonte Orientale. All rights reserved.
//

#include "Configurator.h"

using namespace std;

boost::mutex Configurator::mutex;

Configurator::Configurator(string n): RouterExec(n) {
   MParams p("threads", "Configurator");
   p.load();
   sensor_category = p.get("sensor_category");
   cmd_category = p.get("cmd_category");
   optcmd_category = p.get("optcmd_category");
   do_userprogrammable = p.get("do_userprogrammable");
   ls_category  = p.get("ls_category");
}
   

MMessage Configurator::exec(std::string uri, std::vector<std::string> params, std::string method, std::string body) {
    std::string response, devname, filetype="";
    MMessage msg, mresp;
    NameList nl;
    size_t pos;
    nl.init(uri, '/');

    boost::unique_lock<boost::mutex> lock(mutex);

    if(!hsrv::checkConfigureEnabled()) {
        response = "Not Found";
        mresp.add("body", response);
        return mresp;
    }
    if(method != "configure") {
	response = "Not Found";
        mresp.add("body", response);
        return mresp;
    }
    if(body != "")  addBody(msg, body);

    if (params.back() == "basic") {
        response = basicConf();
    }
    else if(params.back() == "action") {
	response = mkItem(msg, "actions", "action.xml");
    }
    else if(params.back() == "condition") {
	response = mkItem(msg, "conditions", "conditions.xml");
    }
    else if(params.back() == "trcondition") {
	response = mkItem(msg, "tr_conditions", "conditions.xml");
    }
    else if(params.back() == "eventcounter") {
	response = mkItem(msg, "eventcounter", "eventcounter.xml");
    }
    else if(params.back() == "rule") {
	response = mkItem(msg, "rules", "rule.xml");
        mkManifest(msg);
    }
    else if(params.back() == "timedrule") {
	response = mkItem(msg, "timedrules", "timedrule.xml");
    }
    else if(params.back() == "reboot") {
	response = "done";
	exit(1);
    }
    else if(params.size()==3 && params[0]=="delete") {
        response = deleteItem(params[1], params.back());
    }
    mresp.add("body", response);
    return mresp;
}  

string Configurator::basicConf() {
   confReset();
   mkDeviceLists();
   mkActions();
   mkEventcounters();
   mkConditions();
   mkTimedrules();
   mkUserProgrammable();
   return "done";
}

bool Configurator::mkDeviceLists() {
  vector<string> tempname;
  devname.clear();
  cmdname.clear();
  optcmdname.clear();
  lsname.clear();
  NameList scat;
  scat.init(sensor_category, '+');
  NameList ccat;
  ccat.init(cmd_category, '+');
  NameList optcat;
  optcat.init(optcmd_category, '+');
  NameList lscat;
  lscat.init(ls_category, '+');
  for(size_t i=0; i<scat.size(); i++) {
    tempname = NameService::getNames("devices", scat[i]);
    for(size_t j=0; j<tempname.size(); j++) {
	devname.push_back(tempname[j]);
    }
    tempname.clear();
  }
  hsrv::saveItemList("sensordevices", "status", devname);
  for(size_t i=0; i<ccat.size(); i++) {
    tempname = NameService::getNames("devices", ccat[i]);
    for(size_t j=0; j<tempname.size(); j++) {
      cmdname.push_back(tempname[j]);
    }
    tempname.clear();
  }
  hsrv::saveItemList("controldevices", "status", cmdname);
  for(size_t i=0; i<optcat.size(); i++) {
    tempname = NameService::getNames("devices", optcat[i]);
    for(size_t j=0; j<tempname.size(); j++) {
      optcmdname.push_back(tempname[j]);
    }
    tempname.clear();
  }
  hsrv::saveItemList("optcontroldevices", "status", optcmdname);
  for(size_t i=0; i<lscat.size(); i++) {
    tempname = NameService::getNames("devices", lscat[i]);
    for(size_t j=0; j<tempname.size(); j++) lsname.push_back(tempname[j]);
    tempname.clear();
  }
  hsrv::saveItemList("lsdevices", "status", lsname);
  return true;
}

string Configurator::mkItem(MMessage& m, string path, string temp) {
  ifstream devtemplate;
  ofstream device;
  string description = "";
  string oldvalue = "";
  string newvalue = "";
  string name;
  string templatepath = hsrv::configdir+"/templates/"+ temp;
  string devicepath = hsrv::configdir+"/"+path;
  char ch;
  
  name = m.getString("name");
  if(name == "") name = m.getString("$NAME");
  if(name == "") return "Failed";
  devicepath = devicepath + "/" + name + ".xml";

  devtemplate.open(templatepath.c_str());
  if(!devtemplate.is_open()) return "Failed";
  while((ch=devtemplate.get())&&!devtemplate.eof()) {
    description+=ch;
  }
  devtemplate.close();
  MMessage::iterator it;
  for(it=m.begin(); it!=m.end(); it++) {
    oldvalue = it->first;
    newvalue = m.getString(it->first);
    if(oldvalue[0]=='$') while(hsrv::strReplace(description,oldvalue,newvalue));
  }

  device.open(devicepath.c_str());
  if(!device.is_open()) {
    hsrv::logger->info("Impossible to save: "+devicepath);
    return "Failed";
  }

  for(size_t i=0; i<description.size(); i++) device.put(description[i]);
  device.close();
  return "done";
}

string Configurator::getKeyword(string& src, size_t& pos) {
  string found = "";
  size_t i;
  for(i=pos; i<src.size() && src[i]!='$'; i++);
  if(i==src.size()) {
    pos = string::npos;
    return "";
  }
  while(i<src.size() && isUpcase(src[i])) found += src[i];
  pos = i;
  return found;
}

bool Configurator::isUpcase(char ch) {
  return (ch>='A' && ch<='Z');
}

string Configurator::getCategory(string& name) {
  return name.substr(0,2);
}

bool Configurator::mkActions() {
  string path = "actions";
  string tr_path = "tr_actions";
  string temp = "action.xml";
  string nameon;
  string nameoff;
  string nameswitch;
  string cmdon;
  string cmdoff;
  string cmdswitch;
  for(size_t i=0; i<cmdname.size(); i++) {
    MMessage mon;
    MMessage moff;
    MMessage mswitch;
    if(getCategory(cmdname[i])=="SH") {
       nameswitch = "";
       nameon = cmdname[i]+"_pup";
       nameoff = cmdname[i]+"_pdown";
       cmdon = cmdname[i];
       cmdoff = cmdname[i];
    }
    else {
       nameon = cmdname[i]+"_on";
       nameoff = cmdname[i]+"_off";
       nameswitch = cmdname[i]+"_switch";
       cmdon = cmdname[i];
       cmdoff = cmdname[i];
       cmdswitch = cmdname[i];
    }
    // creazione actions per rules
    mon.add("$NAME", nameon);
    mon.add("$TARGET", cmdon);
    if(getCategory(cmdname[i])=="SH") mon.add("$VALUE","UP");
    else if(getCategory(cmdname[i])=="LI") {
	mon.add("$VALUE","PON");
        mswitch.add("$NAME", nameswitch);
        mswitch.add("$TARGET", cmdswitch);
	mswitch.add("$VALUE","ON");
    }
    else mon.add("$VALUE", "ON");
    moff.add("$NAME", nameoff);
    moff.add("$TARGET", cmdoff);
    if(getCategory(cmdname[i])=="SH") moff.add("$VALUE","DOWN");
    else if(getCategory(cmdname[i])=="LI") {
	moff.add("$VALUE","POFF");
    }
    else moff.add("$VALUE", "OFF");
    mkItem(mon, path, temp);
    mkItem(moff, path, temp);
    if(getCategory(cmdname[i])=="LI") mkItem(mswitch, path, temp);
    // creazione actions per timedrules
  } 
  for(size_t i=0; i<optcmdname.size(); i++) {
    MMessage mon;
    MMessage moff;
    if(getCategory(optcmdname[i])=="SH") {
       nameon = optcmdname[i]+"_pup";
       nameoff = optcmdname[i]+"_pdown";
       cmdon = optcmdname[i];
       cmdoff = optcmdname[i];
    }
    else {
       nameon = optcmdname[i]+"_on";
       nameoff = optcmdname[i]+"_off";
       cmdon = optcmdname[i];
       cmdoff = optcmdname[i];
    }
    mon.add("$NAME", nameon);
    mon.add("$TARGET", cmdon);
    if(getCategory(optcmdname[i])=="SH") mon.add("$VALUE","UP");
    else if(getCategory(optcmdname[i])=="LI") mon.add("$VALUE","PON");
    else mon.add("$VALUE", "ON");
    moff.add("$NAME", nameoff);
    moff.add("$TARGET", cmdoff);
    if(getCategory(optcmdname[i])=="SH") moff.add("$VALUE","DOWN");
    else if(getCategory(optcmdname[i])=="LI") moff.add("$VALUE","POFF");
    else moff.add("$VALUE", "OFF");
    mkItem(mon, tr_path, temp);
    mkItem(moff, tr_path, temp);
  }
  return true;
}

bool Configurator::mkEventcounters() {
  string path = "eventcounters";
  string temp = "eventcounter.xml";
  for(size_t i=0; i<devname.size(); i++) {
    MMessage m;
    m.add("$NAME", devname[i]);
    m.add("$SOURCE", devname[i]);
    m.add("$THRESHOLD", "1");
    m.add("$DELAY", "0");
    mkItem(m, path, temp);
  }
  return true;
}

bool Configurator::mkConditions() {
  string path = "conditions";
  string tr_path = "tr_conditions";
  string temp = "condition.xml";
  for(size_t i=0; i<lsname.size(); i++) {
    MMessage m;
    m.add("$NAME", lsname[i]);
    m.add("$SOURCE", lsname[i]);
    mkItem(m, path, temp);
    mkItem(m, tr_path, temp);
  }
  return true;
}

bool Configurator::addBody(MMessage& m, std::string body) {
   if(hsrv::isXmlstring(body)) {
      m.xmlAppend(body);
   }
   else if(isHtml(body)) {
      m.htmlAppend(body);
   } else {
      m.add("body", body);
   }
   return true;
}


bool Configurator::isHtml(std::string& body) {
   if(body.find("&")!=std::string::npos) return true;
   else return false;
}

bool Configurator::confReset() {
   string cmd;
   vector<string> paths;
   paths.push_back(hsrv::configdir+"/actions/");
   paths.push_back(hsrv::configdir+"/tr_actions/");
   paths.push_back(hsrv::configdir+"/rules/");
   paths.push_back(hsrv::configdir+"/timedrules/");
   paths.push_back(hsrv::configdir+"/eventcounters/");
   paths.push_back(hsrv::configdir+"/conditions/");
   paths.push_back(hsrv::configdir+"/tr_conditions/");
   paths.push_back(hsrv::configdir+"/rulemanifest/");
   paths.push_back(hsrv::homedir+"/actions/");
   paths.push_back(hsrv::homedir+"/tr_actions/");
   paths.push_back(hsrv::homedir+"/rules/");
   paths.push_back(hsrv::homedir+"/timedrules/");
   paths.push_back(hsrv::homedir+"/eventcounters/");
   paths.push_back(hsrv::homedir+"/conditions/");
   paths.push_back(hsrv::homedir+"/tr_conditions/");
   for(size_t i=0; i<paths.size(); i++) 
     if(FileManager::isDir(paths[i])) {
	cmd = "rm -f "+paths[i]+"*.xml";
        hsrv::cmdExec(cmd);
     } 
   return true;
}

bool Configurator::mkManifest(MMessage& m) {
   ofstream manifest;
   ostringstream  tmanifest;
   string name = m.getString("$NAME");
   string manifestpath = hsrv::configdir+"/rulemanifest/";
   if(!FileManager::isDir(manifestpath)) FileManager::makeDir(manifestpath, true);
   manifestpath += name;
   manifestpath += ".xml";
   manifest.open(manifestpath.c_str());
   m.xmlPrint(tmanifest,1);
   manifest<<tmanifest.str();
   manifest.close();
   return true;
}

string Configurator::deleteItem(string& itemclass, string& name) {
   if(itemclass == "rule") {
     string fname = name +".xml";
     string manifestpath = hsrv::configdir+"/rulemanifest";
     string rulepathc = hsrv::configdir+"/rules";
     string rulepathh= hsrv::homedir+"/rules";
     FileManager::deleteFile(manifestpath, fname); 
     FileManager::deleteFile(rulepathc, fname); 
     FileManager::deleteFile(rulepathh, fname); 
     return "done";
   }
   else return "Failed";
}

bool Configurator::mkTimedrules() {
   string templpath = hsrv::configdir + "/templates/";
   string trulespath = hsrv::configdir + "/timedrules";
   string autopath = templpath + "AUTOtriggered.xml";
   string manualpath = templpath + "MANUALtriggered.xml";
   if(FileManager::isFile(autopath)) FileManager::copyFile(autopath,trulespath,"AUTOtriggered.xml");
   if(FileManager::isFile(manualpath)) FileManager::copyFile(manualpath,trulespath,"MANUALtriggered.xml");
   return true;
}

bool Configurator::mkUserProgrammable() {
   string rulename;
   string rule_set;
   string rule_reset;
   string alias;
   if(do_userprogrammable != "YES") return false;
   for(size_t i=0; i<optcmdname.size(); i++) {
     MMessage m;
     rulename = "_"+optcmdname[i];
     if(getCategory(optcmdname[i]) == "SH") rule_set = optcmdname[i]+"_up";
     else rule_set = optcmdname[i]+"_on";
     if(getCategory(optcmdname[i]) == "SH") rule_reset = optcmdname[i]+"_down";
     else rule_reset = optcmdname[i]+"_off";
     alias = optcmdname[i];
     m.add("$NAME", rulename);
     m.add("$ALIAS", alias);
     m.add("$SETACTION", rule_set);
     m.add("$RESETACTION", rule_reset);
     mkItem(m, "timedrules", "userprogrammable.xml");
   }
   return true;
}
