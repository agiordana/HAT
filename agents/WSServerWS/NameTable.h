/* 
 * File:   NameTable.h
 * Author: attilio
 *
 * Created on July 10, 2015, 10:41 AM
 */

#ifndef NAMETABLE_H
#define	NAMETABLE_H

#include "agentlib.h"
#include "RpcCall.h"
#include "RpcCall.h"
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp> 
#include <boost/thread/locks.hpp> 
#include <boost/thread/condition.hpp> 


class AgentDesc {
public:
    std::string name;
    std::string port;
    std::string address;
    std::string time;
    std::vector<std::string> command;
    std::vector<std::string> event;
    bool print() {std::cout <<name<<" "<<address<<":"<<port<<" "<<time<<" "<<command.size()<<" "<<event.size()<<endl;};
};

class NameTable {
public:
    static bool reset();
    static bool update(MMessage&);
    static std::string notify(MMessage&);
    static std::string getstatus(MMessage&);
    static std::string forward(std::string&, std::string);
    static bool loadCategories();
    static bool setDevice(MMessage& msg, std::string agent = "");
    static MMessage getAgentDescription(std::string);
    static NameList getCategoryDevices(std::string);
protected:
    static std::vector<AgentDesc> agent;
    static std::map<std::string, std::size_t> namelookup;
    static bool extractCommand(AgentDesc&, std::string&);
    static bool extractEvent(AgentDesc&, std::string&);
    static bool addifnew(AgentDesc&);
    static bool matchAgent(std::string, std::size_t);
    static bool setDeviceStatus(std::string device, std::string mode);
    static std::string getBody(std::string&);
    static std::string getWord(std::string&, std::string, std::size_t&);
    static bool mkTable();
    static bool printTable(bool dp=false);
    static bool mkDeviceNames();
    static bool isDeviceAgent(AgentDesc&);
    static boost::mutex mutexclusion;
    static std::vector<std::string> category;
    static std::string getPage(std::string, AgentDesc&);
    static std::string getCategory(std::string);
    static std::string getDeviceStatus(std::string, std::string);
    static std::string cleanUp(std::string&);
};


#endif	/* COMMANDTABLE_H */

