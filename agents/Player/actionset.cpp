//
//  actionset.cpp
//  mngagent
//
//  Created by Attilio Giordana on 3/24/13.
//  Copyright (c) 2013 Università del Piemonte Orientale. All rights reserved.
//

#include "actionset.h"

using namespace std;

bool MMessageTable::reset() {
  boost::unique_lock<boost::mutex> lock(mut);
  mlist.clear();
  free_threads = 0;
  return true;
}

bool MMessageTable::schedule(MMessage& m) {
  boost::unique_lock<boost::mutex> lock(mut);
  double deadline = m.getDouble("delay")/10 + hsrv::gettime();
  m.add("deadline", deadline);
  mlist.push_back(m);
  cond.notify_one();
  return true;
}

MMessage MMessageTable::get() {
  boost::unique_lock<boost::mutex> lock(mut);
  MMessage m;
  size_t r;
  free_threads++;
  if(mlist.size() == 0) cond.wait(lock);
  free_threads--;
  double first_deadline = mlist[0].getDouble("deadline");
  r = 0;
  for(size_i i=0; i<mlist.size(); i++) {
     double cdeadline = mlist[i].getDouble("deadline");
     if(cdeadline<first_deadline) {
	r = i;
	first_deadline = cdeadline;
     }
  }
  m = mlist[r];
  mlist.erase(mlist.begin()+r);
  return m;
}

int MMessageTable::waiting() {
  boost::unique_lock<boost::mutex> lock(mut);
  return free_threads;
}

ActionSet::ActionSet(string dir) {
    NameList filelist;
    NameList trfilelist;
    string config_dir = hsrv::configdir+"/"+dir;
	
    FileManager::fileList(config_dir, filelist);
    for(unsigned i = 0; i < filelist.size(); i++) {
        // recupera il nome della regola dal nome del file XML (eliminando l'estensione)
        push_back(Action(dir, FileManager::getStem(filelist[i])));
	(*this).back().load();
    }
    
}

bool ActionSet::setStatus(string name, string value) {
    for(size_t i=0; i<this->size(); i++)
        if((*this)[i].get("name")==name) {
            return (*this)[i].setStatus(value);
        }
    return false;
}

bool ActionSet::isActive(string n) {
    for(size_t i=0; i<this->size(); i++)
        if ((*this)[i].get("name")==n) {
            if((*this)[i].get("status")=="ON") return true;
        }
    return false;
}

bool ActionSet::exec(string name, string d) {
    MMessage m;
    m.add("name", name);
    m.add("delay", d);
    todo.schedule(m);
    if(todo.waiting()==0) ths.push_back(new Thread(&ActionSet::exec_thread, this));
    return true;
}

void exec_thread(&ActionSet obj) {
  struct timespec rqtp, rmtp;
  while(true) {
    MMessage m = todo.get();
    string name = m.getString("name");
    double deadline = m.getDouble("deadline");
    double waittime = deadline - hsrv::gettime();
    for(size_t i=0; i<obj->size(); i++) 
        if((*obj)[i].get("name")==name) {
	   if(waittime >0) {
		rqtp.tv_sec = (int) waittime;
		rqtp.tv_nsec = (int)(NANOPERSECOND*(waittime - rqtp.tv_sec));
		int res = nanosleep(&rqtp, &rmtp);
	   }
	   (*obj)[i].exec();
	}
  }
}

bool ActionSet::subjects(MParams &subj) {
    for(size_t i=0; i<this->size(); i++)
        (*this)[i].subjects(subj);
    return true;
}

bool ActionSet::observations(MParams &obs) {
    for(size_t i=0; i<this->size(); i++)
        (*this)[i].observations(obs);
    return true;
}
