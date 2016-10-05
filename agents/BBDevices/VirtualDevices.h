//
//  VirtualDevices.h
//
//  Created by Attilio Giordana on 7/5/2015.
//  Copyright (c) 2015 Penta Dynamic Solutions. All rights reserved.
//

#ifndef _VRTDEVICES_
#define _VRTDEVICES_

#include "agentlib.h"
#include "ObserverThread.h"
#include <boost/thread.hpp>
#include "Device.h"
#include "ElectricMeter.h"
#include "FSMDevice.h"
#include "BinaryInput.h"
#include "AnalogInput.h"
#include "BinaryOutput.h"
#include "RawOutput.h"
#include "RawInput.h"
#include "TriStateOutput.h"
#include "Pwm.h"
#include "RGBPwm.h"


class VrtDevices: public ObserverThread {
public:
    VrtDevices(std::string n);
    ~VrtDevices();
    void start_service_threads();
    bool getObservations(MParams& obs);
    bool getSubjects(MParams& subj);
    bool update(MMessage&);
    bool publish();
    bool doMeasure();
    bool doReset(MMessage&);
    bool doReset(std::string);
    bool execcmd(MMessage&);
    bool exectrigger(MMessage&);
    bool setstatus(MMessage&);
    bool execLocalEvent(MMessage&);
    bool execwait(MMessage&);
protected:
    std::string day;
    void do_work(VrtDevices* obj);
    std::map<std::string, Device*> dev_by_name;
    std::map<std::string, Device*> dev_by_io;
    bool loadDevices(std::string dir);
    bool loadRawOutput(std::string dir);
    bool loadRawInput(std::string dir);
    bool loadDevType(std::string dir1, std::string dir2);
    void doReboot(MMessage&);
    bool freegpioRegister();
    bool devConfigure(MMessage&);
    bool devDelete(MMessage&);
    bool devRemove(std::string, std::string, MMessage&);
    bool doArchiveUpdate();
    bool removeComment(std::string&);
    bool mkManifest(MMessage&);
    bool deleteManifest(std::string);
    bool mkRawOutput(std::string);
    bool mkRawInput(std::string);
    bool makeDevicePage(std::map<std::string, Device*>&);
};

#endif
