/*
 *  agentlib.h
 *  phoneagent
 *
 *  Created by Attilio Giordana on 10/27/11.
 *  Copyright 2011 Università del Piemonte Orientale. All rights reserved.
 *
 */

#ifndef _AGENTLIB_
#define _AGENTLIB_

#include <algorithm>
#include <bitset>
#include <dirent.h>
#include <errno.h>
#include <fstream>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <math.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/timeb.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <vector>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp> 
#include <boost/thread/locks.hpp> 
#include <boost/thread/condition.hpp> 
#include <boost/filesystem.hpp>

#define MAXB 65000
#define NUMSZ 256
#define TOUT 15
#define MAXCONN 6
#define HDLEN 1024

#define ON 1
#define OFF 0
#define ERR 3
#define WAIT 4
#define NANOPERSECOND 1000000000
#define OFFVALUE -10000000000
#define PRIVATE 0
#define PUBLIC 1

// Program, DayProgram e Period define
#define ITALIAN

#define WEEK 1

#ifdef ITALIAN

#define MON "LUN"
#define TUE "MAR"
#define WED "MER"
#define THU "GIO"
#define FRI "VEN"
#define SAT "SAB"
#define SUN "DOM"
#define FROM "Da"
#define TO "A"
#define DAY "Giorno"
#define ACTIVE "Periferici Attivi"
#define TODAY "/_Oggi"
#else
#define MON "MON"
#define TUE "TUE"
#define WED "WED"
#define THU "THU"
#define FRI "FRI"
#define SAT "SAT"
#define SUN "SUN"
#define FROM "From"
#define TO "To"
#define DAY "Day"
#define ACTIVE "Active Devices"
#define TODAY "/_Today"

#endif

#include "hsrv.h"
#include "template.h"
#include "exceptions.h"
#include "logger.h"
#include "attribute.h"
#include "Mattribute.h"
#include "net.h"
#include "iplocation.h"
#include "phone.h"
#include "MMessage.h"
#include "message.h"
#include "alarm.h"
#include "httpserver.h"
#include "filemanager.h"
#include "period.h"
#include "dayprogram.h"
#include "program.h"
#include "tinyxml.h"
#include "tinystr.h"
#include "namelist.h"
#include "pwrprogram.h"
#include "RestHttpServer.h"
#include "Route.h"
#include "ArchiveRoute.h"
#include "CmdRoute.h"
#include "LoggerRoute.h"

#define _DEBUG_ 0

const unsigned int NUM_INDENTS_PER_SPACE=2;

/**
 @class Params
 @brief Insieme di parametri (istanze della classe Attribute)
 Insieme di paramentri che vengono utilizzati dalle varie classi. Ogni parametro è un'istanza della 
 classe Attribute e per gestirli viene utilizzato il template setof<Attribute>
 */
class Params: public setof<Attribute> {
public:
    
    /**
     Costruttore della classe. Inizializza le variabili d'istanza.
     @param name stringa che indica il tipo del parametro
     @see hsrv::configdir
     */
	Params(std::string name){ty = name; path = hsrv::configdir+"/"+name+".xml"; pathh = hsrv::homedir+"/"+name+".xml";};
	
    /**
     Costruttore della classe. Inizializza le variabili d'istanza.
	 @param dir sotto-directory di hsrv::configdir dove risiede la descrizione dell'istanza
     @param name stringa che indica il tipo del parametro
     @see hsrv::configdir
     */	
	Params(std::string dir, std::string name){
		ty = name; path = hsrv::configdir+"/"+dir+"/"+name+".xml"; pathh = hsrv::homedir+"/"+dir;
		FileManager::makeDir(pathh, false);
		pathh = pathh + "/"+name+".xml";
	};
    
    /**
     Carica un insieme di attributi da un file XML
     @param path del file XML che contiene gi attributi da caricare
     @return 1 se tutto ok, 0 altrimenti
     @see setof::xmlLoad()
     */
    int load() { if(hsrv::isfile(pathh)) return xmlLoad(pathh); else return xmlLoad(path); };
	
    /**
     Salva su file gli attributi in formato XML
     @param path del file XML su cui salvare gli attributi
     @return 1 se tutto ok, 0 altrimenti
     @see setof::xmlSave()
     */
    int save() { return xmlSave(pathh); };
	
    /**
     Recupera il valore dell'attributo con il nome passato.
     @param name nome dell'attributo da cercare
     @return valore dell'attributo se esiste, stringa vuota altrimenti
     */
    std::string get(std::string name) {
		std::string r = "";
		for (unsigned i = 0; i < size() && r == ""; i++) 
            if((*this)[i].name == name) 
                r = (*this)[i].value;
		return r;
	}
	
    /**
     Assegna il valore all'attributo con il nome passato se l'attributo esiste. Altrimenti non fa nulla.
     @param name nome dell'attributo da cercare
	 @param value valore da assegnare all'attributo name
     @return 1 se l'attributo esiste, 0 altrimenti
     */
    int set(std::string name, std::string value) {
		int r = 0;
		for (unsigned i = 0; i < size() && r == 0; i++) 
            if((*this)[i].name == name) {
				(*this)[i].value=value;
				r = 1;
			}
		return r;
	}	
    
    /**
     Controlla se esiste un attributo con il valore passato come parametro 
     @param value valore dell'attributo da cercare
     @return 1 se è presente nell'insieme, 0 altrimenti
     */
    int checkValue(std::string value){
        for(unsigned i = 0; i < this->size(); i++) 
            if((*this)[i].value == value) 
                return 1;
        return 0;
    };
	
    /**
     Aggiorna l'insieme di attributi prendendo i valori dal messaggio passato come parametro
     @param m il messaggio da cui prelevare i nuovi valori
     @return il numero di valori modificati
     */
	int update(Message& m){
		int n=0;
		for(unsigned i=0; i<size(); i++) 
			if(m.getString((*this)[i].name)!="") {
				(*this)[i].value=m.getString((*this)[i].name);
				n++;
			}
		save();
		return n;
	}
	
    /**
	 Ripristina i valori originari leggendoli dal file di configurazione
	 @return 1 se l'operazione ha avuto successo
	 */
	int reset() {return xmlLoad(path);};
private:
    
    /** 
     @var std::string
     @brief path del file di configurazione in formato XML
     */
	std::string path;
	std::string pathh;
	
};


class MParams: public setof<MAttribute> {
public:
    
    /**
     Costruttore della classe. Inizializza le variabili d'istanza.
     @param name stringa che indica il tipo del parametro
     @see hsrv::configdir
     */
	MParams(std::string name){ty = name; path = hsrv::configdir+"/"+name+".xml"; pathh = hsrv::homedir+"/"+name+".xml";};
	
    /**
     Costruttore della classe. Inizializza le variabili d'istanza.
	 @param dir sotto-directory di hsrv::configdir dove risiede la descrizione dell'istanza
     @param name stringa che indica il tipo del parametro
     @see hsrv::configdir
     */	
	MParams(std::string dir, std::string name){
		ty = name; path = hsrv::configdir+"/"+dir+"/"+name+".xml"; pathh = hsrv::homedir+"/"+dir;
		FileManager::makeDir(pathh, false);
		pathh = pathh + "/"+name+".xml";
	};
    
	MParams() {ty="nonname"; path="/tmp"; pathh = "/tmp"; };
    
    /**
     Carica un insieme di attributi da un file XML
     @param path del file XML che contiene gi attributi da caricare
     @return 1 se tutto ok, 0 altrimenti
     @see setof::xmlLoad()
     */
    int load() {if(hsrv::isfile(pathh)) return xmlLoad(pathh); else return xmlLoad(path); };
	
    /**
     Salva su file in "home" gli attributi in formato XML
     @param path del file XML su cui salvare gli attributi
     @return 1 se tutto ok, 0 altrimenti
     @see setof::xmlSave()
     */
    int save() { return xmlSave(pathh); };
    /**
     Salva su file in "config" gli attributi in formato XML
     @param path del file XML su cui salvare gli attributi
     @return 1 se tutto ok, 0 altrimenti
     @see setof::xmlSave()
     */
    int config() {return xmlSave(path);};
    /**
     Cancella i files su cui è salvato l'isieme di parametri nella home e nella config dir
     @param path del file nella config dir
     @param pathh del file nella home dir
     @return 1
    */
    int remove() { FileManager::deleteFile(path); FileManager::deleteFile(pathh); return 1; };
    /**
      Recupera il valore dell'attributo con il nome passato.
     @param name nome dell'attributo da cercare
     @return valore dell'attributo se esiste, stringa vuota altrimenti
     */
    std::string get(std::string name, std::string category) {
        std::string r = "";
	for (unsigned i = 0; i < size() && r == ""; i++) 
            if((*this)[i].name == name && (*this)[i].category == category) 
                r = (*this)[i].value;
	return r;
    };
    
    /**
     Recupera il valore dell'attributo con il nome passato.
     @param name nome dell'attributo da cercare
     @return valore dell'attributo se esiste, stringa vuota altrimenti
     */
    std::string get(std::string name) {
	std::string r = "";
	for (unsigned i = 0; i < size() && r == ""; i++) 
            if((*this)[i].name == name && (*this)[i].category == "generic") 
                r = (*this)[i].value;
		return r;
	}
	
       /**
     Recupera il valore dell'attributo con il nome passato.
     @param name nome dell'attributo da cercare
     @return valore dell'attributo se esiste, stringa vuota altrimenti
     */
    std::string categoryof(std::string name) {
		std::string r = "";
		for (unsigned i = 0; i < size() && r == ""; i++) 
            if((*this)[i].name == name) 
                r = (*this)[i].category;
		return r;
    }

    /**
     Assegna il valore all'attributo con il nome passato se l'attributo esiste. Altrimenti non fa nulla.
     @param name nome dell'attributo da cercare
	 @param value valore da assegnare all'attributo name
     @return 1 se l'attributo esiste, 0 altrimenti
     */
    int set(std::string name, std::string category, std::string value) {
	int r = 0;
	for (unsigned i = 0; i < size() && r == 0; i++) 
            if((*this)[i].name == name && (*this)[i].category == category) {
				(*this)[i].value=value;
				r = 1;
			}
	return r;
    }
    /**
     Assegna il valore all'attributo con il nome passato se l'attributo esiste. Altrimenti non fa nulla.
     @param name nome dell'attributo da cercare
	 @param value valore da assegnare all'attributo name
     @return 1 se l'attributo esiste, 0 altrimenti
     */
    int set(std::string name, std::string value) {
	int r = 0;
	for (unsigned i = 0; i < size() && r == 0; i++) 
            if((*this)[i].name == name && (*this)[i].category == "generic") {
				(*this)[i].value=value;
				r = 1;
			}
	return r;
     }	
    /**
     Assegna il valore all'attributo con il nome passato se l'attributo esiste. Altrimenti lo aggiunge.
     @param name nome dell'attributo da cercare
	 @param value valore da assegnare all'attributo name
     @return 1 
     */
    int add(std::string name, std::string category, std::string value) {
        int r = 0;
        for (unsigned i = 0; i < size() && r == 0; i++) 
            if((*this)[i].name == name && (*this)[i].category == category) {
				(*this)[i].value=value;
				r = 1;
			}
        if(r==0) {
            push_back(MAttribute(name,category,value));
        }
        return 1;
    };
    /**
     Assegna il valore all'attributo con il nome passato se l'attributo esiste. Altrimenti lo aggiunge.
     @param name nome dell'attributo da cercare
	 @param value valore da assegnare all'attributo name
     @return 1 
     */
    int add(std::string name, std::string value) {
        int r = 0;
        for (unsigned i = 0; i < size() && r == 0; i++) 
            if((*this)[i].name == name && (*this)[i].category == "generic") {
				(*this)[i].value=value;
				r = 1;
			}
        if(r==0) {
            push_back(MAttribute(name,"generic",value));
        }
        return 1;
    };	
     /**
     Controlla se esiste un attributo della categoria e del valore passati come parametro 
     @param value valore dell'attributo da cercare
     @return 1 se è presente nell'insieme, 0 altrimenti
     */
    int checkValue(std::string value, std::string category){
        for(unsigned i = 0; i < this->size(); i++) 
            if((*this)[i].value == value) 
                return 1;
        return 0;
    };
    /**
     Controlla se esiste un attributo con il valore passato come parametro 
     @param value valore dell'attributo da cercare
     @return 1 se è presente nell'insieme, 0 altrimenti
     */
    int checkValue(std::string value){
        for(unsigned i = 0; i < this->size(); i++) 
            if((*this)[i].value == value) 
                return 1;
        return 0;
    };
    /**
     * ritorna l'insieme di tutti gli attributi che hanno il nome passato come parametro
     * @param name
     * @return 
     */
     setof<MAttribute> extractNamed(std::string name) {
        setof<MAttribute> r;
        for(unsigned i = 0; i < this->size(); i++) 
            if((*this)[i].name == name) r.push_back((*this)[i]);
        return r;  
    };
    /**
     * ritorna l'insieme di tutti gli attributi che hanno la categoria passata come parametro
     * @param category
     * @return 
     */

    setof<MAttribute> extractCategory(std::string category) {
        setof<MAttribute> r;
        for(unsigned i = 0; i < this->size(); i++) 
            if((*this)[i].category == category) r.push_back((*this)[i]);
        return r;
    };
    
    /**
     Aggiorna l'insieme di attributi prendendo i valori dal messaggio passato come parametro
     @param m il messaggio da cui prelevare i nuovi valori
     @return il numero di valori modificati
     */
     int update(MMessage& m){
		int n=0;
		for(unsigned i=0; i<size(); i++) 
			if(m.getString((*this)[i].name)!="") {
				(*this)[i].value=m.getString((*this)[i].name);
				n++;
			}
		save();
		return n;
	}
	
    /**
	 Ripristina i valori originari leggendoli dal file di configurazione
	 @return 1 se l'operazione ha avuto successo
	 */
	int reset() {return xmlLoad(path);};
private:
    
    /** 
     @var std::string
     @brief path del file di configurazione in formato XML
     */
	std::string path;
	std::string pathh;
	
};

class AgentConf {
public:
    /**
     @var std::string
     @brief Nome dell'agente
     */
    std::string agentname;
    
    /**
     @var std::string
     @brief indica lo stato dell'agente
     */
     static int status;
    
    /**
     Costruttore della classe. Inizializza le variabili d'istanza.
     @param puntatore ad un'istanza della classe Message che contiene tutte le informazioni sulla
     richiesta ricevuta
     @return istanza della classe Message che contiene le informazioni sulla risposta che viene 
     fornita al client
     @see getString(), add(), deleteFile()
     */
    AgentConf();
	
    /**
     Risponde alle richieste effettuate dal client tramite una POST. In base alla richiesta il 
     server imposta il formato della risposta che può essere in XML o in HTML.
     @param m puntatore ad un'istanza della classe Message che contiene tutte le informazioni sulla
            richiesta ricevuta
     @return istanza della classe MMessage che contiene le informazioni sulla risposta che viene 
             fornita al client
     @see getString(), add(), deleteFile()
     */
    virtual MMessage postExecM(MMessage*);
    
    /**
     Risponde alle richieste effettuate dal client tramite una GET. In base alla richiesta il 
     server imposta il formato della risposta che può essere in XML o in HTML.
     @param m puntatore ad un'istanza della classe Message che contiene tutte le informazioni sulla
     richiesta ricevuta
     @return istanza della classe MMessage che contiene le informazioni sulla risposta che viene 
     fornita al client
     @see getString(), add()
     */
	virtual MMessage getExecM(MMessage*);
	
    /**
     Aggiorna il valore della variabile currentday recuperando il valore del giorno corrente
     @return 1 se tutto ok 
     @see hsrv::getday()
     */
	int currDayUpd() {currentday = hsrv::getday(); return 1;};
	
    /**
     Controlla se il valore della variabile currentday è diverso dal giorno corrente
     @return 1 se diverso, 0 altrimenti 
     @see hsrv::getday()
     */
    int isNewDay();
        
     /**
	 Imposta il valore del campo status: {ON, OFF}. Se esiste un valore salvato nel file CURRENTSTATUS si imposta
	 il valore del file. Altrimenti si imposta il valore ON.
	 @return il valore dello stato corrente;
	 */
	static int setStatus();
	
    /**
	 Imposta il valore del campo status: {ON, OFF}. prendendolo dal parametro st. il file CURRENTSTATUS viene aggiornato
	 di conseguenza.
	 @param st il valore da impostare nel campo status
	 @return il valore dello stato corrente;
	 */	
	static int setStatus(int st);
    /**
     Metodo usato per registrare una pagina nell'indice
     @param url, l'url da usare sul broewser per accedere alla pagina
     @param page la pagina registrata in formato di MMessage da inviare come risposta
     */
    static bool pageRegister(std::string url, MMessage& page);
    /**
     La tavola in cui si registrano le pagine WEB accessibili da un browser
     */
    static std::map<std::string,MMessage> urlindex;


protected:
    static boost::mutex mutexclusion;
    /**
     @var std::string
     @brief contiene una stringa che rappresenta il giorno corrente nel formato "aaaa_mm_gg"
     */
	std::string currentday;
    
    /**
     @var int
     @brief codice errore
     */
    int myerr;
	
    /**
     @var int
     @brief numero massimo di fallimenti
     */
    int maxfail;
	
    /**
     @var int
     @brief numero di chiamate effettuate
     */
    int callmade;
    
    /**
     @var HttpServer*
     @brief puntatore all'istanza del server HTTP che viene istanziato nel 
     *      costruttore dell'agente
     */
    HttpServer* http_server;

    /**
     @var HttpServer*
     @brief puntatore all'istanza del server HTTP che viene istanziato nel 
     *      costruttore dell'agente
     */
     RestService* rest_server;
    
    /**
     Crea un file di lock (.lock) nella home directory dell'agente contenente il pid del suo
     processo
     @return 0 se il file è stato creato correttamente, -1 altrimenti
     */
	int mklock();
    
    /**
     Recupera il pid dell'agente dal file di lock presente nella sua home directory
     @return pid processo agente, 0 se la lettura non è andata a buon fine
     */
	pid_t getlockvalue();
    
    /**
     Salva su file la stringa passata come secondo parametro
     @param file nome file nella home directory dell'agente sul quale salvare la stringa
     @param content stringa da scrivere su file
     @return 1 se tutto ok
     */
	static int saveCurrent(std::string, std::string);
    
    /**
     Controlla se l'estensione del file corrisponde a quella passata come secondo parametro
     @param fname nome del file sul quale effettuare il controllo
     @param ext estensione
     @return 1 se corrisponde, 0 altrimenti
     */
	int matchextension(std::string, std::string);
    
    /**
     Salva il file di configurazione su disco
     @param ty tipo del file
     @param name nome del file
     @param body contenuto da scrivere su file
     @return 1 se il file è stato creato correttamente, 0 altrimenti
     */
	int saveFile(std::string&, std::string&, std::string&, bool raw=false);
    
    /**
     Elimina il file di configurazione passato come parametro
     @param type tipo del file
     @param name nome del file
     @return 1 se il file è stato eliminato correttamente, 0 altrimenti
     */
	int deleteFile(std::string&, std::string&);
	
    /**
     Recupera una lista di file con l'estensione specificata nella directory passata come primo 
     parametro
     @param dir directory nella quale cercare i file
     @param ext estensione dei file da recuperare
     @return puntatore ad un vettore di stringhe
     @see matchextension()
     */
    std::vector<std::string>* getdirlist(std::string, std::string);
    
    /**
     Recupera una lista di tutti i file presenti nella directory specificata
     @param path path nel quale cercare i file
     @param flist puntatore ad un vettore di stringhe che contiene i nomi di tutti i file trovati
     @return 1 se tutto ok, 0 altrimenti
     */
	int getFilelist(std::string, std::vector<std::string>*);
    
    /**
     Imposta come working directory la directory home dell'agente
     @return 1 se chdir termina correttamente, 0 altrimenti
     @see chdir()
     */
	int sethomedir();
    
    /**
     * Istanzia un oggeto di tipo HttpServer per la gestione delle richieste HTTP
     * 
     * @return true se l'inizializzazione va a buon fine, false altrimenti 
     */
    bool start_server();
    bool start_rest_server();
    static MMessage getStatus(std::string);
    MMessage getIAA();
    MMessage getPage(MMessage*);
    MMessage mkIndexPage(std::string url);
    /**
     Prepara la risposta per le richieste che non riesce a soddisfare
     @param m puntatore al messaggio ricevuto
	 @return istanza di message con specificato il linguaggio (XML o HTML) e metodo(XML) o 
     body(HTML) che contiene la stringa "unknown"
     @see hsrv::getday()
     */
    MMessage mkUnknownM(MMessage*);
    MMessage mkHomePage(MMessage*);
    std::string hex2ascii(std::string);
    unsigned hexCharDecode(unsigned);
};

#include "MMessageQueue.h"
#include "Thread.h"
#include "remoteagent.h"
#include "MRemoteAgent.h"
#include "Observer.h"
#include "ObserverTCP.h"
#include "Subject.h"
#include "SubjectSet.h"
#include "Trigger.h"
#include "agentnet.h"
#include "cipher.h"
#include "udpclient.h"
#include "udpserver.h"
#include "ArchiveManager.h"
#include "RestHttpServer.h"
#include "RpcCall.h"
#include "RpcWaitTable.h"


#endif
