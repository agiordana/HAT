/* 
 * File:   LoggerRoute.h
 * Author: andrea
 *
 * Created on 10 aprile 2015, 15.29
 */

#ifndef LOGGERROUTE_H
#define	LOGGERROUTE_H

#include "Route.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

class LoggerRoute : public RouterExec {
public:        
    LoggerRoute(std::string n="logroute") : RouterExec(n) {};
    MMessage exec(std::string uri, std::vector<std::string> params, std::string method, std::string body="");

private:
   std::string getDevStatus(std::string devname);
   std::string getCatStatus(std::string devname);
   std::string getDimmerStatus(std::string devname);
   std::string getShutterStatus(std::string devname);
   std::string getLog(MMessage m);
};

#endif	/* LOGGERROUTE_H */

