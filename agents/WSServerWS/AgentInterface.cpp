using namespace std;

#include "AgentInterface.h"

AgentInterface::AgentInterface (string n) {
    id = n;
}

AgentInterface::~AgentInterface() {

}

void AgentInterface::start_service_threads() {
    this->service_thread.push_back(new Thread(&AgentInterface::do_work, this));
}

void AgentInterface::do_work(AgentInterface* obj) {
    MMessage msg;
    NameTable::loadCategories();
    NameTable::reset();
    while(true) {
        msg = obj->receive_message();
        if(msg.mtype == "net" && msg.msubtype == "agent_info") NameTable::update(msg);
    }
}
