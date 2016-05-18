/* 
 * File:   RpcCall.h
 * Author: attilio
 *
 * Created on July 7, 2015, 12:01 PM
 */

#ifndef RPCCALL_H
#define	RPCCALL_H

#include "agentlib.h"

class RpcCall: public IPlocation, public MParams {
public:
    std::string name;
    RpcCall(std::string n): MParams(n), IPlocation() { init(n); };
    RpcCall(std::string, std::string);
    std::string sendStringMessage(std::string&);
protected:
    bool init(std::string&);  
};

#endif	/* RPCCALL_H */

