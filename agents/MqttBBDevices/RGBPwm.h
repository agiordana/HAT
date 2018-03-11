/* 
 * File:   Pwm.h
 * Author: attilio
 *
 * Created on May 12, 2015, 2:58 PM
 */

#ifndef RGBPWM_H
#define	RGBPWM_H
#include "agentlib.h"
#include "Device.h"

class RGBPwm: public MParams, public Device {
public:
       std::string getValue();
       setof<MAttribute> input();
       setof<MAttribute> output();
       bool getObservations(MParams&);
       bool getSubjects(MParams&);
       RGBPwm(std::string, std::string);
       bool execcmd(MMessage&);
       bool publish();
private:
       double currentTime;
       std::string currentIntensity;
       std::string currentColor;
       int intensity;
       std::string outputStatus;
       int R;
       int G;
       int B;
       std::string remap(std::string&);
       std::string Rout;
       std::string Gout;
       std::string Bout;
       bool sendCmd(std::string value="100");
       bool eventNotify(std::string value="100");
       int ha2int(std::string);
       bool save();
};


#endif	/* RGBPWM_H */

