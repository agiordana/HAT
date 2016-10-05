/*
 *  ruleset.h
 *  mngagent
 *
 *  Created by Attilio Giordana on 1/13/12.
 *  Copyright 2012 Università del Piemonte Orientale. All rights reserved.
 *
 */

#ifndef _mngagent_ruleset_
#define _mngagent_ruleset_

#include "agentlib.h"
#include "rule.h"
#include "eventcounterset.h"
#include "BooleanConditionSet.h"

class RuleSet: public setof<Rule> {
public:
    /**
	 Costruttore della classe. Inizializza n istanze della classe Rule, tante quanti sono i file XML
     presenti nella working directory dell'agente. Ogni file XML contiene tutti i parametri che 
     vengono utilizzati dalle istanze di Rule.
	 @param dir directory che contiene i file XML con i parametri per le istanze di Rule
     */
	RuleSet(std::string);
    /**
     
	 @param ec istanza di EventCounterSet, contiene istanze di EventCounter
     @param action insieme di istanze di Message che contengono le informazioni da inviare come
                   risposta al client 
     @return numero di matching trovati
     */
	int match(EventCounterSet* ec, BooleanConditionSet* cnd, setof<MMessage>& action);
	/**
     * Imposta il tempo di attesa da utilizzare per la modalità program_wait
     * @param wt tempo di attesa in secondi
     * @return ritorna il tempo di attesa impostato
     */
    bool setValidityTime(std::string, std::string, std::string);
    bool setStatus(std::string, std::string);
    bool subjects(MParams& subj);
    bool timeUpdate();
protected:
    /**
     * Controlla se la regola è attiva
     * @param r istanza di Rule, contiene le informazioni sulla regola
     * @param active 
     * @return true se la regola deve essere eseguita, false altrimenti
     */
	bool isON(Rule&);
};

#endif
