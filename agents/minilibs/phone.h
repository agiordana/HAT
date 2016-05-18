//
//  phone.h
//  phoneagent
//
//  Created by Davide Monfrecola on 11/15/11.
//  Copyright 2011 Università del Piemonte Orientale. All rights reserved.
//

#ifndef phoneagent_phone_h
#define phoneagent_phone_h

class TiXmlNode;
class Attribute;

/** 
 @class Phone
 @brief Informazioni sui telefoni
 
 Informazioni sui numeri di telefono. Ogni istanza ha associato un numero di telefono, il
 nome della persona e il tipo. Inoltre ad ognuna viene associata una lista di allarmi di
 tipo setof<Attribute>.
 I tipi di telefono definiti sono 5: "smsring" (accetta sia sms che chiamate), 
 "sms" (solo sms), "ring" solo chiamate, "securitya" (polizia in massima priorita`), 
 "securityb" (polizia solo se nessuno degli altri ha risposto).
 @author Attilio Giordana
 @date Novembre 2011
 */
class Phone {

public:
    /** 
     @var std::string
     @brief tipo di telefono. Cinque tipi disponibili (vedere descrizione classe)
     */
    std::string ty;
    /** 
     @var std::string
     @brief nome associato al numero di telefono
     */
	std::string name;
    /** 
     @var std::string
     @brief numero di telefono associato al nome
     */
	std::string number;
    
    /**
     Costruttore della classe. Inizializza tipo(normal di default), numero e nome.
     @param n nome associato al numero
     @param p numero di telefono
     */
	Phone(std::string n, std::string p) {ty="normal"; number = p; name = n;};
	/**
     Costruttore della classe. Inizializza tipo (smsring di default), numero, nome e il 
     tipo di allarme (alarms).
     @param n nome associato al numero
     @param p numero di telefono
     */
    Phone(const char *n, const char *p) {ty="smsring"; number = std::string(p); name = std::string(n); alarms.ty="alarms";};
    /**
     Costruttore della classe. Inizializza tipo, numero, nome e il tipo di allarme (alarms).
     @param t tipo di telefono
     @param n nome associato al numero
     @param p numero di telefono
     */
	Phone(std::string t, std::string n, std::string p) {ty=t; number = p; name = n; alarms.ty="alarms";};
	/**
     Costruttore della classe. Inizializza tipo, numero, nome e il tipo di allarme (alarms).
     @param t tipo di telefono
     @param n nome associato al numero
     @param p numero di telefono
     */
    Phone(const char *t, const char *n, const char *p) {ty=std::string(t); number = std::string(p); name = std::string(n); alarms.ty="alarms";};
	/**
     Costruttore della classe.
     @param <non definito>
     */
    Phone(std::string&);
    /**
     Costruttore della classe. Inizializza tipo, numero, nome e il tipo di allarme. Tutte
     le informazioni vengono recuperate dal nodo XML passato come parametro
     @param prg 
	 puntatore ad un nodo XML che contiene le informazioni
     @see hsrv::getAttrbyName()
     */
	Phone(TiXmlNode*);
    /**
     Costruttore della classe. Inizializza tipo(normale di default), numero e nome. 
     */
	Phone(){ty="normal"; number=""; name="";};
    /**
     Crea una stringa che contiene le informazioni sul telefono in formato XML. 
     La stringa viene poi appesa ad una std::ostringstream passato per riferimento
     @param out ostringstream alla quale viene appesa la stringa creata
     @param md se > 0 inserisce l'intestazione xml
     @see Alarm::xmlPrint()
     */
	int xmlPrint(std::ostringstream& out, int md=0);
    /**
     Codifica le informazioni in XML
     @param md se > 0 inserisce l'intestazione xml
     @return stringa XML contenente le informazioni sul telefono e gli allarmi associati 
             ad esso
     @see Alarm::xmlEncode()
     */
    std::string xmlEncode(int md = 0);
    /**
     Estrae le informazioni sul telefono e sugli allarmi ad esso associati da un nodo XML
     @param desc puntatore ad un nodo XML contenente le informazioni sul telefono e gli allarmi 
            associati ad esso
     @return 1 se tutto ok, 0 altrimenti
     @see Alarm::xmlDecode()
     */
	int xmlDecode(TiXmlNode* desc);
    /**
     Estrae le informazioni sul telefono e sugli allarmi ad esso associati da una stringa
     codificata in XML
     @param xmldesc stringa XML contenente le informazioni sul telefono
     @return 1 se tutto ok, 0 altrimenti
     @see Alarm::xmlDecode()
     */
	int xmlDecode(std::string&);
    /**
     Carica il file XML con il nome specificato
     @param name nome del file XML su disco
     @return 1 se xmlDecode viene eseguita correttamente, 0 altrimenti
     @see xmlDecode(), hsrv::isfile()
     */
	int xmlLoad(std::string);
	/**
     Salva un file XML che contiene le informazioni sull'istanza di Phone codificate in XML
     @param name nome del file XML su disco
     @return 1 se tutto ok, 0 altrimenti
     @see xmlEncode()
     */
    int xmlSave(std::string);
    /**
     Cerca se l'allarme specificato è presente tra gli allarmi associati al telefono
     @param nome dell'allarme
     @return 1 è presente, 0 altrimenti
     */
	int member(std::string);
    /**
     Aggiunge un allarme alla lista degli allarmi associati al telefono
     @param al nome dell'allarme
     @return 1 se tutto ok
     @see Attribute::Attribute()
     */
	int alarmAdd(std::string al) {alarms.push_back(Attribute(al, "ON")); return 1;};

private:
    /**
     Insieme di allarmi associati al numero di telefono. Ogni allarme è rappresentato da 
     un'istanza della classe Attribute
     @param alarms nome dell'allarme
     @return 1 se tutto ok
     @see Attribute::Attribute()
     */
	setof<Attribute> alarms;
};


#endif
