/* 
 * File:   RpcTable.h
 * Author: attilio
 *
 * Created on July 6, 2015, 12:37 PM
 */

#ifndef RPCWAITTABLE_H
#define	RPCWAITTABLE_H
#include "agentlib.h"

class RpcEntry {
public:
    RpcEntry(std::string);
    std::string tag;
    MMessageQueue<MMessage> waiting_queue;
    double rtime;
};

class RpcWaitTable {
public:
    RpcWaitTable();
    bool signal(MMessage&);
    bool checkTimeOut();
    std::string callRegister(MMessage&);
    MMessage wait(std::string);
protected:
    boost::mutex mutexLock;
    std::vector<RpcEntry*> pending;
    std::string tag;
    std::string getTag();
    int tagSequence; 
    MMessage mkComplain(std::string);
    RpcEntry* getEntry(std::string);
    bool clearEntry(RpcEntry*);
};

#endif	/* RPCTABLE_H */
