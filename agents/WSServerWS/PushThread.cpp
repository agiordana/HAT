//
//  File.cpp
//  logger
//
//  Created by Attilio Giordana on 5/13/14.
//  Copyright (c) 2014 Università del Piemonte Orientale. All rights reserved.
//

#include <sys/types.h>

#include "PushThread.h"

#include <assert.h>
#include "easywsclient.hpp"
#include <memory>

using namespace std;


PushThread::~PushThread() {
}

PushThread::PushThread(string name) {
    id = name;
    name = name;
    MParams params("threads/PushThread");
    params.load();
    
    MParams nparams("agentnet");
    nparams.load();
    string httpport = nparams.get("port");
    int ipushport = hsrv::a2int(httpport);
    ipushport++;
    port = hsrv::int2a(ipushport);
    wsurl = params.get("wsurl");
    logfile = params.get("logfile");
    if(logfile == "") logfile = hsrv::homedir+"/"+logfile;
    hsrv::strReplace(wsurl,"$PORT",port);
}

void PushThread::start_service_threads() {
    this->service_thread.push_back(new Thread(&PushThread::do_signal, this));
}

void PushThread::do_signal(PushThread* obj) {
   using easywsclient::WebSocket;
   static WebSocket::pointer ws = NULL;

   ws=WebSocket::from_url(this->wsurl);
   assert(ws);
   MMessage msg;
   string push_message;
   while(true) {
	msg = obj->receive_message();
	if(msg.mtype == "logcmd" && msg.msubtype == "archive") {
	   push_message = notifyEvent(msg);
	   if(push_message != "") {
		ws->send(push_message);
		ws->poll();
	   }
	}
   }
}

string PushThread::notifyEvent(MMessage& m) {
   string featurename = "";
   string info = m.mtype + " " +m.msubtype + " " +m.getString("class");
   if(m.mtype == "logcmd" && m.msubtype == "archive" &&
        (m.getString("class")=="event"||m.getString("class")=="program"||m.getString("class")=="cmd"||m.getString("class")=="alarm")) {
      if(m.getString("feature")=="") return "";
      featurename = m.getString("feature");
      if(m.getString("class") == "alarm") return notifyAlarm(m);    //Alarm
      if(featurename == "prgstatus") return notifyProgramChange(m); //Program change
      if(m.getString("class") == "cmd") return notifyCmdChange(m);
      else return notifyDeviceChange(m);
   }
   return "";
}

string PushThread::notifyProgramChange(MMessage& m) {
   string cname;
   string mode;
   string program_name;
   string active;
   cname = m.getString("source");
   if(cname == "") cname = m.getString("agent");
   mode = m.getString("status");
   program_name = m.getString("program");
   active = m.getString("program_active");
   string msg = "{\"device\": {\"name\":\"" + cname +"\",\"mode\":\""+mode+"\",\"active\":\""+active+"\"}}\n";
   return logMessage(m.getString("class"),msg,logfile);
}

string PushThread::notifyDeviceChange(MMessage& m) {
   NameList namefull;
   string featurename = "";
   string cname;
   string name;
   string device;
   string state;
   string mode;
   string value;
   string msg = "";
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
                if(namefull[2]!="state") return "";
                if((device=="LI"||device=="SO"||device=="SA"||device=="GA"||device=="GV" ||device=="IR")) value = convertValue(value);
                else if(device=="DI") value = remapDimmer(value);
                else if(device=="SH" && value == "UP") value = "up";
                else if(device=="SH" && value=="DOWN") value = "down";
                else if(device=="SH" && value=="OFF") value = "off";
                break;
          otherwise: return "";
          }
   string color = m.getString("color");

   if(color=="") msg = "{\"device\": {\"name\":\"" + cname +"\",\"status\":\""+value+"\"}}\n";
     else msg = "{\"device\": {\"name\":\"" + cname +"\",\"status\":\""+value+"\",\"color\":\""+color+"\"}}\n";

   return logMessage(m.getString("class"),msg,logfile);
}

string PushThread::remapDimmer(string value) {
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

string PushThread::notifyAlarm(MMessage& m) {
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

   return logMessage(m.getString("class"),msg,logfile);
}


string PushThread::notifyCmdChange(MMessage& m) {
   char buffer[MAXB];
   string cname;
   string action;
   string value;
   size_t count=0;
   cname = m.getString("agentname");
   action = m.getString("feature");
   value = m.getString("value");
   if(cname == "Alarm") return "";
   if(value == "ON" || value == "1") value = "true";
   else {
        if(value == "OFF" || value == "0") value = "false";
   }
   string msg = "{\"device\": {\"name\":\"" + cname +"\",\"action\":\""+action+"\",\"value\":\"" + value +"\"}}\n";

   return logMessage(m.getString("class"),msg,logfile);
}

string PushThread::logMessage(string ty, string& m, string logfile) {
   ostringstream out;
   string time = "20" + hsrv::getasciitimecompact();
   string tolog = m.substr(3);
   tolog = tolog.substr(tolog.find("{")+1);
   tolog = tolog.substr(0,tolog.find("}")+1);
   out<<"{\"time\":\""<<time<<"\",\"type\":\""<<ty<<"\","<<tolog<<endl;
   if(logfile!="") {
       ofstream logger_out(logfile.c_str(), ofstream::app);
       if(logger_out.is_open()) {
         logger_out<<"{\"time\":\""<<time<<"\",\"type\":\""<<ty<<"\","<<tolog<<endl;
         logger_out.close();
       }
   }
   return out.str();
}

string PushThread::convertValue(string& v) {
   NameList value;
   string res;
   value.init(v,'+');
   for(size_t i=0; i<value.size(); i++) {
      if(res != "") res+= '+';
      if(value[i] == "ON" || value[i] == "1") res+="true";
      else if(value[i] == "OFF" || value[i] == "0") res+="false";
      else res+=value[i];
   }
   return res;
}
