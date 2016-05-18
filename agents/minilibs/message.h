//
//  message.h
//  phoneagent
//
//  Created by Davide Monfrecola on 11/15/11.
//  Copyright 2011 Università del Piemonte Orientale. All rights reserved.
//

#ifndef phoneagent_message_h
#define phoneagent_message_h

/** @brief Messaggio
 
 Messaggio che viene utilizzato per lo scambio di informazioni.
 @author Attilio Giordana
 @date Novembre 2011
 */

class Message:public std::map<std::string, std::string> {
public:
    std::string mtype;
    int priority;
    /**
     Crea una nuova istanza della classe estraendo le informazioni da un file XML
     @param desc puntatore al nodo XML contenente le informazioni
     @return 1 se xmlDecode termina correttamente
     @see xmlDecode()
     */
    Message(TiXmlNode* desc = NULL) {
		if(desc != NULL) xmlDecode(desc);
		else mtype="*";
	};
    /**
     Crea una nuova istanza della classe inizializzando la variabile che indica il tipo di 
     messaggio e la sua priorità (0 di default)
     @param t stringa che indica il tipo di messaggio
     */
	Message(std::string t) { mtype = t; priority = 0; };
    /**
     Aggiunge una nuova coppia chiave-valore alla mappa
     @param f stringa utilizzata come chiave
     @param v stringa utilizzata come valore
     @return 1 se tutto ok
     */
	int add(std::string f,std::string v){(*this)[f] = v; return 1;};
	/**
     Aggiunge una nuova coppia chiave-valore alla mappa
     @param f stringa utilizzata come chiave
     @param v intero utilizzato come valore
     @return 1 se tutto ok
     @see hsrv::int2a()
     */
    int add(std::string f,int v){std::string r=hsrv::int2a(v); (*this)[f]=r; return 1;};
	/**
     Aggiunge una nuova coppia chiave-valore alla mappa
     @param f stringa utilizzata come chiave
     @param long utilizzato come valore
     @return 1 se tutto ok
     @see hsrv::long2a
     */
    int add(std::string f,long v){std::string r=hsrv::long2a(v); (*this)[f]=r; return 1;};
	/**
     Aggiunge una nuova coppia chiave-valore alla mappa
     @param f stringa utilizzata come chiave
     @param v unsigned utilizzato come valore
     @return 1 se tutto ok
     @see hsrv::unsigned2a()
     */
    int add(std::string f,unsigned v){std::string r=hsrv::unsigned2a(v); (*this)[f]=r; return 1;};
	/**
     Aggiunge una nuova coppia chiave-valore alla mappa
     @param f stringa utilizzata come chiave
     @param v double utilizzata come valore
     @return 1 se tutto ok
     @see hsrv::double2a()
     */
    int add(std::string f,double v){std::string r=hsrv::double2a(v); (*this)[f]=r; return 1;};
	int remove(std::string f) {std::map<std::string,std::string>::iterator it; if((it=find(f))!=end()) erase(it); return 1;};
	/**
     Recupera un valore specificando la chiave
     @param f chiave da ricercare
     @return il valore della chiave cercata se esiste, stringa vuota altrimenti
     */
    std::string getString(std::string f){std::map<std::string,std::string>::iterator it; if((it=find(f))!=end()) return it->second;else return "";};
	/**
     Recupera un valore intero specificando la chiave
     @param f chiave da ricercare
     @return il valore intero della chiave cercata se esiste, 0 altrimenti
     @see hsrv::a2int()
     */
    int getInt(std::string f){std::map<std::string,std::string>::iterator it; if((it=find(f))!=end()) return hsrv::a2int(it->second);else return 0;};
	/**
     Recupera un valore long specificando la chiave
     @param f chiave da ricercare
     @return il valore long della chiave cercata se esiste, 0 altrimenti
     @see hsrv::a2long()
     */
    long getLong(std::string f){std::map<std::string,std::string>::iterator it; if((it=find(f))!=end()) return hsrv::a2long(it->second);else return 0;};
	/**
     Recupera un valore unsigned specificando la chiave
     @param f chiave da ricercare
     @return il valore unsigned della chiave cercata se esiste, 0 altrimenti
     @see hsrv::a2unsigned()
     */
    unsigned getUnsigned(std::string f){std::map<std::string,std::string>::iterator it; if((it=find(f))!=end()) return hsrv::a2unsigned(it->second);else return 0;};
	/**
     Recupera un valore double specificando la chiave
     @param f chiave da ricercare
     @return il valore double della chiave cercata se esiste, 0 altrimenti
     @see hsrv::a2double()
     */
    double getDouble(std::string f){std::map<std::string,std::string>::iterator it; if((it=find(f))!=end()) return hsrv::a2double(it->second);else return 0;};
    /**
     Ritorna una stringa con le informazioni sul messaggio in formato HTML
     @return stringa contenente le informazioni sul messaggio in formato HTML
     */
	std::string htmlEncode();
    /**
     Ritorna una stringa con le informazioni codificate per essere utulizzate in una POST HTTP
     @return stringa contenente le informazioni sul messaggio nel formato 
             "chiave=valore&chiave=valore..."
     */
	std::string htmlPostEncode();
    /**
     Ritorna una stringa con le informazioni sul messaggio in formato XML
     @param md se > 0 stampa l'intestazione XML
     @return stringa contenente le informazioni sul messaggio in formato XML
     */
    std::string xmlEncode(int md = 0);
    /**
     Aggiunge alla ostringstream passata come parametro la stringa che contiene le informazioni 
     sul messaggio in formato XML.
     @param out ostringstream che contiene la stringa XML con le informazioni sul messaggio
     @param md se > 0 stampa l'intestazione XML
     @return 1 se tutto ok
     */
	int xmlPrint(std::ostringstream&, int md=0);
    /**
     Estrae le informazioni sull'attributo da una stringa in formato XML
     @param xmldesc stringa XML con le informazioni sull'attributo
     @return 1 se tutto ok, 0 altrimenti
     @see hsrv::getAttrbyName(), hsrv::charExplode(), hsrv::charPrintf()
     */
	int xmlDecode(std::string&);
    /**
     Estrae le informazioni sull'attributo da un nodo XML
     @param desc puntatore ad un nodo XML contenente le informazioni sul messaggio
     @return 1 se tutto ok, 0 altrimenti
     @see hsrv::getAttrbyName()
     */
	int xmlDecode(TiXmlNode* desc);
    /**
     Estrae le informazioni sull'attributo da una stringa in formato XML e aggiunge la coppia 
     chiave-valore alla mappa
     @param m stringa XML con le informazioni sul messaggio
     @return 1 se tutto ok, 0 altrimenti
     @see hsrv::getAttrbyName()
     */
	int xmlAppend(std::string&);
    /**
     Estrae le informazioni sull'attributo dalla stringa passata come parametro
     @param m query string HTML con le informazioni sul messaggio
     @return 1 se tutto ok, 0 altrimenti
     @see hsrv::getAttrbyName()
     */
	int htmlAppend(std::string&);
	/**
     Restituisce il valore della home directory.
     @return home directory
     @see hsrv::homedir
     */
	std::string gethomedir() { return hsrv::homedir; };
};


#endif
