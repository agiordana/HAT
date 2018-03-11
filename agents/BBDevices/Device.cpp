#include "Device.h"

using namespace std;

bool Device::update(string field, string value) {
    return false;
}

bool Device::execcmd(MMessage& m) {
    return false;
}

bool Device::setstatus(MMessage& m) {
    return false;
}

bool Device::publish() {
    return false;
}

bool Device::doMeasure() {
    return false;
}

bool Device::print() {
    return false;
}

bool Device::reset(string field) {
    return false;
}

string Device::getValue(string field) {
    return "undefined";
}

bool Device::getObservations(MParams& obs) {
    return true;
}

bool Device::getSubjects(MParams& obs) {
    return true;
}

setof<MAttribute> Device::input() {
    setof<MAttribute> tmp;
    return tmp;
}

setof<MAttribute> Device::output() {
    setof<MAttribute> tmp;
    return tmp;
}

Device::Device() {
    status = "OFF";
}

Device::~Device() {
    
}


string Device::userType() {
    if(name=="") return "UN";
    string tmp = "";
    for(size_t i=0; i<name.length()&& name[i] != '_'; i++) tmp += name[i];
    if(tmp.length() == 2) return tmp;
    else return "UN";
}

