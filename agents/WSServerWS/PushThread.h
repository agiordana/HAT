//
//  PushThread.h
//  logger
//
//  Created by Attilio Giordana on 5/13/14.
//  Copyright (c) 2014 Università del Piemonte Orientale. All rights reserved.
//

#ifndef WS_PushThread_h
#define WS_PushThread_h
#include "agentlib.h"
#include "ObserverThread.h"

class PushThread: public ObserverThread {
public:
    PushThread(std::string n);
    virtual ~PushThread();
    void start_service_threads();
private:
    std::string name;
    std::string port;
    std::string wsurl;
    std::string logfile;
    void do_signal(PushThread* obj);
    std::string notifyEvent(MMessage&);
    std::string notifyDeviceChange(MMessage&);
    std::string notifyProgramChange(MMessage&);
    std::string notifyCmdChange(MMessage&);
    std::string notifyAlarm(MMessage&);
    std::string convertValue(std::string&);
    std::string remapDimmer(std::string);
    string logMessage(std::string, std::string&, string logfile="");
};

#endif
