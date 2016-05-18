/*
 *  eventcounterset.h
 *  mngagent
 *
 *  Created by Attilio Giordana on 1/13/12.
 *  Copyright 2012 Università del Piemonte Orientale. All rights reserved.
 *
 */

#ifndef _mngagent_evcounterset_
#define _mngagent_evcounterset_
#include "eventcounter.h"

/** @brief Insieme di istanze di classe EventCounter
 Insieme di istanze di EventCounter. 
 
 @author Attilio Giordana
 @date Gennaio 2012
 */
class EventCounterSet: public setof<EventCounter> {
public:
    /**
	 Costruttore della classe. Inizializza n istanze della classe EventCounter, tante quanti sono i
     file XML presenti nella working directory dell'agente. Ogni file XML contiene tutti i parametri
     che vengono utilizzati dalle istanze di EventCounter.
	 @param dir directory che contiene i file XML con i parametri per le istanze di EventCounter
     */
	EventCounterSet(std::string dir);
    /**
     Per ogni evento presente richiama la funzione EventCounter::eventUpdate
     @param event nome dell'evento, source dell'event, comment commento dell'evento
     @return true se il contatore è maggiore di 0, false altrimenti
     */
	bool eventUpdate(MMessage& m);
    /**
     Per ogni evento presente richiama la funzione EventCounter::timeUpdate()
     @return true se il contatore è maggiore di 0, false altrimenti
     */
	bool timeUpdate();
    bool clearDQ();
    
    bool observations(MParams& obs);
};

#endif