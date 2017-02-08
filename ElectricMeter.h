/* 
 * File:   ElectricMeter.h
 * Author: attilio
 *
 * Created on May 12, 2015, 2:58 PM
 */

#ifndef ELECTRICMETER_H
#define	ELECTRICMETER_H
#include "agentlib.h"
#include "Device.h"

class ElectricMeter: public MParams, public Device {
public:
       bool execcmd(MMessage&);
       bool update(std::string, std::string);
       bool publish();
       bool doMeasure();
       bool reset(std::string);
       std::string getValue(std::string);
       setof<MAttribute> input();
       setof<MAttribute> output();
       bool getSubjects(MParams&);
       ElectricMeter(std::string, std::string);
private:
        double energy;
        double Ipower;
        double Apower;
        double Wpower;
        double origin;
        double currentTime;
        double alpha;
        double quantum;
        std::string unit;
        std::string in;
	double deltaEnergy;
};


#endif	/* ELECTRICMETER_H */

