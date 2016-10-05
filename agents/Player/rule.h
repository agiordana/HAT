/*
 *  rule.h
 *  mngagent
 *
 *  Created by Attilio Giordana on 1/13/12.
 *  Copyright 2012 Università del Piemonte Orientale. All rights reserved.
 *
 */

#ifndef _mngagent_rule_
#define _mngagent_rule_

#include "agentlib.h"
#include "eventcounterset.h"
#include "BooleanConditionSet.h"

class Rule: public MParams {
public:
    /**
	 * Costruttore della classe. Inizializza i parametri che caratterizzano l'istanza della classe.
     * Ogni istanza rappresenta una regola che viene caricata da un file XML
	 * @param dir directory che contiene il file XML con i parametri per l'istanza della classe
     * @param name nome della regola da caricare
     */
    Rule(std::string, std::string);
    /**
     
     */
    std::string name;
    bool match(EventCounterSet*, BooleanConditionSet*, setof<MMessage>&);
    bool init(); 
    bool subjects(MParams& subj);
    bool setStatus(std::string);
    bool setValidityTime();
    bool setValidityTime(std::string,std::string);
    bool isActive();
    bool timeUpdate() { return cycleUpdate(); };
    std::string alias;
private:
    int status;
    /**
     @var NameList
     @brief lista di eventi abilitanti
     */
    NameList condition;
    /**
     @var NameList
     @brief lista di condizioni abilitanti quando sono "true"
     */
    NameList enabling_true_condition;
    /**
     @var NameList
     @brief lista di condizioni abilitanti quando sono "false"
     */
    NameList enabling_false_condition;
    /**
     @var NameList
     @brief lista di azioni
     */
    NameList action;
    bool cycleUpdate();
    double latentUntil;
    int from;
    int to;
    int midnight;
    int mine_t_cond;
    int mine_f_cond;
    int mincond;
    long cycle;
    long reftime;
    long nextcyclestart;
    int c_time;
    bool cycleactive;
    std::string behavior;
    bool prcondition;
    bool tcondition;
    bool fcondition;
    bool evcondition;
    double prcondition_time;
    double tcondition_time;
    double fcondition_time;
    double evcondition_time;
    std::string computeDelay(unsigned);
};

#endif
