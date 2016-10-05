/* 
 * File:   ElectricMeter.h
 * Author: attilio
 *
 * Created on May 12, 2015, 2:58 PM
 */

#ifndef BINARYOUTPUT_H
#define	BINARYOUTPUT_H
#include "agentlib.h"
#include "Device.h"

class BinaryOutput: public MParams, public Device {
public:
       bool execcmd(MMessage&);
       std::string getValue();
       setof<MAttribute> input();
       setof<MAttribute> output();
       bool is_deadline_waiting();
       bool getSubjects(MParams&);
       bool getObservations(MParams&);
       BinaryOutput(std::string, std::string);
       bool publish();
private:
        double currentTime;
        double dDuration;;
        double deadline;
        std::string currentValue;
        std::string duration;
	std::string default_value;
	std::string wait_condition;
	std::string remap(std::string&);
        std::string out;
	bool save();
};


#endif	/* BINARYOUTPUT_H */

