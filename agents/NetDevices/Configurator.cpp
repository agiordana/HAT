//
//  Configurator.cpp
//  mngagent
//
//  Created by Attilio Giordana on 1/10/15.
//  Copyright (c) 2015 Università del Piemonte Orientale. All rights reserved.
//

#include "Configurator.h"
#include<bits/stdc++.h>

using namespace std;

boost::mutex Configurator::mutex;

Configurator::Configurator(string n): RouterExec(n) {
   MParams p("threads", "Configurator");
   p.load();
   sensor_category = p.get("sensor_category");
   cmd_category = p.get("cmd_category");
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

    if(params.back() == "adddevice") {
	response = mkItem(msg, "devices");
        mkManifest(msg);
    }
    else if(params.back() == "reboot") {
	response = "done";
	exit(1);
    }
    else if(params.size()==3 && params[1] == "delete") {
        response = deleteItem(params.back());
    }
    else if(params.back() == "devicelist") {
        response = deviceList();
    }
    else if(params.back() == "componentlist") {
        response = componentList();
    }
    else if(params.back() == "inputlist") {
        response = inputList();
    }
    mresp.add("body", response);
    return mresp;
}  

string Configurator::deviceList() {
  string res = "";
  vector<string> devlist;
  string dirpath = hsrv::configdir+"/devices";
  FileManager::dirList(dirpath,devlist);
  for(size_t i=0; i<devlist.size(); i++) {
    if(res != "") res += "+";
    res += devlist[i];
  }
  return res;
}

string Configurator::componentList() {
  vector<string> tempname;
  string res = "";
  NameList ccat;
  componentname.clear();
  ccat.init(cmd_category, '+');
  for(size_t i=0; i<ccat.size(); i++) {
    tempname = NameService::getNames("devices", ccat[i]);
    for(size_t j=0; j<tempname.size(); j++) {
        componentname.push_back(tempname[j]);
    }
  }
  for(size_t i=0; i<componentname.size(); i++) {
    if(res != "") res += "+";
    res += componentname[i];
  }
  return res;
}

string Configurator::inputList() {
  vector<string> tempname;
  string res = "";
  inputname.clear();
  NameList scat;
  scat.init(sensor_category, '+');
  for(size_t i=0; i<scat.size(); i++) {
    tempname = NameService::getNames("devices", scat[i]);
    for(size_t j=0; j<tempname.size(); j++) {
      inputname.push_back(tempname[j]);
    }
  }
  for(size_t i=0; i<inputname.size(); i++) {
    if(res != "") res += "+";
    res += inputname[i];
  }
  return res;
}


string Configurator::mkItem(MMessage& m, string path) {
  ifstream devtemplate;
  ifstream controltemplate;
  ifstream componenttemplate;
  ofstream device;
  ofstream control;
  ofstream component;
  string description = "";
  string name;
  string devtype;
  string prefix;
  string oldvalue, newvalue;
  string templatepath = hsrv::configdir+"/templates/device.xml";
  string devicepath = hsrv::configdir+"/"+path;
  string devicefile;
  string controlfile;
  string componentfile;
  char ch;
  name = m.getString("name");
  if(name == "") name = m.getString("$NAME");
  if(name == "") return "Failed";
  prefix = m.getString("$PREFIX");
  if(prefix == "") prefix = "LI";
  devtype = m.getString("$TYPE");
  if(devtype=="") devtype="onoff";
  FileManager::makeDir(devicepath,true);
  devicepath = devicepath + "/"+prefix+"_"+name;
  FileManager::makeDir(devicepath,true);
  devicefile = devicepath + "/"+prefix+"_"+name + ".xml";

  devtemplate.open(templatepath.c_str());
  if(!devtemplate.is_open()) return "Failed";
  while((ch=devtemplate.get())&&!devtemplate.eof()) {
    description+=ch;
  }
  devtemplate.close();

  device.open(devicefile.c_str());
  if(!device.is_open()) {
    hsrv::logger->info("Impossible to save: "+devicepath);
    return "Failed";
  }
  MMessage::iterator it;
  for(it=m.begin(); it!=m.end(); it++) {
    oldvalue = it->first;
    newvalue = m.getString(it->first);
    if(oldvalue[0]=='$') while(hsrv::strReplace(description,oldvalue,newvalue));
  }

  for(size_t i=0; i<description.size(); i++) device.put(description[i]);
  device.close();

//make control description;
  componentname.init(m.getString("$COMPONENT"));
  if(componentname.size()==0) {
    hsrv::logger->info("Wrong number of component!!!!");
    return "Failed";
  }
  description = "";
  templatepath = hsrv::configdir+"/templates/control.xml";
  controlfile = devicepath + "/control.xml";
  controltemplate.open(templatepath.c_str());
  if(!controltemplate.is_open()) return "Failed";
  while((ch=controltemplate.get())&&!controltemplate.eof()) {
    description+=ch;
  }
  controltemplate.close();

  for(it=m.begin(); it!=m.end(); it++) {
    oldvalue = it->first;
    newvalue = m.getString(it->first);
    if(oldvalue[0]=='$') while(hsrv::strReplace(description,oldvalue,newvalue));
  }
  description = mkControl(description, devtype, componentname.size());
  control.open(controlfile.c_str());
  for(size_t i=0; i<description.size(); i++) control.put(description[i]);
  control.close();
  
  // make the component
  templatepath = hsrv::configdir+"/templates/component.xml";
  for(size_t i=0; i<componentname.size(); i++) {
     componentfile = devicepath + "/"+componentname[i]+".xml";
     componenttemplate.open(templatepath.c_str());
     if(!componenttemplate.is_open()) return "Failed";
     description = "";
     while((ch=componenttemplate.get())&&!componenttemplate.eof()) {
       description+=ch;
     }
     componenttemplate.close();
     componenttemplate.clear();
     while(hsrv::strReplace(description,"$NAME",componentname[i]));
     hsrv::strReplace(description,"$ORDER", hsrv::unsigned2a(i));
     component.open(componentfile.c_str());
     for(size_t j=0; j<description.size(); j++) component.put(description[j]);
     component.close();
     component.clear();
  }
  
  return "done";
}

string Configurator::getCategory(string& name) {
  return name.substr(0,2);
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
   paths.push_back(hsrv::configdir+"/rulemanifest/");
   paths.push_back(hsrv::homedir+"/actions/");
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
   string prefix = m.getString("$PREFIX");
   string manifestpath = hsrv::configdir+"/devicemanifest/";
   if(!FileManager::isDir(manifestpath)) FileManager::makeDir(manifestpath, true);
   manifestpath += prefix;
   manifestpath += "_";
   manifestpath += name;
   manifestpath += ".xml";
   manifest.open(manifestpath.c_str());
   m.xmlPrint(tmanifest,1);
   manifest<<tmanifest.str();
   manifest.close();
   return true;
}

string Configurator::deleteItem(string& name) {
   string fname = name +".xml";
   string manifestpath = hsrv::configdir+"/devicemanifest";
   string devicepathc = hsrv::configdir+"/devices/"+name;
   string devicepathh= hsrv::homedir+"/devices/"+name;
   if(!FileManager::isDir(devicepathc)) return "Failed";
   FileManager::deleteFile(manifestpath, fname); 
   string cmd = "/bin/rm -r "+devicepathc;
   hsrv::cmdExec(cmd);
   if(!FileManager::isDir(devicepathh)) return "done";
   cmd = "/bin/rm -r "+devicepathh;
   hsrv::cmdExec(cmd);
   return "done";
}

string Configurator::mkControl(string& ctempl, string& devtype, unsigned n) {
   string res = ctempl;
   string current="";
   string next="";
   string control = "";
   if(devtype == "onoff") {
     for(unsigned i=0; i<n; i++) {
       if(i>0) {
	  current += "+";
	  next += "+";
       }
       current += "OFF";
       next += "ON";
     }
     control = "<attribute name=\"";
     control += current;
     control += "\" category=\"control\" value=\"";
     control += next;
     control += "\"/>\n";
     control += "<attribute name=\"";
     control += next;
     control += "\" category=\"control\" value=\"";
     control += current;
     control += "\"/>"; 
     hsrv::strReplace(res, "$CONTROL", control);
   }
   else if(devtype == "cyclic") {
     vector<string> gray;
     generateGrayCode((int)n, gray);
     for(size_t i=0; i<gray.size(); i++) {
       current = gray[i];
       next = (i<gray.size()-1 ? gray[i+1] : gray[0]);
       if(control != "") control += '\n';
       control += "<attribute name=\"";
       control += current;
       control += "\" category=\"control\" value=\"";
       control += next;
       control += "\"/>";
     }
     hsrv::strReplace(res, "$CONTROL", control);
   }
   else hsrv::logger->info("CONFIGURATOR: "+devtype+"not supported");  
   return res;
}

bool Configurator::generateGrayCode(int N, vector<string>& codes) {
 
    N = pow(2,N);
    string grey[N];
    int i,j,n=2;
    grey[0] ="OFF";
    grey[1] ="ON";
    while(n<N) {
        n = n*2;
        for(i=n/2,j=n/2-1;i<n && j>=0;i++,j--)
            grey[i] = grey[j];
        for(i=0;i<n/2;i++)
            grey[i] = "OFF+" + grey[i];
        for(i=n/2;i<n;i++)
            grey[i] = "ON+" + grey[i];
    }
    for(i=0;i<n;i++) {
        codes.push_back(grey[i]);
    }
    return true;
}

