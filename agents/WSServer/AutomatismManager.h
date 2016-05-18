/* 
 * File:   AutomatismManager.h
 * Author: attilio
 *
 * Created on July 6, 2015, 12:37 PM
 */

#ifndef AUTOMATISMMANAGER_H
#define	AUTOMATISMMANAGER_H
#include "agentlib.h"
#include "NameTable.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>


class AutomatismManager {
public:
    static bool init(std::string dir);
    static MMessage execGet(std::vector<std::string>& params, std::string& method, std::string body="");
    static MMessage execPut(std::vector<std::string>& params, std::string& method, std::string body="");
    static MMessage execPost(std::vector<std::string>& params, std::string& method, std::string body="");
    static boost::mutex mutexclusion;
protected:
   static MParams par;
   static bool isDigit(char);
   static std::string mkExternalName(int, std::string, std::string);
   static std::string getBody(std::string);
   static bool checkAndMakePath(std::string&);
   static std::string makeGetAnswer(std::string&);
   static std::string loadTemplate(std::string);
   static std::string mkResource(std::string&);
   static std::string mkAnswerBody(std::string&, std::string, char);
   static std::string extractString(std::string& src, char first, char last);
   static std::string fileDownload(std::string);
   static std::string getFile(std::string);
   static std::string extractValue(std::string&, std::string);
   static bool sendFile(std::string, std::string);
   static std::string  sendCmd(std::string);
   static std::string  cvtimej2x(std::string);
   static std::string  cvtimex2j(std::string);
};

#endif	/* AUTOMATISMMANAGER_H */
