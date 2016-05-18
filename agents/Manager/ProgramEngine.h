#ifndef _PROGRAM_ENGINE
#define _PROGRAM_ENGINE

#include "agentlib.h"
#include "ObserverThread.h"
#include <boost/thread.hpp>


class ProgramEngine: public ObserverThread {
public:
    ProgramEngine(std::string n);
    ~ProgramEngine();
    void start_service_threads();
private:
    Program* program;
    std::string previous_program;
    std::vector<std::string> active;
    void do_work(ProgramEngine* obj);
    bool setprogram(MMessage& m, Program* p);
    bool program_notify(Program* p, std::vector<std::string>& al, bool change=false);
    bool makeStatusPage(Program& p, std::vector<std::string>& al);
    bool makeVersionPage(Program& p, std::vector<std::string>& al);
    bool program_info(MMessage& msg);
};

#endif
