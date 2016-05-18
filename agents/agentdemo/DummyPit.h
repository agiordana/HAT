//
//  DummySource.h
//  soapagent
//
//  Created by Attilio Giordana on 2/4/13.
//  Copyright (c) 2013 Università del Piemonte Orientale. All rights reserved.
//

#ifndef _DUMMY_PIT
#define _DUMMY_PIT

#include "agentlib.h"
#include "ObserverThread.h"
#include <boost/thread.hpp>


class DummyPit: public ObserverThread {
public:
    DummyPit(std::string n);
    ~DummyPit();
    void start_service_threads();
//    virtual bool getObservations(MParams& obs);
//    virtual bool getSubjects(MParams& subj);
protected:
    void do_work(DummyPit* obj);
};

#endif
