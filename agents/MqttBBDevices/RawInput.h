/* 
 * File:   ElectricMeter.h
 * Author: attilio
 *
 * Created on May 12, 2015, 2:58 PM
 */

#ifndef RAWINPUT_H
#define	RAWINPUT_H
#include "agentlib.h"
#include "Device.h"

class RawInput: public MParams, public Device {
public:
       bool execcmd(MMessage&);
       std::string getValue();
       setof<MAttribute> input();
       setof<MAttribute> output();
       bool getSubjects(MParams&);
       bool getObservations(MParams&);
       RawInput(std::string, std::string);
private:
        double currentTime;
        std::string currentValue;
        std::string out;
        std::string rawtype;
        std::string remap(std::string&);
};


#endif	/* RAWINPUT_H */

