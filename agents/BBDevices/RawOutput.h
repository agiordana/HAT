/* 
 * File:   ElectricMeter.h
 * Author: attilio
 *
 * Created on May 12, 2015, 2:58 PM
 */

#ifndef RAWOUTPUT_H
#define	RAWOUTPUT_H
#include "agentlib.h"
#include "Device.h"

class RawOutput: public MParams, public Device {
public:
       bool execcmd(MMessage&);
       std::string getValue();
       setof<MAttribute> input();
       setof<MAttribute> output();
       bool getSubjects(MParams&);
       bool getObservations(MParams&);
       RawOutput(std::string, std::string);
private:
        double currentTime;
        std::string currentValue;
        std::string out;
        std::string rawtype;
        std::string remap(std::string&);
};


#endif	/* BINARYOUTPUT_H */

