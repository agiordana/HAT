/* 
 * File:   Trigger.h
 * Author: attilio
 *
 * Created on November 21, 2012, 11:19 PM
 */

#ifndef TRIGGER_H
#define	TRIGGER_H

//#include "agentlib.h"

#define TPRECISION 10

class Trigger: public MParams {
public:
    Trigger(std::string file);
    
    void exec(Trigger* obj);
    
    bool start(){ service_thread.push_back(new Thread(&Trigger::exec, this)); return true;};

private:
    
    long Euclide(long, long);
    
    bool initSubject();
    
    std::vector<Thread*> service_thread;
    
    double cycle;
    
    std::map<std::string, double> tm;
    
    struct timespec rqtp, rmtp;
};

#endif	/* TRIGGER_H */

