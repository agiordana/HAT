//
//  GPIOThread.h
//
//  Created by Attilio Giordana on 7/5/2015.
//  Copyright (c) 2015 Penta Dynamic Solutions. All rights reserved.
//

#ifndef _GPIOTHREAD_
#define _GPIOTHREAD_

#include "agentlib.h"
#include "ObserverThread.h"
#include <boost/thread.hpp>
#include "Gpio.h"
#include <poll.h>
#include "Device.h"
#include "ElectricMeter.h"


class GPIOThread: public ObserverThread {
public:
    GPIOThread(std::string n);
    ~GPIOThread();
    void start_service_threads();
    bool getObservations(MParams& obs);
    bool getSubjects(MParams& subj);
protected:
    MMessageQueue<MMessage>* probe_polling;
    vector<Gpio*> gpio;
    vector<Gpio*> input;
    vector<Gpio*> output;
    vector<Gpio*> pwmout;
    vector<Gpio*> onewire;
    bool MkExport();
    bool MkOnewire(std::string);
    virtual void do_work(GPIOThread* obj);
    void do_work1(GPIOThread* obj);
    void do_work2(GPIOThread* obj);
    std::string getAsciiValue(std::string& s, std::string kw);
    struct pollfd *pfd;
    int get_current_value(int);
//    bool set_current_value(std::string, std::string);
    bool gpioRegister();
    std::string isDefined(std::string);
    bool owDefine(std::string, std::string, std::string);
    bool owPurge(std::vector<std::string>&, std::map<std::string, bool>&);
    std::string mkName();
};

#endif
