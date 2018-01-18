//
//  RemoteServerThread.h
//  logger
//
//  Created by Attilio Giordana on 8/27/14.
//  Copyright (c) 2014 Università del Piemonte Orientale. All rights reserved.
//

#ifndef logger_RemoteServerThread_h
#define logger_RemoteServerThread_h
#include "agentlib.h"
#include "ObserverThread.h"
#include <openssl/rsa.h>

class RemoteServer: public MParams {
public:
    bool restSend(std::string url, std::string msg);
    bool setserver(std::string s) {server=s; return true;};
    RemoteServer(std::string f): MParams(f) {};
    bool load();
private:
    std::string server;
    std::string urlroot;
    std::string cmdtemplate;
    std::string installer_code;
    std::string authentication;
};

class RemoteServerThread: public ObserverThread {
public:
    RemoteServerThread(std::string name);
    
    RemoteServerThread(const RemoteServerThread& orig);
    
    virtual ~RemoteServerThread();
    
    /**
     * Inserisce i puntatori nel vettore delle thread e fa partire le thread
     * di servizio 
     */
    void start_service_threads();
    
    
private:
    /**
     * @var std::string
     * @brief nome observer
     */
    
    /**
     * @var std::string
     * @brief working directory utilizzata dall'archivio
     */
	std::string workingdir;
    
    RemoteServer* server;
    
    void do_work(RemoteServerThread* obj);
};

#endif
