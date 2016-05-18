/* 
 * File:   RpcServer.h
 * Author: attilio
 *
 * Created on July 8, 2015, 7:15 AM
 */

#ifndef RPCSERVER_H
#define	RPCSERVER_H
#include "agentlib.h"
#include "Route.h"
#include "DeviceManager.h"
#include "AlarmManager.h"
#include "AutomatismManager.h"
#include "LogManager.h"
#include "PwrManager.h"
#include "SrvManager.h"
#include "AuthManager.h"

class RpcServer: public RouterExec {
public:
    RpcServer(std::string n="rpcserver"): RouterExec(n){};
    MMessage exec(std::string uri, std::vector<std::string> params, std::string method, std::string body="");
};

#endif	/* RPCSERVER_H */

