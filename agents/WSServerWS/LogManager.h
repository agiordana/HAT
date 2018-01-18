/* 
 * File:   AutomatismManager.h
 * Author: attilio
 *
 * Created on July 6, 2015, 12:37 PM
 */

#ifndef LOGMANAGER_H
#define	LOGMANAGER_H
#include "agentlib.h"
#include "NameTable.h"
#include "Translator.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>


class LogManager {
public:
    static bool init(std::string dir);
    static MMessage execPost(std::vector<std::string>& params, std::string& method, std::string body="");
    static boost::mutex mutexclusion;
protected:
   static Translator* translator;
   static MParams par;
   static std::string getBody(std::string);
   static string mkRequest(std::string&, std::string&);
};

#endif	/* LOGMANAGER_H */
