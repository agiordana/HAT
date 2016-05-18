//
//  PushThread.h
//  logger
//
//  Created by Attilio Giordana on 5/13/14.
//  Copyright (c) 2014 Università del Piemonte Orientale. All rights reserved.
//

#ifndef logger_PushThread_h
#define logger_PushThread_h
#include "agentlib.h"
#include "ObserverThread.h"
#include "PushConnection.h"
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/condition.hpp>

class PushThread: public ObserverThread {
public:
    PushThread(std::string n);
    virtual ~PushThread();
    void start_service_threads();
private:
    PushConnection* client;
    PushConnection* logconnection;
    sockaddr_in addr;
    sockaddr_in client_addr;
    int rvport;
    std::string name;
    std::string port;
    void do_register(PushThread* obj);
    void do_poll(PushThread* obj);
    void do_signal(PushThread* obj);
    bool clientInsert(PushConnection* client);
    bool AYAsend();
    bool clearExpired();
    bool notifyEvent(MMessage&);
    std::vector<PushConnection> connections;
    bool setup();
    unsigned short portaddr(std::string&);
    long pollperiod;
    int maxttl;
    static boost::mutex mutexsignal;
    static boost::mutex mutexpoll;
    MMessage* page;
    bool mkStatusPage(std::string);
    bool updateStatusPage(std::string client, std::string mode="add");
};

#endif
