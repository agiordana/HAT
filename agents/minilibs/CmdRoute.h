/* 
 * File:   CmdRoute.h
 * Author: andrea
 *
 * Created on 10 aprile 2015, 15.29
 */

#ifndef CMDROUTE_H
#define	CMDROUTE_H

#include "Route.h"

class CmdRoute : public RouterExec {
public:        
    CmdRoute(std::string n="cmdroute") : RouterExec(n) {};
    MMessage exec(std::string uri, std::vector<std::string> params, std::string method, std::string body="");
protected:
    bool addBody(MMessage&, std::string);
    bool isHtml(std::string&);
};

#endif	/* CMDROUTE_H */

