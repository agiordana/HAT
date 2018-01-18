/* 
 * File:   AutomatismManager.h
 * Author: attilio
 *
 * Created on July 6, 2015, 12:37 PM
 */

#ifndef PWRMANAGER_H
#define	PWRMANAGER_H
#include "agentlib.h"
#include "NameTable.h"
#include "Translator.h"
#include "JParser.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>

class DeviceDescriptor {
public:
  DeviceDescriptor(std::string a, std::string b, std::string c) {name = a; priority = b; status = c;};
  std::string name;
  std::string priority;
  std::string status;
};

class PwrManager {
public:
    static bool init(std::string dir);
    static MMessage execGet(std::vector<std::string>& params, std::string& method, std::string body="");
    static MMessage execPost(std::vector<std::string>& params, std::string& method, std::string body="");
    static boost::mutex mutexclusion;
protected:
   static MParams par;
   static NameList devCategories;
   static std::string getBody(std::string);
   static std::string mkAnswer(std::vector<DeviceDescriptor>&);
   static std::string getCategory(std::string file);
   static std::string sendCmd(std::string file);
   static bool sendFile(std::string, std::string file);
   static std::string fileDownload(std::string file);
   static void print(boost::property_tree::ptree const&);
   static void getDevices(boost::property_tree::ptree const&, std::vector<DeviceDescriptor>&, std::string);
   static void mkDevDescription(boost::property_tree::ptree const&, std::vector<DeviceDescriptor>&, std::string);
   static bool sortEval(const DeviceDescriptor& a, const DeviceDescriptor& b);
   static bool setDeviceStatus(std::string, std::string, std::string);
};

#endif	/* PWRMANAGER_H */
