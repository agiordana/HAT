using namespace std;

#include "agent.h"
#include <boost/thread.hpp>

Agent* agent;
MMessageQueue<MMessage>* test;

void* log_thread() {
    Message log_message;
    ostringstream ss;
	
    while (1) {
        log_message = hsrv::logger->receive();   // mutua esclusione
        hsrv::logger->writeLog(log_message);     
    }
	
}

int main (int argc, char * const argv[]) {
    
	if(argc < 2) {
        cout << "Usage: soapagent <confdir>" << endl;
        return 0;
    }

	if(!hsrv::load(argv[0],argv[1])) {
		cout << "directory: " << argv[1] << " not found" << endl;
		return 0;
	}
    
    boost::thread logThread(log_thread); 
    
	agent = new Agent();
    
	return 0;
}
