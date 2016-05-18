/* 
 * File:   Subject.cpp
 * Author: trampfox
 * 
 * Created on November 8, 2012, 3:31 PM
 */

#include "Subject.h"

using namespace std;

Subject::Subject(std::string type, std::string subtype) {
    this->type = type;
    this->subtype = subtype;
}

Subject::Subject(const Subject& orig) {
}

Subject::~Subject() {
}

void Subject::register_observer(Observer* observer) {
    if(isNotRegistered(observer)) this->observers.push_back(observer);
}

bool Subject::isNotRegistered(Observer* observer) {
    vector<Observer*>::iterator it;
    for (it = observers.begin(); it!=observers.end(); it++) {
        if (*it==observer) {
            return false;
        }
    }
    return true;
}

void Subject::remove_observer(Observer* observer) {
    vector<Observer*>::iterator it, pos=observers.end();
    for (it = observers.begin(); it!=observers.end(); it++) {
        if (*it==observer) {
            pos = it;
            *it = NULL;
        }
    }
    if(pos!=observers.end()) observers.erase(pos);
}

void Subject::remove_observer(string id) {
    vector<Observer*>::iterator it, pos=observers.end();
    for (it = observers.begin(); it!=observers.end(); it++) {
        if ((*it)->id==id) {
            pos = it;
            *it = NULL;
        }
    }
    if(pos!=observers.end()) observers.erase(pos);
}


void Subject::notify_observers(MMessage& msg) {
    vector<Observer*>::iterator it;
    
//    cout << "[Subject] Message received: " << msg.xmlEncode(1) << endl;
    
    for(it = this->observers.begin();it != this->observers.end();it++) {
//	cout << "Notify on object: " << (*it) << endl;
        (*it)->notify(msg);
    }
}
    
