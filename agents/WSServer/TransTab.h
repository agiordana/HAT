/* 
 * File:   TransTab.h
 * Author: attilio
 *
 * Created on July 22, 2015, 9:56 AM
 */

#ifndef TRANSTAB_H
#define	TRANSTAB_H

#include "agentlib.h"

class TransEntry: public MParams {
public:
    TransEntry(std::string, std::string);
    TransEntry(){category = "NULL";};
    bool match(std::vector<std::string>&, string&);
    bool isnoentry() {return (category=="NULL");};
    MMessage rpc2cmd(std::vector<std::string>&, std::string&, std::string&);
    std::string xml2jsonBody(std::string&);
    std::string xml2jsonStatus(std::string&);
    bool print();
protected:
    NameList prefix;
    std::string method;
    std::string category;
    size_t nameindex;
    size_t agentindex;
    size_t valueindex;
    std::string getName(std::vector<std::string>& params);
    std::string getEvent(std::vector<std::string>& params);
    std::string getAgent(std::vector<std::string>& params);
    std::string getValue(std::vector<std::string>& params);
    std::string nv2av(std::vector<std::string>& params, std::string);
    std::string extractValue(std::string&, std::string);
    std::string convertValue(std::string&);
    std::string replaceSpaces(std::string&);
};

class TransTab: public std::vector<TransEntry> {
public:
    /* Tanslation Table: implements the method for converting rpc calls to commands */
    TransTab(std::string dir);
    TransEntry get(std::vector<std::string>&, std::string&);
};


#endif	/* TRANSTAB_H */

