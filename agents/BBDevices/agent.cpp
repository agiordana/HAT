/*
 *  agent.cpp
 *  iopwragent
 *
 *  Created by Davide Monfrecola on 07/11/12.
 *  Copyright 2012 Università del Piemonte Orientale. All rights reserved.
 *
 */

using namespace std;

#include "agent.h"
#include "Subject.h"
#include "agentnet.h"

Agent::Agent() {
    NameList pp;
    if(!install()) exit(-1);
    sethomedir();
    
    if(mklock()<0) {
        cout << "a copy of this iopwragent is already running! abort!" << endl; 
        exit(-1);
    };
	
    setStatus();
    trigger = new Trigger("trigger");
    timer = new Timer("Timer");
    timer->init();
    
    gpio = new GPIOThread("GPIOThread");
    gpio->init();
    
    devices = new VrtDevices("VrtDevices");
    devices->init();
    

    string info = "Agent: "+ hsrv::agentname + " Started";
    hsrv::logger->info(info, __FILE__, __FUNCTION__, __LINE__);

    // TEMP -> effettua lo start di tutti i gateway caricati
    AgentNet::init(hsrv::agentnet);
    trigger->start();
    timer->start();
    gpio->start();
    devices->start();
    if(_DEBUG_) SubjectSet::printSet();
    AgentNet::SubjectRegister();
    if (!start_rest_server()) {
      exit(-1);
    }

}



bool Agent::install() {
    string obs = hsrv::homedir+"/observers";
    string gpio = hsrv::homedir+"/gpio";
    string gpioactive = hsrv::homedir+"/gpioactive";
    string gpiofree = hsrv::homedir+"/gpiofree";
    string devices = hsrv::homedir+"/devices";
    string electricmeter = hsrv::homedir+"/devices/ElectricMeter";
    string fsmdev = hsrv::homedir+"/devices/FSMDevice";
    string binaryin = hsrv::homedir+"/devices/BinaryInput";
    string binaryout = hsrv::homedir+"/devices/BinaryOutput";
    string pwm = hsrv::homedir+"/devices/Pwm";
    string rgbpwm = hsrv::homedir+"/devices/RGBPwm";
    
    if(!FileManager::makeDir(hsrv::homedir, true)) {
	exit(-1);
    }
    if(!FileManager::makeDir(obs, true)) {
	exit(-1);
    }
    if(!FileManager::makeDir(gpio, true)) {
	exit(-1);
    }
    if(!FileManager::makeDir(gpioactive, true)) {
	exit(-1);
    }
    if(!FileManager::makeDir(gpiofree, true)) {
	exit(-1);
    }
    if(!FileManager::makeDir(devices, true)) {
	exit(-1);
    }
    if(!FileManager::makeDir(electricmeter, true)) {
	exit(-1);
    }
    if(!FileManager::makeDir(fsmdev, true)) {
	exit(-1);
    }
    if(!FileManager::makeDir(binaryin, true)) {
	exit(-1);
    }
    if(!FileManager::makeDir(binaryout, true)) {
	exit(-1);
    }
    if(!FileManager::makeDir(pwm, true)) {
	exit(-1);
    }
    if(!FileManager::makeDir(rgbpwm, true)) {
	exit(-1);
    }
    hsrv::archive = new ArchiveManager("archive");
    if (!hsrv::archive->init()) {
        exit(-1);
    }

    hsrv::router = new RoutingMap(hsrv::configdir+"/route.xml");  

    return true;
}
