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


class MdFilter: public ObserverThread {
public:
    MdFilter(std::string n);
    ~MdFilter();
    void start_service_threads();

    static std::string path2Url(std::string path);

private:
    void do_work(MdFilter* obj);
    std::string filterCMD;
};

#endif
