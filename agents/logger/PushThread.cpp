//
//  File.cpp
//  logger
//
//  Created by Attilio Giordana on 5/13/14.
//  Copyright (c) 2014 Università del Piemonte Orientale. All rights reserved.
//

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include "PushThread.h"

using namespace std;
boost::mutex PushThread::mutexsignal;
boost::mutex PushThread::mutexpoll;


PushThread::~PushThread() {
}

PushThread::PushThread(string name) {
    id = name;
    name = name;
    MParams params("threads/PushThread");
    params.load();
    string tb = params.get("maxttl", "generic");
    if(tb!="") maxttl = hsrv::a2int(tb);
    else maxttl = 2;
    tb = params.get("pollperiod", "generic");
    if(tb!="") pollperiod = hsrv::a2long(tb);
    
    this->page = new MMessage("PushService");
    
    MParams nparams("agentnet");
    nparams.load();
    string httpport = nparams.get("port");
    int ipushport = hsrv::a2int(httpport);
    ipushport++;
    port = hsrv::int2a(ipushport);
    rvport = 0;
    mkStatusPage(port);

    signal(SIGPIPE, SIG_IGN); 
}

void PushThread::start_service_threads() {
    this->service_thread.push_back(new Thread(&PushThread::do_register, this));
    this->service_thread.push_back(new Thread(&PushThread::do_poll, this));
    this->service_thread.push_back(new Thread(&PushThread::do_signal, this));
}

void PushThread::do_register(PushThread* obj) {

    int fd;
    if(!setup()) {
        hsrv::logger->info("rvport activation failed", __FILE__, __FUNCTION__, __LINE__);
    }
    client = NULL;
    while(1) {
	socklen_t addrlen = sizeof(client_addr);
        fd = accept(rvport, (struct sockaddr *)&client_addr, &addrlen);
        client = new PushConnection(fd);
	client->address = hsrv::unsigned2a(ntohl(client_addr.sin_addr.s_addr));
	client->port = hsrv::unsigned2a((unsigned)ntohs(client_addr.sin_port));
	client->ttl = obj->maxttl;
	clientInsert(client);
 	updateStatusPage(connections[connections.size()-1].describe(), "add");
        delete client;
	client = NULL;
    }
}

bool PushThread::clientInsert(PushConnection* client) {
	boost::unique_lock<boost::mutex> lock(mutexsignal);
	boost::unique_lock<boost::mutex> lock1(mutexpoll);
	connections.push_back(*client);
	return true;
}

bool PushThread::AYAsend() {
       for(unsigned i=0; i<connections.size(); i++) {
              boost::unique_lock<boost::mutex> lock(mutexpoll);
              if(!connections[i].AYAsend()) connections[i].ttl=-1;
              else {
                      if(connections[i].IAAreceive()) connections[i].ttl = maxttl;
                      else connections[i].ttl--;
              }
        }
	return true;
}


bool PushThread::clearExpired() {
   vector<PushConnection>::iterator it;
   bool erased = false;
   boost::unique_lock<boost::mutex> lock(mutexsignal);
   boost::unique_lock<boost::mutex> lock1(mutexpoll);
   for(it=connections.begin(); !erased && it!=connections.end(); it++) 
         if(it->ttl<=0) {
              it->shutDown();
	      updateStatusPage(it->describe(), "delete");
              connections.erase(it);
              erased = true;
         }
   return true;
}



void PushThread::do_poll(PushThread* obj) {
   while(true) {
	AYAsend();
	clearExpired();
   	sleep(obj->pollperiod);
   }
}

void PushThread::do_signal(PushThread* obj) {
   MMessage msg;
   while(true) {
	msg = obj->receive_message();
	if(msg.mtype == "logcmd" && msg.msubtype == "archive") {
	   notifyEvent(msg);
	}
   }
}

bool PushThread::notifyEvent(MMessage& msg) {
   boost::unique_lock<boost::mutex> lock(mutexsignal);
   for(unsigned i=0; i<connections.size(); i++) {
      connections[i].notifyEvent(msg);
   }
   return true;
}

bool PushThread::setup() {
    int yes = 1;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(portaddr(port));
    
    if(rvport > 0)
        close(rvport);
    //setaddr();
    if((rvport = socket(AF_INET, SOCK_STREAM, 0)) < 0) return false;   
    if(bind(rvport, (struct sockaddr *)&addr, sizeof(addr)) <0) return false;
    listen(rvport, MAXCONN);
    setsockopt(rvport, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(yes));
    return true;
}

unsigned short PushThread::portaddr(string& port) {
        unsigned p;

        sscanf(port.c_str(),"%us", &p);

    return (unsigned short)p;
}

bool PushThread::mkStatusPage(string port) {
    page->add("port", port);
    page->add("language", "xml");
    AgentConf::pageRegister("/PushService.xml", *page);
    page->add("language", "html");
    AgentConf::pageRegister("/PushService", *page);
    return true;
}

bool PushThread::updateStatusPage(string client, string mode) {
    if(mode=="add") {
    	this->page->add(client, hsrv::getasciitimeday());
    }
    else {
	this->page->erase(client);
    }
    this->page->add("language", "xml");
    AgentConf::pageRegister("/PushService.xml", *page);
    this->page->add("language", "html");
    AgentConf::pageRegister("/PushService", *page);
    return true;
}
