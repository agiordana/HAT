/* 
 * File:   ArchiveRoute.h
 * Author: andrea
 *
 * Created on 10 aprile 2015, 15.29
 */

#ifndef ARCHIVEROUTE_H
#define	ARCHIVEROUTE_H

#include "Route.h"

class ArchiveRoute : public RouterExec {
public:        
    ArchiveRoute(std::string n="archive") : RouterExec(n) {};
    MMessage exec(std::string uri, std::vector<std::string> params, std::string method, std::string body="");
};

#endif	/* ARCHIVEROUTE_H */

