/* 
 * File:   Gpio.h
 * Author: attilio
 *
 * Created on May 7, 2015, 10:13 PM
 */

#ifndef GPIO_H
#define	GPIO_H
#include "agentlib.h"

class Gpio: public MParams {
public:
    std::string name;
    std::string gpioname;
    std::string gpiotype;
    Gpio(std::string, std::string);
//    bool MkExport();
    std::string pathname;
    std::string gpiovalue;
    double wait_until;
    double w;
    std::string currentValue;
    bool setCurrentValue(std::string);
    bool initialise();
    bool program();
    bool setInitialValue();
private:
    std::string getpath(std::string);
    std::string get_and_set_value(std::string);
    static NameList initDone;
    std::string gpiopath;
    std::string dutyCycle;
};


#endif	/* GPIO_H */
