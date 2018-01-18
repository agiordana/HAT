/* 
 * File:   ServiceManager.h
 * Author: attilio
 *
 * Created on July 6, 2015, 12:37 PM
 */

#ifndef SRVMANAGER_H
#define	SRVMANAGER_H
#include "agentlib.h"
#include "NameTable.h"
#include "Translator.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>


class SrvManager {
public:
    static bool init(std::string dir);
    static MMessage execPut(std::vector<std::string>& params, std::string& method, std::string body="");
    static MMessage execGet(std::vector<std::string>& params, std::string& method, std::string body="");
    static boost::mutex mutexclusion;
protected:
   static MParams par;
   static std::string getBody(std::string);
   static string mkRequest(std::string&, std::string&);
   static std::string macLastTime(std::string mac);
   static bool macTimeUpdate(std::string mac);
};

#endif	/* SRVMANAGER_H */
