//
//  httpserver.h
//  phoneagent
//
//  Created by Davide Monfrecola on 11/15/11.
//  Copyright 2011 Università del Piemonte Orientale. All rights reserved.
//

#ifndef phoneagent_httpserver_h
#define phoneagent_httpserver_h

class MMessage;

/** @brief Classe server HTTP
 
 Utilizzata per creare istanze di server HTTP usate per gestire le richieste 
 effettute dai client
 @author Attilio Giordana
 @date Novembre 2011
 */
class HttpServer: public Net {
public:
	/**
	 Costruttore della classe. Crea una nuova istanza azzerando il valore della porta
	 e richiamando il metodo load() che carica i parametri dal file di configurazione
	 in formato XML
	 @see load() 
	 */
	HttpServer(){ port = ""; load(); };
    /**
	 Costruttore della classe. Crea una nuova istanza azzerando il valore della porta
	 e richiamando il metodo load() che carica i parametri dal file di configurazione
	 in formato XML
	 @see load() 
	 */
	HttpServer(std::string path){ port = ""; load(path); };
	/**
	 Riceve una richiesta HTTP in formato XML o HTML e prepara un'istanza di
	 Message contente le infomazioni su di essa. 
	 @return puntatore ad un'istanza di message con informazioni sulla richiesta ricevuta
	 */
	Message* getRequest();
    
    /**
	 Riceve una richiesta HTTP in formato XML o HTML e prepara un'istanza di
	 Message contente le infomazioni su di essa. 
	 @return puntatore ad un'istanza di message con informazioni sulla richiesta ricevuta
	 */
	MMessage* getRequestM();
    
	/**
	 Invia una risposta al client codificata in XML o HTML. La scelta viene fatta in base al
	 valore dell'attributo langiage presente nel messaggio di risposta. Il valore dell'attributo
	 viene inizializzato in base alla codifica utilizzata dal client per la richiesta
	 @param answer puntatore ad un'istanza di Message contenente le informazioni sulla risposta da
		inviare al client
	 @return numero di byte scritti sul socket
	 */
	int sendAnswer(Message*);
    
    /**
	 Invia una risposta al client codificata in XML o HTML. La scelta viene fatta in base al
	 valore dell'attributo langiage presente nel messaggio di risposta. Il valore dell'attributo
	 viene inizializzato in base alla codifica utilizzata dal client per la richiesta
	 @param answer puntatore ad un'istanza di MMessage contenente le informazioni sulla risposta da
		inviare al client
	 @return numero di byte scritti sul socket
	 */
	int sendAnswerM(MMessage*);
    
	/**
 	 Setup di una nuova connessione
	 @return 
	 @see IPLocation::rvportSetup()	 
	 */
	int setUp() {return(rvport.rvportSetup());};
	/**
	 Chiude una connessione attiva
	 @return 
	 @see IPLocation::connectionClose()	 
	 */
	int goDown() {return rvport.connectClose();};
	/**
	 Carica i parametri di configurazione del server leggendoli dal file di configurazione in 
	 formato XML
	 @return 1 se tutto ok
	 @see xmlLoad()	 
	 */
	int load();
    /**
	 Carica i parametri di configurazione del server leggendoli dal file di configurazione in 
	 formato XML
     @param path path del file HttpServer.xml che contiene la configurazione del server HTTP
	 @return 1 se tutto ok
	 @see xmlLoad()	 
	 */
	int load(std::string& path);
	/**
	 Salva la configurazione del server in un file di configurazione in formato XML
	 @return 1 se tutto ok
	 @see xmlEncode()
	 */
	int xmlSave();
    
private:
	/**
	 @var std::string
	 @brief porta sulla quale rimane in ascolto il server. Viene recuperata dal file di configurazione
	  	dal costruttore della classe richiamando il metodo load()
	 */
    	std::string port;
	/**
	 Crea un stringa in formato XML che contiene i parametri del server 
	 @param xmlfile se > 0 inserisce l'intestazione XML
	 @return 1 se tutto ok
	 @see xmlEncode()
	 */
	int xmlLoad(std::string);
    /**
	 Crea un stringa in formato XML che contiene i parametri del server 
     @param path path del file di configurazione da leggere
	 @param xmlfile se > 0 inserisce l'intestazione XML
	 @return 1 se tutto ok
	 @see xmlEncode()
	 */
	int xmlLoad(std::string, std::string);
	/**
	 Crea un stringa in formato XML che contiene i parametri del server 
	 @param se > 0 inserisce l'intestazione XML
	 @return 1 se tutto ok
	 @see xmlEncode()
	 */
    	std::string xmlEncode(int md=0) {
        std::string r="";
		if(md>0) r="<?xml version=\"1.0\" ?>";
		r = r+"<class type=\"HttpServer\" port=\""+port+"\"/>"; return r;
    	};
	
	IPlocation rvport;
	int fd;
	struct  sockaddr_in client;
	int getBodyLength(std::string&);
    
};

#endif
