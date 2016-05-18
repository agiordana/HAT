/* 
 * File:   DigitalInput.h
 * Author: attilio
 *
 * Created on May 12, 2015, 2:58 PM
 */

#ifndef _ANALOGINPUT_H
#define	_ANALOGINPUT_H
#include "agentlib.h"
#include "Device.h"
#define SEPARATOR '$'

class AnalogInput: public MParams, public Device {
public:
       bool update(std::string, std::string);
       bool execcmd(MMessage&);
       std::string getValue();
       setof<MAttribute> input();
       setof<MAttribute> output();
       bool getSubjects(MParams&);
       AnalogInput(std::string, std::string);
       bool publish();
private:
	double scale_factor;
        double currentTime;
	std::string currentValue;
	std::string in;
        std::string remap(std::string&);
};


#endif	/* BINARYINPUT_H */

