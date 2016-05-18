//
//  pwrprogram.h
//  pwragent
//
//  Created by Attilio Giordana on 2/23/12.
//  Copyright (c) 2012 Università del Piemonte Orientale. All rights reserved.
//

#ifndef pwragent_pwrprogram_h
#define pwragent_pwrprogram_h


class FeatureValue {
public:
    double value;
    std::string beginTime;
    std::string endTime;
    FeatureValue(double, std::string, std::string);
    FeatureValue(std::string, std::string, std::string);
    FeatureValue(){value=0;beginTime="";endTime="";};
};
 

class PwrProgram: public std::map<std::string, FeatureValue> {
public:
    bool install();
    bool load(std::string);
    bool load();
    FeatureValue getCurrentTarget(std::string);
    FeatureValue getNextTarget(std::string);
    bool show();
    bool update();
    bool getProgramsList(std::map<std::string, std::string>&);
    bool getProgramsVersions(std::map<std::string, std::string>&);
    std::vector<std::string> active() {return areas;};
    std::string currentProgram() {return current; };
    bool xmlprint(std::ostringstream& out){if(doc!=NULL) out<<*doc; return (doc!=NULL);};
    PwrProgram();
protected:
    std::vector<std::string> areas;
    TiXmlDocument* doc;
    std::string current;
    bool getDefault();
    bool getTargets();
    bool getPeriods(TiXmlNode*, std::string, std::string);
    bool isInside(std::string, std::string, std::string);
    std::string readattr(std::string f, std::string name);
};

#endif
