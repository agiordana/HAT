/*
 * File:   NameSrvice.h
 * Author: attilio
 *
 * Created on September 22, 2015, 13:51 PM
 */

#ifndef NAMESERVICE_H
#define NAMESERVICE_H

#include "agentlib.h"
#include "agentnet.h"
#include "RpcCall.h"

class NameService {
public:
   static std::vector<std::string> getNames(std::string domain, std::string category = "");
   static bool checkName(std::string domain, std::string name);
private:
   static std::string getName(std::string&, std::string, std::string, std::size_t&);
   static std::string extractValue(std::string&, std::string, size_t);
};

#endif /* NAMESERVICE_H */
