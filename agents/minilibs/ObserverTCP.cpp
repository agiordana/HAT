/* 
 * File:   ObserverTCP.cpp
 * Author: trampfox
 * 
 * Created on November 9, 2012, 9:27 AM
 */

#include "agentlib.h"

using namespace std;

ObserverTCP::ObserverTCP(std::string dir, std::string name): MRemoteAgent(dir, name) { 
    vector<string> tg;
    this->load();
    this->id = name;
    this->input_queue = new MMessageQueue<MMessage>();
    // utilizzo metodo MParams ereditato da RemoteAgent
    this->observes = extractCategory("observe");
    target.init(get("target"));
}

void ObserverTCP::dispatcher(ObserverTCP* obj) {

    while(1) {
        MMessage m = obj->receive_message();
	if(AgentConf::status == ON) {
		this->send(m);
	}
    }

}

MMessage ObserverTCP::receive_message() {
    return this->input_queue->receive();
}


bool ObserverTCP::notify(MMessage& m) {
    if(match_target(m)) this->input_queue->send(m);
    return true;
}


bool ObserverTCP::start() {
    this->service_threads.push_back(new Thread(&ObserverTCP::dispatcher, this));
    
    // si aggiunge come observer per gli eventi specificati nel file di 
    // configurazione
    setof<MAttribute>::iterator it;
    for(it = this->observes.begin();it != this->observes.end();it++) {
        // value = tipo, name = sottotipo
        if(_DEBUG_) {
            stringstream info;
            info << "Observer ~ type: " << it->value << " | subtype: " << it->name << " -- added" << endl;
            hsrv::logger->info(info.str(), __FILE__, __FUNCTION__, __LINE__);
        }
        SubjectSet::add_observer(it->value, it->name, this);
    }
    return true;
}

bool ObserverTCP::stop() {
    for(unsigned i=0; i<this->service_threads.size(); i++) {
        delete service_threads[i];
        service_threads[i]=NULL;
    }
    service_threads.clear();
    return true;
}
