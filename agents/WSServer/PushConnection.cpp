//
//  PushConnection.cpp
//  logger
//
//  Created by Attilio Giordana on 5/12/14.
//  Copyright (c) 2014 Università del Piemonte Orientale. All rights reserved.
//

#include "PushConnection.h"
#define TOUTIAA 5
using namespace std;

//Push Connection creation
PushConnection::PushConnection(int fd) {
    mode = "*";
    connection = fd;
    string msg = getJsonMessage();
    getRegistration(msg);
    logfile = "";
    logpush = "";
}

PushConnection::PushConnection(string file, string events) {
    logfile = hsrv::homedir+"/logs/" + file;
    logpush = hsrv::homedir+"/LOGPUSH";
    logevents.init(events,'+');
    FileManager::makeDir(logfile,true);
    logfile += "/data";
}

string PushConnection::getJsonMessage() {
    string msg="";
    char inbuffer[MAXB];
    bool end=false;
    int timeout=0;
    size_t n;
    for(int i=0; i<MAXB; i++) inbuffer[i]=0;
    if (connection<=0) return ""; 
    while (!end && (timeout=dataAvailable())>0 && (n=read(connection, inbuffer, MAXB))>0) {
        for(int i=0; i<n && !end; i++) {
            if(inbuffer[i]!='\n') msg+=inbuffer[i];
            else end = true;
	}
    }
    if (end) {
        return msg;
    }
    else {
	stringstream info;
	string m = string(inbuffer);
        info<<"non JSON message arrived: ("<<m.size()<<") "<<m;
        hsrv::logger->info(info.str(),__FILE__, __FUNCTION__, __LINE__);
        return "";
    }
}

int PushConnection::dataAvailable() {
	int rc;
	fd_set fds;
	struct timeval tv;
	
	tv.tv_sec = TOUTIAA;
	tv.tv_usec = 0;
	
	FD_ZERO(&fds);
	FD_SET(connection,&fds);
	rc = select(connection+1, &fds, NULL, NULL, &tv);
	
    if (rc < 0) 
        return -1;
    
    return FD_ISSET(connection,&fds) ? 1 : 0;
}

bool PushConnection::show() {
   map<string,string>::iterator it;
   cout<<"CLIENT: "<<port<<":"<<address<<" Connection: "<<connection<<endl;
   cout<<"---- event map ----------"<<endl;
   for(it=events.begin(); it!=events.end(); it++) cout<<it->first<<": "<<it->second<<endl;
   cout<<"-------------------------"<<endl;
   return true;
}

//genera una descrizione della connessione in forma di stringa
string PushConnection::describe() {
  map<string,string>::iterator it;
  string description = "<"+address+":"+port+">[";
  for(it=events.begin(); it!=events.end(); it++) description = description+it->first+"; ";
  description += "]";
  return description;
}

bool PushConnection::AYAsend() {
   char buffer[MAXB];
   int count;
   sprintf(buffer,"{\"device\":{\"name\":\"AYA\"}}\n");
   count = write(connection,buffer,strlen(buffer));
   if(count>0) return true;
   else return false;
}

bool PushConnection::IAAreceive() {
    string msg = getJsonMessage();
    if(msg=="") return false;
    if(isRegistration(msg)) {
       getRegistration(msg);
       return true;
    }
    return isIAA(msg);
}

bool PushConnection::notifyEvent(MMessage& m) {
   string featurename = "";
   string info = m.mtype + " " +m.msubtype + " " +m.getString("class");
   if(m.mtype == "logcmd" && m.msubtype == "archive" &&
	(m.getString("class")=="event"||m.getString("class")=="program"||m.getString("class")=="cmd"||m.getString("class")=="alarm")) {
      if(m.getString("feature")=="") return false;
      featurename = m.getString("feature");
      if(m.getString("class") == "alarm") return notifyAlarm(m);    //Alarm
      if(featurename == "prgstatus") return notifyProgramChange(m); //Program change
      if(m.getString("class") == "cmd") return notifyCmdChange(m);
      else return notifyDeviceChange(m);
   }
   return false;
}

bool PushConnection::notifyProgramChange(MMessage& m) {
   char buffer[MAXB];
   string cname;
   string mode;
   string program_name;
   string active;
   int count = 0;
   cname = m.getString("source");
   if(cname == "") cname = m.getString("agent");
   mode = m.getString("status");
   program_name = m.getString("program");
   active = m.getString("program_active");
   string msg = "{\"device\": {\"name\":\"" + cname +"\",\"mode\":\""+mode+"\",\"active\":\""+active+"\"}}\n";
   if(logpush != "") logMessage(m.getString("class"),msg,logpush);
   if(logfile!="") {
	if(isToLog(m.getString("class"))) count = logMessage(m.getString("class"),msg,logfile);
   }
   else if(checkRegister(cname)) {
         sprintf(buffer,"%s", msg.c_str());
         count = write(connection, buffer, strlen(buffer));
   }
   return count>0;
}
  

bool PushConnection::notifyDeviceChange(MMessage& m) {
   NameList namefull;
   string featurename = "";
   string cname;
   string name;
   string device;
   string state;
   string mode;
   char buffer[MAXB];
   string value;
   string value1 = "";
   string value2 = "";
   int count = 0;
   featurename = m.getString("feature");
   namefull.init(featurename,'_');
   value=m.getString("value");
   if(namefull.size()>=2) {
	  name = namefull[1];
	  device = namefull[0];
   }
   else {
	  name = namefull[0];
	  device = "XX";
   }
   cname = device+"_"+name;
   switch((int)namefull.size()) {
	  case 1:
	  case 2:
		break;
	  case 3:
		if(namefull[2]!="state") return false;
                if((device=="LI"||device=="SO"||device=="SA"||device=="GA"||device=="GV" ||device=="IR") && (value == "1"||value == "ON")) value = "true";
                else if((device=="LI"||device=="SO"||device=="SA"||device=="GA"||device=="GV" ||device=="IR") && (value == "0"||value == "OFF")) value = "false";
		else if(device=="DI") value = remapDimmer(value);
		else if(device=="SH" && value == "UP") value = "up";
		else if(device=="SH" && value=="DOWN") value = "down";
		else if(device=="SH" && value=="OFF") value = "off";
		break;
	  otherwise: return false;
          }
   string color = m.getString("color");
   string msg;
   if(color=="") msg = "{\"device\": {\"name\":\"" + cname +"\",\"status\":\""+value+"\"}}\n";
   else msg = "{\"device\": {\"name\":\"" + cname +"\",\"status\":\""+value+"\",\"color\":\""+color+"\"}}\n";
   if(logpush != "") logMessage(m.getString("class"),msg,logpush);
   if(logfile!="") {
      if(isToLog(cname)) count = logMessage(m.getString("class"),msg,logfile);
   }
   else if(events.count(cname)>0) {
         sprintf(buffer,"%s", msg.c_str());
         count = write(connection, buffer, strlen(buffer));
   }
   return count>0;
}

string PushConnection::remapDimmer(string value) {
   double val = hsrv::a2double(value);
   if(val<1) return "0";
   if(val >91) return "100";
   if(val >81) return "90";
   if(val >71) return "80";
   if(val >61) return "70";
   if(val >51) return "60";
   if(val >41) return "50";
   if(val >31) return "40";
   if(val >21) return "30";
   if(val >11) return "20";
   if(val >1) return "10";
   return "0";
}

bool PushConnection::notifyAlarm(MMessage& m) {
   char buffer[MAXB];
   string cname;
   string alarm;
   string source;
   string video = "";
   size_t count=0;
   cname = m.getString("agentname");
   alarm = m.getString("feature");
   source = m.getString("source");
   video = m.getString("video");
   string msg = "{\"device\": {\"name\":\"" + cname +"\",\"alarm\":\""+alarm+"\",\"source\":\"" + source +"\",\"video\":\"" + video +"\"}}\n";

   if(logpush != "") logMessage(m.getString("class"),msg,logpush);
   if(logfile!="") {
         if(isToLog(m.getString("class"))) count = logMessage(m.getString("class"),msg,logfile);
   }
   else if(checkRegister(cname)) {
      sprintf(buffer,"%s", msg.c_str());
      count = write(connection, buffer, strlen(buffer));
   }
   return count>0;
}

bool PushConnection::notifyCmdChange(MMessage& m) {
   char buffer[MAXB];
   string cname;
   string action;
   string value;
   size_t count=0;
   cname = m.getString("agentname");
   action = m.getString("feature");
   value = m.getString("value");
   if(cname == "Alarm") return false;
   if(value == "ON" || value == "1") value = "true";
   else {
        if(value == "OFF" || value == "0") value = "false";
   }
   string msg = "{\"device\": {\"name\":\"" + cname +"\",\"action\":\""+action+"\",\"value\":\"" + value +"\"}}\n";

   if(logpush != "") logMessage(m.getString("class"),msg,logpush);
   if(logfile!="") {
	if(isToLog(action)) count = logMessage(m.getString("class"),msg,logfile);
   }
   else if(checkRegister(cname)) {
       sprintf(buffer,"%s", msg.c_str());
       count = write(connection, buffer, strlen(buffer));
   }
   return count>0;
}

bool PushConnection::checkRegister(string name) {
    if(events.count(name)==0) return false;
    else if(mode == "*") return true;
    else return mode == events[name];
}

bool PushConnection::isRegistration(string msg) {
    if (msg.find("devices") != string::npos) {
        return true;
    }
    else return false;
}

bool PushConnection::isIAA(string msg) {
    if (msg.find("IAA") != string::npos) {
        return true;
    }
    else return false;
}

bool PushConnection::getRegistration(string msg) {
    JSONNode n;
    const JSONNode* c;
    string cname;
    string value;

    if(msg == "") return false;
    events.clear();
    n = libjson::parse(msg.c_str());
    JSONNode::const_iterator i = n.begin();
    while(i!=n.end()&&cname!="devices") {
        cname = i->name();
        if(cname!="devices") i++;
    }
    if(cname=="devices") {
        c = &(*i);
        for(i= c->begin(); i!=c->end(); i++) {
            cname = i->begin()->name();
            value = (string)(i->begin()->as_string());
            events[value] = "*";
        }
    }
    return true;
}

int PushConnection::logMessage(string ty, string& m, string& logfile) {
   ofstream out(logfile.c_str(), ofstream::app);
   string time = "20" + hsrv::getasciitimecompact();
   string tolog = m.substr(3);
   tolog = tolog.substr(tolog.find("{")+1);
   tolog = tolog.substr(0,tolog.find("}")+1);
   out<<"{\"time\":\""<<time<<"\",\"type\":\""<<ty<<"\","<<tolog<<endl;
   out.close();
   return 1;
}

bool PushConnection::isToLog(string event) {
  if(logevents.member("all") || logevents.member(event))
	return true;
  else return false;
}
