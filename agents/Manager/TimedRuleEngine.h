#ifndef _TIMEDRULE_ENGINE
#define _TIMEDRULE_ENGINE

#include "agentlib.h"
#include "ObserverThread.h"
#include <boost/thread.hpp>
#include "timedruleset.h"


class TimedRuleEngine: public ObserverThread {
public:
    TimedRuleEngine(std::string n);
    ~TimedRuleEngine();
    void start_service_threads();
private:
    TimedRuleSet* timed_rule;
    BooleanConditionSet* condition;
    MMessage program;
    bool alarm_on;
    std::vector<std::string> active;
    void do_work(TimedRuleEngine* obj);
    bool makeTimedRulePage(TimedRuleSet& tr);
    bool makeConditionPage(BooleanConditionSet& cnd);
    bool getObservations(MParams& obs);
    bool getSubjects(MParams& subj);
};

#endif
