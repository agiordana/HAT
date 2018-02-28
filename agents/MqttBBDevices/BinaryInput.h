/* 
 * File:   DigitalInput.h
 * Author: attilio
 *
 * Created on May 12, 2015, 2:58 PM
 */

#ifndef _BINARYINPUT_H
#define	_BINARYINPUT_H
#include "agentlib.h"
#include "Device.h"
#define SEPARATOR '$'

class BinaryInput: public MParams, public Device {
public:
       bool update(std::string, std::string);
       bool execcmd(MMessage&);
       bool setstatus(MMessage&);
       std::string getValue();
       setof<MAttribute> input();
       setof<MAttribute> output();
       bool getSubjects(MParams&);
       bool getObservations(MParams&);
       BinaryInput(std::string, std::string);
       bool publish();
private:
        double currentTime;
	std::string currentValue;
	std::string in;
	std::string delay;
        std::string remap(std::string&);
        std::map<std::string, MMessage> delay_table;
};


#endif	/* BINARYINPUT_H */

