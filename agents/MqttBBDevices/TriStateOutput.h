/* 
 * File:   ElectricMeter.h
 * Author: attilio
 *
 * Created on May 12, 2015, 2:58 PM
 */

#ifndef TRISTATEOUTPUT_H
#define	TRISTATEOUTPUT_H
#include "agentlib.h"
#include "Device.h"

class TriStateOutput: public MParams, public Device {
public:
       bool execcmd(MMessage&);
       std::string getValue();
       setof<MAttribute> input();
       setof<MAttribute> output();
       bool getSubjects(MParams&);
       bool getObservations(MParams&);
       TriStateOutput(std::string, std::string);
       bool publish();
private:
       double currentTime;
       std::string currentStatus;
       std::string duration;
       std::string default_status;
       std::string wait_condition;
       std::string remap(std::string&);
       std::string out1;
       std::string out2;
       std::string ing1;
       std::string ing2;
       std::string ing1v;
       std::string ing2v;
       bool docmdaction(std::string);
       bool dowaitaction();
       bool notifyEvent(std::string);
       bool checkUpcase(MMessage&);
       bool save();
};


#endif	/* BINARYOUTPUT_H */

