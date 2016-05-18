//
//  alarm.h
//  phoneagent
//
//  Created by Davide Monfrecola on 11/15/11.
//  Copyright 2011 Università del Piemonte Orientale. All rights reserved.
//

#ifndef phoneagent_alarm_h
#define phoneagent_alarm_h

/** @brief Informazioni sull'allarme
 
 Informazioni su un allarme. Ogni istanza ha associato il nome dell'allarme, la sua priorità e il 
 numero di ripetizioni da effettuare. 
 @author Attilio Giordana
 @date Novembre 2011
 */
class Alarm {
public:
    /**
     @var std::string
     @brief nome dell'allarme
     */
    std::string name;
	/**
     @var std::string
     @brief sorgente dal quale è partito l'allarme
     */
    std::string source;
    /**
     @var std::string
     @brief commento associato all'allarme
     */
	std::string comment;
    /**
     @var int
     @brief priorità associata all'allarme
     */
	int priority;
    /**
     @var unsigned
     @brief numero di ripetizioni da effettuare
     */
	unsigned nrepeat;
    /**
     Costruttore della classe
     @param stdstringa 
     */
	Alarm(std::string&);
	/**
     Costruttore della classe attraverso un file XML che contiene le informazioni sull'allarme
     @param desc puntatore ad un nodo XML
     @see xmlDecode()
     */
    Alarm(TiXmlNode* desc){ xmlDecode(desc); };
	/**
     Costruttore della classe. Le informazioni sull'allarme vengono passate come parametri.
     Inizializza il tipo di lista a "phonelist" e a stringa vuota la sorgente e il commento
     @param n nome dell'allarme
     @param p priorità associata all'allarme
     @param r numero di ripetizioni (valore di default = 1)
     @see xmlDecode()
     */
    Alarm(std::string n, int p, unsigned r=1) {name=n; priority=p; nrepeat=r; phonelist.ty="phonelist";source=comment="";};
	/**
     Costruttore della classe senza parametri. 
     @see xmlDecode()
     */
    Alarm(){name = source = comment = ""; priority = 0; nrepeat = 0; phonelist.ty = "phonelist";};
    /**
     Inizializza la lista dei telefoni prendendo le informazioni dall'insieme di istanze della
     classe Phone passata come parametro
     @param ph insieme di istanze della classe Phone
     @return 1 se tutto ok
     */
	int initPhoneList(setof<Phone>&);
	/**
     Aggiunge alla ostringstream passata come parametro la stringa che contiene le informazioni 
     sull'allarme in formato XML.
     @param out ostringstream che contiene la stringa XML con le informazioni sull'allarme
     @param md se > 0 stampa l'intestazione XML
     @return 1 se tutto ok
     */
    int xmlPrint(std::ostringstream& out, int md = 0);
	/**
     Ritorna una stringa con le informazioni sull'allarme in formato XML
     @param md se > 0 stampa l'intestazione XML
     @return stringa contenente le informazioni sull'allarme in formato XML
     */
    std::string xmlEncode(int md=0);
    /**
     Estrae le informazioni sull'allarme da una stringa in formato XML
     @param xmldesc stringa XML con le informazioni sull'attributo
     @return 1 se tutto ok, 0 altrimenti
     @see hsrv::getAttrbyName()
     */
	int xmlDecode(std::string&);
    /**
     Estrae le informazioni sull'allarme da un nodo XML
     @param desc puntatore ad un nodo XML contenente le informazioni sull'attributo
     @return 1 se tutto ok, 0 altrimenti
     @see hsrv::getAttrbyName()
     */
	int xmlDecode(TiXmlNode*);
    /**
     Carica il file XML con il nome specificato
     @param name nome del file XML su disco
     @return 1 se xmlDecode viene eseguita correttamente, 0 altrimenti
     @see xmlDecode(), hsrv::isfile()
     */
	int xmlLoad(std::string);
    /**
     Salva un file XML che contiene le informazioni sull'istanza di Alarm 
     codificate in XML
     @param name nome del file XML su disco
     @return 1 se tutto ok, 0 altrimenti
     @see xmlEncode()
     */
	int xmlSave(std::string);
    /**
     @var setof<Phone>
     @brief insieme di istanze di Phone con le informazioni sui numeri di telefoni associati 
            all'allarme
     */
	setof<Phone> phonelist;
};

#endif



