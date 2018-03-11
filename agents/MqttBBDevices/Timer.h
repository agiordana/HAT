//
//  Timer.h
//  soapagent
//
//  Created by Attilio Giordana on 3/12/13.
//  Copyright (c) 2013 Università del Piemonte Orientale. All rights reserved.
//

#ifndef soapagent_Timer_h
#define soapagent_Timer_h

#include "agentlib.h"
#include "ObserverThread.h"
#include "FSMDevice.h"

class TimedCondition {
public:
    std::string type;
    std::string subtype;
    std::string value;
    double wait_time;
    TimedCondition() {type=subtype=value=""; wait_time = 0; expired = false;};
    TimedCondition(std::string, std::string, std::string);
    bool message(MMessage&);
    bool update();
    bool update(double d);
    bool isEqual(TimedCondition&);
    bool expired;
    bool print() {std::cout<<"--"<<type<<" "<<subtype<<" "<<value<<" "<<wait_time<<" "<<expired<<std::endl; return true;};
private:
    std::string getDuration(std::string&);
};

class Timer: public ObserverThread {
public:
    std::vector<TimedCondition> condition;
    Timer(std::string n);
    ~Timer();
    void start_service_threads();
private:
    bool update();
    bool update(double);
    void do_work(Timer* obj);
    bool startTimer(TimedCondition);
    bool resetTimer(TimedCondition);
    double getDoubleDuration(std::string);
};


#endif
