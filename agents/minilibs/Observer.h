/* 
 * File:   Observer.h
 * Author: Davide Monfrecola<davide.monfrecola@gmail.com>
 * Copyright: 2012 Universit√† del Piemonte Orientale. All rights reserved.
 * 
 * Created on November 8, 2012, 3:29 PM
 */

#ifndef OBSERVER_H
#define	OBSERVER_H

//#include "agentlib.h"
#include "namelist.h"

class Subject;
class SubjectSet;

class Observer {
public:
    std::string id;
    Observer();
    Observer(const Observer& orig);
    virtual ~Observer(){};
    virtual bool notify(MMessage&) = 0;
    bool match_target(MMessage&);
protected:
    NameList target;
    MMessageQueue<MMessage>* input_queue;
};

#endif	/* OBSERVER_H */

