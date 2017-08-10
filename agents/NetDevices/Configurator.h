//
//  Configurator.h
//  mngagent
//
//  Created by Attilio Giordana on 1/10/15.
//  Copyright (c) 2013 Università del Piemonte Orientale. All rights reserved.
//

#ifndef mngagent_Configurator_h
#define mngagent_Configurator_h

#include "agentlib.h"
#include <boost/thread.hpp>
#include "ObserverThread.h"
#include "NameService.h"
#include "Route.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/condition.hpp>

class Configurator: public RouterExec {
public:
    Configurator(std::string n="configurator");
    MMessage exec(std::string uri, std::vector<std::string> params, std::string method, std::string body="");
private:
    NameList devname;
    NameList inputname;
    NameList componentname;
    std::string sensor_category;
    std::string cmd_category;
    bool mkComponentLists();
    std::string mkItem(MMessage&, std::string);
    std::string getCategory(std::string&);
    bool addBody(MMessage&, std::string);
    bool confReset();
    bool isHtml(std::string&);
    bool mkManifest(MMessage&);
    std::string deleteItem(std::string&);
    std::string deviceList();
    std::string componentList();
    std::string inputList();
    std::string mkControl(std::string&, std::string&, unsigned n);
    bool generateGrayCode(int, std::vector<std::string>&);
    static boost::mutex mutex;
};


#endif
