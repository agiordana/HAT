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


class Receiver: public ObserverThread {
public:
    Receiver(std::string n);
    ~Receiver();
    void start_service_threads();
private:
    void do_work(Receiver* obj);
};

#endif
