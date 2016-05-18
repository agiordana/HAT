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

#define  CONFIGURATIONS "/home/horus/agents/CONFIGURATIONS"


class Configurator: public RouterExec {
public:
    Configurator(std::string n="configurator");
    MMessage exec(std::string uri, std::vector<std::string> params, std::string method, std::string body="");
private:
    bool installer(MMessage&);
    bool doReset();
    bool isHtml(std::string&);
    bool addBody(MMessage&, std::string);
};


#endif
