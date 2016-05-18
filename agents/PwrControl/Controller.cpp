
#include "Controller.h"


using namespace std;


//----- class Controller ---------------------------------

Controller::Controller(string n) {
    name = n;
    this->scheduler = new Scheduler("scheduler");
    
    id = n;
}

Controller::~Controller() {
    
}


void Controller::start_service_threads() {
    this->service_thread.push_back(new Thread(&Controller::do_work, this));
}

void Controller::do_work(Controller* obj) {
    MMessage msg;
    MMessage mt;
    MMessage m;
    setof<MMessage> action_todo;
    bool res;
    double tval;
    obj->makeSchedulerPage(*obj->scheduler);

    for (unsigned i=0; i<action_todo.size(); i++) {
       SubjectSet::notify(action_todo[i]);
    }
    while(true) {
        action_todo.clear();
        msg = obj->receive_message();
        
        if(msg.mtype == "event") {
            action_todo.clear();
            res = scheduler->checkControl(msg, action_todo);
            for (unsigned i=0; i<action_todo.size(); i++) {
                SubjectSet::notify(action_todo[i]);
            }
            obj->makeSchedulerPage(*obj->scheduler);
        }
        else if (msg.mtype == "cmd" && msg.msubtype == "setcontrol") {
            scheduler->setControl(msg);
            obj->makeSchedulerPage(*obj->scheduler);
        }
        else if (msg.mtype == "cmd" && msg.msubtype == "resetcontrol") {
            scheduler->resetControl(msg);
            obj->makeSchedulerPage(*obj->scheduler);
        }
        else if (msg.mtype == "cmd" && msg.msubtype == "settarget") {
            scheduler->setTarget(msg);
            obj->makeSchedulerPage(*obj->scheduler);
        }
        else if (msg.mtype == "cmd" && msg.msubtype == "setschedulerstatus") {
            scheduler->set("status", msg.getString("value"));
            obj->makeSchedulerPage(*obj->scheduler);
        }
	else if (hsrv::checkConfigureEnabled() && msg.mtype == "cmd" && msg.msubtype == "mkcontroldev") {
	    scheduler->makeControlDevices();
	    scheduler->makeSensorDevices();
	    exit(1);
	}
        else if (msg.mtype == "trigger" && msg.msubtype == "checkalignement") {
	    action_todo.clear();
	    scheduler->aligneControl(action_todo);
	    for (unsigned i=0; i<action_todo.size(); i++) {
	       SubjectSet::notify(action_todo[i]);
	    }
	}
    }
}


bool Controller::makeSchedulerPage(Scheduler& s) {
    return s.makePage();
}


bool Controller::getObservations(MParams &obs) {
    ObserverThread::getObservations(obs);
    scheduler->observations(obs);
    return true;
}

bool Controller::getSubjects(MParams &subj) {
    ObserverThread::getSubjects(subj);
    scheduler->subjects(subj);
    return true;
}

