//
//  Device.h
//  ioagent
//
//  Created by Attilio Giordana on 3/6/13.
//  Copyright (c) 2013 Università del Piemonte Orientale. All rights reserved.
//

#ifndef FSMDEVICE_h
#define FSMDEVICE_h

#include "agentlib.h"
#include "Device.h"

class FSMCondition {
public:
    std::string type;
    std::string subtype;
    std::string value;
    std::string key;
    FSMCondition(){type=subtype=value="";};
    FSMCondition(TiXmlNode*);
    FSMCondition(std::string);
    FSMCondition(std::string a, std::string b, std::string c) {type=a; subtype=b; value=c; key=a+"_"+b+"_"+c;};
    bool print() {std::cout<<"      FSMCondition: type="<<type<<" subtype="<<subtype<<" value="<<value<<std::endl; return true;};
    bool operator == (FSMCondition);
    bool operator != (FSMCondition);
    bool operator < (FSMCondition);
    bool operator <= (FSMCondition);
    bool operator > (FSMCondition);
    bool operator >= (FSMCondition);
    bool input(setof<MAttribute>&);
};

class Action {
public:
    std::string type;
    std::string subtype;
    std::string value;
    Action(){type=subtype=value="";};
    Action(TiXmlNode*);
    Action(std::string);
    Action(std::string a, std::string b, std::string c) {type=a; subtype=b; value=c;};
    bool print() {std::cout<<"      Action: type="<<type<<" subtype="<<subtype<<" value="<<value<<std::endl; return true;};
    bool subjects(MParams&);
    bool output(setof<MAttribute>&);
};

class Transition {
public:
    std::string name;
    std::string newstate;
    FSMCondition condition;
    std::vector<Action> action;
    Transition() {newstate = "";};
    Transition(std::string);
    Transition(TiXmlNode*);
    bool print();
    bool observations(MParams&);
    bool subjects(MParams&);
    bool activeFSMConditions(std::vector<FSMCondition>&);
    bool exec(FSMCondition&, std::vector<MMessage>&, size_t&);
    bool compileNumeric(std::string, size_t);
    bool input(setof<MAttribute>&);
    bool output(setof<MAttribute>&);
private:
    size_t nextstate;
};

class State: public std::vector<Transition> {
public:
    std::string name;
    State() {name = ""; start = false;};
    State(std::string);
    State(TiXmlNode*);
    bool start;
    bool print();
    bool observations(MParams&);
    bool subjects(MParams&);
    bool activeFSMConditions(std::vector<FSMCondition>&);
    bool exec(FSMCondition&, std::vector<MMessage>&, size_t&);
    bool compileNumeric(std::string, size_t);
    bool input(setof<MAttribute>&);
    bool output(setof<MAttribute>&);
private:
    size_t numeric_name;
};

class FSMDevice: public Device, public std::vector<State> {
public:
    ~FSMDevice();
    FSMDevice(std::string dir, std::string file);
    FSMDevice();
    bool load(std::string file);
    bool print();
    bool execcmd(MMessage&);
    bool update(std::string, std::string);
    bool getObservations(MParams&);
    bool getSubjects(MParams&);
    bool activeObservations(MParams&);
    bool activeFSMConditions(std::vector<FSMCondition>& c) { return (*this)[current].activeFSMConditions(c); };
    size_t currentState() { return current; };
    size_t oldState() {return oldstate; };
    size_t exec(FSMCondition& c, std::vector<MMessage>& m);
    std::string currentStateName() {return (*this)[current].name; };
    bool remove();
    setof<MAttribute> input();
    setof<MAttribute> output();
private:
    std::string currentValue;
    std::vector<FSMCondition> expected;
    size_t current;
    size_t oldstate;
    TiXmlDocument* doc;
    bool publish();    
};

#endif
