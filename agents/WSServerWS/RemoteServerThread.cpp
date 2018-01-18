//
//  RemoteServerThread.cpp
//  logger
//
//  Created by Attilio Giordana on 8/27/14.
//  Copyright (c) 2014 Università del Piemonte Orientale. All rights reserved.
//

#include "RemoteServerThread.h"

using namespace std;

//---------------- class RemoteServer -----------------------------------

bool RemoteServer::load() {
    MParams::load();
    server = get("server");
    urlroot = get("urlroot");
    cmdtemplate = get("sendcmd");
    installer_code = get("installer_code");
    authentication = get("authentication");
    return true;
}

bool RemoteServer::restSend(string url, string msg) {
    if (server==""||server=="$SERVER"||urlroot==""||cmdtemplate=="") {
        return false;
    }
    string digest = "";
    string cmd = cmdtemplate;
    string resturl = urlroot+"/"+url;
    string res;
    string aespwd = hsrv::configdir+"/keys/"+installer_code;
    if(installer_code!=""&&authentication!="") {
        string timestamp = hsrv::double2a(hsrv::gettime());
        string text = timestamp + "<>" + msg;
        string kcmd = authentication;
        hsrv::strReplace(authentication, "$PASSWD", aespwd);
        digest = hsrv::cmdExec(authentication);
    }
    hsrv::strReplace(cmd, "$URL", resturl);
    hsrv::strReplace(cmd, "$SERVER", server);
    hsrv::strReplace(cmd, "$BODY", "\""+msg+"\"");
    if (digest != "") {
        cmd = cmd + " -d "+digest;
    }
    res = hsrv::cmdExec(cmd);
    if(res.find("200 OK")) return true;
        else return false;
}

//---------------- class RemoteServerThread -----------------------------

RemoteServerThread::RemoteServerThread(string n) {
    id = n;
    server = new RemoteServer("remoteserver");
    server->load();
}

RemoteServerThread::RemoteServerThread(const RemoteServerThread& orig) {
}

RemoteServerThread::~RemoteServerThread() {
}

void RemoteServerThread::start_service_threads() {
    this->service_thread.push_back(new Thread(&RemoteServerThread::do_work, this));
}

void RemoteServerThread::do_work(RemoteServerThread* obj) {
    MMessage msg;
    while (true) {
        msg = obj->receive_message();
        if(msg.mtype == "rscmd" && msg.msubtype == "serversend") {
            server->restSend(msg.getString("url"), msg.getString("item"));
        }
    }
}

