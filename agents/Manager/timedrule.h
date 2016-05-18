//
//  timedrule.h
//  mngagent
//
//  Created by Attilio Giordana on 3/28/12.
//  Copyright (c) 2012 Università del Piemonte Orientale. All rights reserved.
//

#ifndef mngagent_timedrule_h
#define mngagent_timedrule_h

#include "agentlib.h"
#include "BooleanConditionSet.h"

class TimedRule: public MParams {
public:
    std::string name;
    std::string alias;
    /**
     * Crea una nuova istanza che gestisce le regole temporizzate 
     * @param 
     * @param 
     */
    TimedRule(std::string, std::string);
    /**
     * 
     * @param 
     * @param 
     * @return 
     */
    bool timeUpdate(std::string, int, BooleanConditionSet*, setof<MMessage>&);
    bool subjects(MParams& subj);
    bool init() {
        enabling_condition.init(get("enabling_condition"), '&');
		return true;
	};
    bool setStatus(std::string);
    bool setValidityTime();
    bool setValidityTime(std::string,std::string);
    bool setActionList(std::string, std::string);
protected:
    int status;
    NameList enabling_condition;
    std::string behavior;
    long reftime;
    int c_time;
    int counter;
    int from;
    int to;
    long cycle;
    long nextcyclestart;
    int midnight;
    int active;
    bool exec;
    /**
     * 
     * @param t
     * @param from
     * @param to
     * @return 
     */
    bool isInRange(int t, int from, int to) { if(from < t && to<0) return true; else return (t >= from && t<= to); };
    /**
     * 
     * @param 
     * @param 
     * @return 
     */
    bool cycleUpdate();
    /**
     * 
     * @param 
     * @param 
     * @return 
     */
    bool makeAction(int, setof<MMessage>&);
    bool checkCondition(BooleanConditionSet*);
    bool checkProgramName(std::string);
};

#endif
