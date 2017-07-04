/* 
 * File:   AlarmManager.h
 * Author: attilio
 *
 * Created on July 6, 2015, 12:37 PM
 */

#ifndef ALARMMANAGER_H
#define	ALARMMANAGER_H
#include "agentlib.h"
#include "NameTable.h"
#include "TransTab.h"
#include "JParser.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>


class AlarmManager {
public:
    static bool init(std::string dir);
    /* effettua la chiamata convertendo l'uri e il body in un comando che viene
	inviato ad un agente e si pone in attesa della segnalazione 
	dell'avvenuta esecuzione del comando */
    static MMessage execGet(std::vector<std::string>& params, std::string& method, std::string body="");
    static MMessage execPut(std::vector<std::string>& params, std::string& method, std::string body="");
    static MMessage execPost(std::vector<std::string>& params, std::string& method, std::string body="");
    /* sveglia la thread di una rpc in attesa della comunicazione del 
	cambiamento di stato dovuto al comando ad essa associato */
    static boost::mutex mutexclusion;
protected:
   static MMessage mkAnswer(std::string&, TransEntry&, std::string);
   static TransTab* trans_tab;
   static bool mksensors_called;
   static bool init_called;
   static MParams par;
   static bool mkSensorsAndActions(bool mode=false);
   static bool isDigit(char);
   static std::string mkExternalName(int, std::string, std::string);
   static std::string getBody(std::string);
   static std::string getProgramStatus(std::string);
   static std::string getAlarmFile(std::string);
   static bool checkAndMakePath(std::string&);
   static std::string makeGetAnswer(std::string&);
   static bool checkAndInitPrograms(bool mode=false);
   static bool initAuto();
   static bool initZones();
   static bool isZoneDefined();
   static bool zoneUpdate(std::string);
   static std::string loadTemplate(std::string);
   static std::string mkResource(std::string&);
   static std::string extractString(std::string& src, char first, char last);
   static bool initExecutionQueues();
   static std::string setExecQueue(std::string&, std::string&, std::string&);
   static std::string schedulej2x(std::string&);
   static std::string zonesj2x(std::string&);
   static std::string fileDownload(std::string);
   static bool sendFile(std::string, std::string);
   static std::string  sendCmd(std::string);
   static bool cleanZones(std::string);
};

#endif	/* ALARMMANAGER_H */
