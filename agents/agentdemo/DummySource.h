//
//  DummySource.h
//  soapagent
//
//  Created by Attilio Giordana on 2/4/13.
//  Copyright (c) 2013 Università del Piemonte Orientale. All rights reserved.
//

#ifndef _DUMMY_SOURCE
#define _DUMMY_SOURCE

#include "agentlib.h"
#include "ObserverThread.h"
#include <boost/thread.hpp>


class DummySource: public ObserverThread {
public:
    DummySource(std::string n);
    ~DummySource();
    virtual void start_service_threads();
//    virtual bool getObservations(MParams& obs);
//    virtual bool getSubjects(MParams& subj);
protected:
    virtual void do_work(DummySource* obj);
    unsigned i;
};

#endif
