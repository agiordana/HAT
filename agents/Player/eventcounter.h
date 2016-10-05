/*
 *  eventcounter.h
 *  mngagent
 *
 *  Created by Attilio Giordana on 1/13/12.
 *  Copyright 2012 Università del Piemonte Orientale. All rights reserved.
 *
 */

#ifndef _mngagent_evcounter_
#define _mngagent_evcounter_

#include "agentlib.h"

class Event {
public:
	double time;
	std::string source;
	std::string video;
	Event(std::string s, double t) {source = s; video = ""; time = t;};
	Event(std::string s, std::string v, double t) {source = s; video = v; time = t;};
};

class EvList : public std::vector<Event> {
public:
	int update(double);
	std::string history();
	std::string video();
};

class DelayQueue: public std::vector<MMessage> {
public:
    DelayQueue(std::string d) {delay = hsrv::a2double(d); next_deadline = 0;};
    bool messageGet(MMessage& m);
    bool messageAppend(MMessage& m);
    bool print();
private:
    double delay;
    double next_deadline;
    unsigned next_message;
    bool deadlineUpdate();
};

/** @brief Contatore eventi
 Mantiene un contatore degli eventi. Gli eventi possono essere singoli o un aggregato di eventi
 singoli. Per ogni evento o classe di eventi vengon mantenute una serie di informazioni.
 @author Attilio Giordana
 @date Gennaio 2012
 */
class EventCounter: public MParams {
public:
    std::string name;
    /**
	 Crea una nuova istanza caricando i parametri da un file XML. Inizializza le variabili che 
     contengono le informazioni su
       - soglia
       - valore per l'incremento
       - valore per il decremento
	 @param dir directory dove si trova il file XML
     @param name nome dell'event counter per il quale creare un'istanza
     */
	EventCounter(std::string, std::string);
    /**
     Incrementa il contatore di dIncrement se corrisponde con il nome passato come parametro o se 
     fa parte della lista alias (istanza di NameList)
     @param name nome dell'evento, source source dell'evento, comment comment dell'evento
     @return valore del contatore 
     */
	double eventUpdate(MMessage& m);
    /**
     Decrementa il valore del contatore in base al timestamp dell'ultimo update e del timestamp
     attuale. Sostituisce polling per aggiornamento del counter.
     @return valore del contatore 
     */
	double timeUpdate();
    /**
     Controlla se l'eventCounter è attivo.
     @return true se il numero di eventi è maggiore della soglia impostata, false altrimenti 
     */
	bool isActive() { return (counter >= threshold); };
	/**
     Costruisce una stringa contenente la storia degli ultimi eventi registrati.
     @return la stringa della storia
     */	
	std::string getHistory() {return history.history();};
	std::string getVideo() {return history.video();};
	std::string getComment() {return comment;};
    bool clearDQ();
    
    bool observations(MParams& obs);
    
protected:
    DelayQueue* dq;
    /**
     @var double
     @brief contatore di eventi
     */
	double counter;
	/**
     @var double
     @brief valore da utilizzare per l'incremento del valore del counter
     */
    double dIncrement;
	/**
     @var double
     @brief valore da utilizzare per decremento del valore del counter
     */
    double dDecrement;
	/**
     @var double
     @brief valore per la soglia che il contatore non deve sorpassare
     */
    double threshold;
    /**
     @var NameList
     @brief lista di alias per gli eventi aggregati
     */
	NameList alias;
        /**
     @var NameList
     @brief lista dei valori in ingressi che lo attivano
     */
	NameList values;
    /**
     @var double
     @brief tempo di ultimo update (ms)
     */
	double last_time;
	/**
     @var double
     @brief tempo per l'update attuale (ms)
     */
    double current_time;
	/**
     @var EvList
     @brief storia degli ultimi eventi che sono stati registrati
     */	
	EvList history;
	/**
	 @var comment
	 @brief stringa che contiene il commento dell'ultimo event registrato
	 */
	std::string comment;
    double Update(MMessage& m);
    bool match(MMessage& m);
};

#endif
