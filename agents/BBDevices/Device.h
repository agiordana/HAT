/* 
 * File:   Device.h
 * Author: attilio
 *
 * Created on May 12, 2015, 2:13 PM
 */

#ifndef DEVICE_H
#define	DEVICE_H
#include "agentlib.h"

class Device {
public:
    std::string name;
    std::string alias;
    virtual bool update(std::string, std::string);
    virtual bool execcmd(MMessage&);
    virtual bool setstatus(MMessage&);
    virtual bool publish();
    virtual bool doMeasure();
    virtual bool print();
    virtual bool reset(std::string);
    virtual std::string getValue(std::string);
    virtual setof<MAttribute> input();
    virtual setof<MAttribute> output();
    virtual bool getObservations(MParams&);
    virtual bool getSubjects(MParams&);
    std::string status;
    std::string userType();
    Device();
    virtual ~Device();
};

#endif	/* DEVICE_H */

