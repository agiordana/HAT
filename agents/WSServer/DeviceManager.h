/* 
 * File:   DeviceManager.h
 * Author: attilio
 *
 * Created on July 6, 2015, 12:37 PM
 */

#ifndef DEVICEMANAGER_H
#define	DEVICEMANAGER_H
#include "agentlib.h"
#include "NameTable.h"
#include "TransTab.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

class DeviceManager {
public:
    static bool init(std::string file);
    /* effettua la chiamata convertendo l'uri e il body in un comando che viene
	inviato ad un agente e ritorna l'esito del comando 
	dell'avvenuta esecuzione del comando */
    static MMessage execGet(std::vector<std::string>& params, std::string& method, std::string body="");
    static MMessage execGetSet(std::vector<std::string>& params, std::string& method, std::string body="");
    static MMessage execPut(std::vector<std::string>& params, std::string& method, std::string body="");
    static MMessage execPost(std::vector<std::string>& params, std::string& method, std::string body="");
    static MMessage execPostSet(std::vector<std::string>& params, std::string& method, std::string body="");
    static boost::mutex mutexclusion;
protected:
    static std::string extractValue(std::string&, std::string);
    static std::string remapValue(std::string);
    static TransTab* trans_tab;
    static MParams par;
    static MMessage mkAnswer(std::string&, TransEntry&, std::string);
};

#endif	/* RPCTABLE_H */
