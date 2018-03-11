/* 
 * File:   ObserverThread.cpp
 * Author: trampfox
 * 
 * Created on November 9, 2012, 9:27 AM
 */

using namespace std;

#include "ObserverThread.h"

ObserverThread::ObserverThread() {
    this->input_queue = new MMessageQueue<MMessage>();
}

bool ObserverThread::init() {
    MParams obs("OBS");
    MParams subj("SUBJ");
    this->getObservations(obs);
    this->getSubjects(subj);
    this->observes = obs.extractCategory("observe"); // eventi osservati
    this->subjects = subj.extractCategory("subject"); // eventi generati
    NetSubjects::initSubjects(hsrv::agentnet, obs, "import");
    NetSubjects::initSubjects(hsrv::agentnet, subj, "export");
    for(int j = 0;j < this->subjects.size();j++) {
        // aggiunge subject: name = sottipo, value = tipo
        SubjectSet::add_subject(this->subjects[j].value, this->subjects[j].name);
    }
}

MMessage ObserverThread::receive_message() {
    return this->input_queue->receive();
}

bool ObserverThread::notify(MMessage& m) {
    if(match_target(m)) this->input_queue->send(m);
    return true;
}

bool ObserverThread::getObservations(MParams& obs) {
    string pathname="threads/"+this->id;
    MParams param(pathname);
    param.load();
    for (size_t i=0; i<param.size(); i++) {
        if (param[i].category=="observe") {
            obs.add(param[i].name, param[i].category, param[i].value);
        }
    }
    return true;
}

bool ObserverThread::getSubjects(MParams& subj) {
    string pathname="threads/"+this->id;
    MParams param(pathname);
    param.load();
    for (size_t i=0; i<param.size(); i++) {
        if (param[i].category=="subject") {
            subj.add(param[i].name, param[i].category, param[i].value);
        }
    }
}

void ObserverThread::do_work(ObserverThread* obj) {
    cout<<"ObserverThread::do_work"<<endl;
}

void ObserverThread::do_work1(ObserverThread* obj) {
cout<<"ObserverThread::do_work1"<<endl;
}

bool ObserverThread::start() {
    // carica eventi da osservare (category observe)
    setof<MAttribute>::iterator it;
    for(it = this->observes.begin();it != this->observes.end();it++) {
        SubjectSet::add_observer(it->value, it->name, dynamic_cast<Observer*>(this));
    }
   
    this->start_service_threads();
    return true;
}

void ObserverThread::start_service_threads() {
    this->service_thread.push_back(new Thread(&ObserverThread::do_work, this));
}

void ObserverThread::stop_service_thread() {
}
