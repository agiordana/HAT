/* 
 * File:   ServiceManager.h
 * Author: attilio
 *
 * Created on July 6, 2015, 12:37 PM
 */

#ifndef CONFMANAGER_H
#define	CONFMANAGER_H
#include "agentlib.h"
#include "NameTable.h"
#include "Translator.h"
#include "GuiLib.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>


class ConfManager {
public:
    static bool init(std::string dir);
    static MMessage execPut(std::vector<std::string>& params, std::string& method, std::string body="");
    static boost::mutex mutexclusion;
protected:
   static MParams par;
   static std::string getBody(std::string);
   static std::string getProto(std::string);
   static std::string mkButtons(std::map<std::string, BarHolder*>&);
   static std::string mkShowfun(std::map<std::string, BarHolder*>&);
   static std::string mkAreaList(std::map<std::string, BarHolder*>&);
   static std::string mkDevList(std::map<std::string, BarHolder*>&);
};

#endif	/* CONFMANAGER_H */
