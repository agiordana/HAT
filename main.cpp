using namespace std;

#include "agent.h"
#include <boost/thread.hpp>
#include <sys/wait.h>

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
    	int status = 0;
	stringstream info;
	pid_t process;
	if(argc < 2) {
          cout << "Usage: soapagent <confdir>" << endl;
          return 0;
    	}

	if(!hsrv::load(argv[0],argv[1])) {
		cout << "directory: " << argv[1] << " not found" << endl;
		return 0;
	}
	if(setreuid(geteuid(),geteuid())<0) cout<<"Failed to become root!!!"<<endl;
	if(setregid(getegid(),getegid())<0) cout<<"Failed to become staff!!!"<<endl;
	while(1) {
	     if((process = fork()) == 0) {
    
	        boost::thread logThread(log_thread); 
    
		agent = new Agent();
    
      	     }
      	     else waitpid(-1,&status,0);
	     if(status != 256 ) sleep(20);
	     string lockfile = "rm " + hsrv::homedir+"/.lock";
             hsrv::cmdExec(lockfile);

	     info <<"status: "<< status<<",  restarting!!!!"<<endl;
	     hsrv::logger->info(info.str());
     	}
	return 0;
}
