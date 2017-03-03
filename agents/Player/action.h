//
//  action.h
//  mngagent
//
//  Created by Attilio Giordana on 3/24/13.
//  Copyright (c) 2013 Università del Piemonte Orientale. All rights reserved.
//

#ifndef mngagent_action_h
#define mngagent_action_h
#include "agentlib.h"

class ThreadEntry {
public:
  double deadline;
  boost::thread* th;
  ThreadEntry(double a, boost::thread* b){deadline = a; th = b; };
  ~ThreadEntry() {};
};
  

class Action: public MParams {
public:
    std::string name;
    Action(std::string, std::string);
    bool setStatus(std::string);
    bool exec(std::string);
    bool observations(MParams& obs);
    bool subjects(MParams& subj);
    int member(std::string);
    std::string delay;
    bool purge_expired(double);
private:
    void do_work(Action* obj);
    std::vector<ThreadEntry> ths;
};


#endif
