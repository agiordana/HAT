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
    JSONNode n;
    const JSONNode* c;
    string cname;
    string value;
    int j = 0;
    connection = fd;
    string msg = getJsonMessage();
    if(msg != "");
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
	   events[value] = true;
        }
    }
}


string PushConnection::getJsonMessage() {
    string msg="";
    char inbuffer[MAXB];
    bool end=false;
    int timeout=0;
    size_t n;
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
        hsrv::logger->info("non JSON message arrived",__FILE__, __FUNCTION__, __LINE__);
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
   map<string,bool>::iterator it;
   cout<<"CLIENT: "<<port<<":"<<address<<" Connection: "<<connection<<endl;
   cout<<"---- event map ----------"<<endl;
   for(it=events.begin(); it!=events.end(); it++) cout<<it->first<<": "<<it->second<<endl;
   cout<<"-------------------------"<<endl;
   return true;
}

//genera una descrizione della connessione in forma di stringa
string PushConnection::describe() {
  map<string,bool>::iterator it;
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
   if(msg!="") return true;
   else return false;
}

bool PushConnection::notifyEvent(MMessage& m) {
   string featurename = "";
   if(m.mtype == "logcmd" && m.msubtype == "archive" && (m.getString("class")=="event"||m.getString("class")=="program")) {
      if(m.getString("feature")=="") return false;
      featurename = m.getString("feature");
      if(featurename == "setprogram") 
	  return notifyProgramChange(m);
      else return notifyDeviceChange(m);
   }
   return false;
}

bool PushConnection::notifyProgramChange(MMessage& m) {
   char buffer[MAXB];
   string cname;
   string mode;
   string program_name;
   int count = 0;
   cname = m.getString("source");
   mode = m.getString("mode");
   program_name = m.getString("value");
   if(events.count(cname)>0) {
         string msg = "{\"device\": {\"name\":\"" + cname +"\",\"mode\":\""+mode+"\",\"program_name:\"" + program_name +"\"}}\n";
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
		if(device=="LI" && value == "1") value = "true";
		else if(device=="LI" && value == "0") value = "false";
		else if(device=="DI") value = remapDimmer(value);
		break;
	  case 4:
		if(device=="SH" && namefull[2]!="up" && namefull[1]!="down") return false;
		if((value=="0" || value=="OFF")&&(namefull[2]=="up"||namefull[2]=="down")) value = "off";
		else value=namefull[2];
		break;
	  otherwise: return false;
          }
   if(events.count(cname)>0) {
         string msg = "{\"device\": {\"name\":\"" + cname +"\",\"status\":\""+value+"\"}}\n";
         sprintf(buffer,"%s", msg.c_str());
         count = write(connection, buffer, strlen(buffer));
   }
   return count>0;
}

string PushConnection::remapDimmer(string value) {
   double val = hsrv::a2double(value);
   if(val<1) return "0";
   if(val >1000) return "100";
   if(val >500) return "80";
   if(val >200) return "60";
   if(val >100) return "40";
   if(val >50) return "20";
   return "0";
}
