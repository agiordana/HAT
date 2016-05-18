/* 
 * File:   MMessage.h
 * Author: attilio
 *
 * Created on November 10, 2012, 6:06 PM
 */

#ifndef MMESSAGE_H
#define	MMESSAGE_H


class MstringList: public std::vector<std::string> {
public:
    MstringList();
    MstringList(std::string);
    MstringList(std::vector<std::string>&);
    bool add(std::string);
    bool add(std::vector<std::string>&);
    std::string xmlEncode(std::string);
    std::string htmlEncode(std::string);
    bool xmlDecode(std::string);
    bool xmlDecode(TiXmlNode*);
    bool xmlPrint(std::ostringstream& out, int md=0);
    std::string getString();
    std::vector<std::string> getList();
    std::string getString(unsigned);
    std::string hasType() {return mytype;};
protected:
    std::string mytype;
};

class MMessage:public std::map<std::string,MstringList> {
public:
    std::string mtype;
    std::string msubtype;
    int priority;
    /**
     Crea una nuova istanza della classe estraendo le informazioni da un file XML
     @param desc puntatore al nodo XML contenente le informazioni
     @return 1 se xmlDecode termina correttamente
     @see xmlDecode()
     */
    MMessage(TiXmlNode* desc = NULL) {
		if(desc != NULL) xmlDecode(desc);
		else {
                    mtype="*";
                    msubtype="*";
                    priority=0;
                }
	};
    
/**
     Crea una nuova istanza della classe inizializzando la variabile che indica il tipo di 
     messaggio e la sua priorità (0 di default). Il sottotipo vene inizializzato a * (valore di default)
     @param t stringa che indica il tipo di messaggio
     */
    MMessage(std::string t) { mtype = t; msubtype="*"; priority = 0; };
     /**
     Crea una nuova istanza della classe inizializzando la variabile che indica il tipo di 
     messaggio, il sottotipo e la priorità (0 di default)
     @param t stringa che indica il tipo di messaggio
     */
    MMessage(std::string t, std::string st) { mtype = t; msubtype=st; priority = 0; };
    ~MMessage(){clear();};
    /**
     Aggiunge una nuova coppia chiave-valore alla mappa
     @param f stringa utilizzata come chiave
     @param v stringa utilizzata come valore
     @return true se tutto ok
     */
    
    bool add(std::string,std::string);
	/**
     Aggiunge una nuova coppia chiave-valore alla mappa
     @param f stringa utilizzata come chiave
     @param v intero utilizzato come valore
     @return true se tutto ok
     @see hsrv::int2a()
     */
    bool add(std::string f,int v);
	/**
     Aggiunge una nuova coppia chiave-valore alla mappa
     @param f stringa utilizzata come chiave
     @param long utilizzato come valore
     @return true se tutto ok
     @see hsrv::long2a
     */
    bool add(std::string f,long v);
	/**
     Aggiunge una nuova coppia chiave-valore alla mappa
     @param f stringa utilizzata come chiave
     @param v unsigned utilizzato come valore
     @return true se tutto ok
     @see hsrv::unsigned2a()
     */
    bool add(std::string f,unsigned v);
	/**
     Aggiunge una nuova coppia chiave-valore alla mappa
     @param f stringa utilizzata come chiave
     @param v double utilizzata come valore
     @return true se tutto ok
         * 
     @see hsrv::double2a()
     */
    bool add(std::string f,double v);
    
    bool add(std::string, std::vector<std::string>&);
    
    /**
     * Recupera tutti gli elementi che fanno parte del messaggio
     *
     * @return mappa di stringhe che ha come chiave il nome dell'attributo e come
     *         valore il suo valore
     */
    std::map<std::string, std::string> getElements();
    
    bool remove(std::string f);
	/**
     Recupera un valore specificando la chiave
     @param f chiave da ricercare
     @return il valore della chiave cercata se esiste, stringa vuota altrimenti
     */
     std::string getString(std::string);
     /**
     Recupera un valore intero specificando la chiave
     @param f chiave da ricercare
     @return il valore intero della chiave cercata se esiste, 0 altrimenti
     @see hsrv::a2int()
     */
    int getInt(std::string f);
 /**
     Recupera un valore long specificando la chiave
     @param f chiave da ricercare
     @return il valore long della chiave cercata se esiste, 0 altrimenti
     @see hsrv::a2long()
     */
    long getLong(std::string f);
	/**
     Recupera un valore unsigned specificando la chiave
     @param f chiave da ricercare
     @return il valore unsigned della chiave cercata se esiste, 0 altrimenti
     @see hsrv::a2unsigned()
     */
    unsigned getUnsigned(std::string f);
	/**
     Recupera un valore double specificando la chiave
     @param f chiave da ricercare
     @return il valore double della chiave cercata se esiste, 0 altrimenti
     @see hsrv::a2double()
     */
    double getDouble(std::string f);
    
    std::vector<std::string> getList(std::string f);
            
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
    bool xmlPrint(std::ostringstream&, int md=0);
    /**
     Estrae le informazioni sull'attributo da una stringa in formato XML
     @param xmldesc stringa XML con le informazioni sull'attributo
     @return 1 se tutto ok, 0 altrimenti
     @see hsrv::getAttrbyName(), hsrv::charExplode(), hsrv::charPrintf()
     */
    bool xmlDecode(std::string&);
    /**
     Estrae le informazioni sull'attributo da un nodo XML
     @param desc puntatore ad un nodo XML contenente le informazioni sul messaggio
     @return 1 se tutto ok, 0 altrimenti
     @see hsrv::getAttrbyName()
     */
    bool xmlDecode(TiXmlNode* desc);
    /**
     Estrae le informazioni sull'attributo da una stringa in formato XML e aggiunge la coppia 
     chiave-valore alla mappa
     @param m stringa XML con le informazioni sul messaggio
     @return 1 se tutto ok, 0 altrimenti
     @see hsrv::getAttrbyName()
     */
    bool xmlAppend(std::string&);
    /**
     Estrae le informazioni sull'attributo dalla stringa passata come parametro
     @param m query string HTML con le informazioni sul messaggio
     @return 1 se tutto ok, 0 altrimenti
     @see hsrv::getAttrbyName()
     */
    bool htmlAppend(std::string&);
	/**
     Restituisce il valore della home directory.
     @return home directory
     @see hsrv::homedir
     */
    std::string gethomedir() { 
            return hsrv::homedir; 
    };

    bool myclear(int md=0);
        
};


#endif	/* MMESSAGE_H */

