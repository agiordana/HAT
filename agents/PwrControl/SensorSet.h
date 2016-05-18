//
//  controlset.h
//  engagent
//
//  Created by Attilio Giordana on 7/14/14.
//  Copyright (c) 2014 Università del Piemonte Orientale. All rights reserved.
//

#ifndef engagent_sensorset_h
#define engagent_sensorset_h

#include "Sensor.h"

class SensorSet: public std::vector<Sensor> {
public:
    SensorSet(std::string dir);
    bool setSensor(MMessage&);
    bool resetSensor(MMessage&);
    std::string getControlMeasure();
    std::string getThreshold();
    bool observations(MParams&);
};

#endif
