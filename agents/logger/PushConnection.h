//
//  PushConnection.h
//  logger
//
//  Created by Attilio Giordana on 5/12/14.
//  Copyright (c) 2014 Università del Piemonte Orientale. All rights reserved.
//

#ifndef logger_PushConnection_h
#define logger_PushConnection_h
#include "agentlib.h"
#include "../libjson/libjson.h"


class PushConnection {
private:
    int connection;
    int ayaPeriod;
    std::map<std::string, bool> events;
    std::string getJsonMessage();
    int dataAvailable();
    bool notifyDeviceChange(MMessage&);
    bool notifyProgramChange(MMessage&);
public:
    PushConnection(int fd);
    /*
     ** notifica la lista degli eventi contenuti nel messaggio
     ** Ogni evento è una coppia <nome, valore>
     */
    bool notifyEvent(MMessage&);
    bool AYAsend();
    bool IAAreceive();
    std::string address;
    std::string port;
    bool show();
    std::string describe();
    int ttl;
    bool shutDown() {close(connection); return true; };
    std::string remapDimmer(std::string);
};

#endif
