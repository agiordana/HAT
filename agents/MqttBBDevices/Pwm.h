/* 
 * File:   Pwm.h
 * Author: attilio
 *
 * Created on May 12, 2015, 2:58 PM
 */

#ifndef PWM_H
#define	PWM_H
#include "agentlib.h"
#include "Device.h"

class Pwm: public MParams, public Device {
public:
       std::string getValue();
       setof<MAttribute> input();
       setof<MAttribute> output();
       bool getObservations(MParams&);
       bool getSubjects(MParams&);
       Pwm(std::string, std::string);
       bool execcmd(MMessage&);
       bool publish(std::string value="");
private:
       bool save();
       bool eventNotify(std::string);
       double currentTime;
       std::string currentValue;
       std::string remap(std::string&);
       std::string out;
};


#endif	/* PWM_H */

