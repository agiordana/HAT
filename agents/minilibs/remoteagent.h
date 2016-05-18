/*
 *  remoteagent.h
 *  phoneagent
 *
 *  Created by Attilio Giordana on 11/20/11.
 *  Copyright 2011 Università del Piemonte Orientale. All rights reserved.
 *
 */

#ifndef phoneagent_remoteagent_h
#define phoneagent_remoteagent_h

/** @brief Informazioni sull'allarme
 
 Interfaccia per inviare comandi ad un agente esterno.
 @author Attilio Giordana
 @date Novembre 2011
 */
class RemoteAgent: public IPlocation, public Params {
public:
    /**
     @var std::string
     @brief Nome dell'agente remoto
     */
	std::string name;
    /**
     Costruttore della classe, crea un'istanza di Params passandogli il nome dell'agente, 
     un'istanza di IPlocation e richiama il metodo init passandogli il nome dell'agente remoto
     @param n nome dell'agente remoto
     @see init()
     */
	RemoteAgent(std::string n): Params(n), IPlocation() { init(n); };
	RemoteAgent(std::string dir, std::string n): Params(dir, n), IPlocation() { init(n); };
/*	name=n; post="/"; host="127.0.0.1";
		useragent="Mozilla/4.0"; pragma="no-cache";
		contenttype=" ";
	};
 */
    /**
     Carica i parametri dell'agente remoto dal file di configurazione in formato XML
     @return 1 se tutto ok
     @see Params::load(), Params::get()
     */
	int load();
	/**
     Prepara un messaggio in formato XML o HTML da inviare al client estraendo le informazioni 
     dall'istanza di Message che viene passata come parametro. 
     Il formato viene deciso in base al valore di language.
     @param m istanza di Message contenente le informazioni sul messaggio da inviare
     @return 1 se tutto ok, 0 se il nome dell'agente è "none", -1 se la chiamata al combinatore
             telefonico va in timeout
     @see Message::xmlEncode(), Message::htmlPostEncode(), IPlocation::connectSetup(),
          IPlocation::dataAvailable(), HorusLogger::alert()
     */
    int send(Message);
    
    /**
     Prepara un messaggio in formato XML o HTML da inviare al client estraendo le informazioni 
     dall'istanza di Message che viene passata come parametro. 
     Il formato viene deciso in base al valore di language.
     @param m istanza di Message contenente le informazioni sul messaggio da inviare
     @return 1 se tutto ok, 0 se il nome dell'agente è "none", -1 se la chiamata al combinatore
             telefonico va in timeout
     @see Message::xmlEncode(), Message::htmlPostEncode(), IPlocation::connectSetup(),
          IPlocation::dataAvailable(), HorusLogger::alert()
     */
    int send(MMessage&);
    
    /**
     Prepara un messaggio in formato XML o HTML da inviare al client estraendo le informazioni 
     dall'istanza di Message che viene passata come parametro. 
     Il formato viene deciso in base al valore di language.
     @param m istanza di Message contenente le informazioni sul messaggio da inviare
     @return 1 se tutto ok, 0 se il nome dell'agente è "none", -1 se la chiamata al combinatore
     telefonico va in timeout
     @see Message::xmlEncode(), Message::htmlPostEncode(), IPlocation::connectSetup(),
     IPlocation::dataAvailable(), HorusLogger::alert()
     */
    int sendStringMessage(std::string);
    /**
     Reset informazioni (ancora da implementare)
     */
	int reset();
    /**
     Verifica se il remote agent è il destinatario di un dato messaggio
     @return true se il target passato come parametro è un stringa vuota o il nome dell'agente
             o il target impostato nel file di configurazione dell'agente. Ritorna false se 
             tutte e tre le condizioni sono false
    */
	bool isTarget(std::string tg) {if(tg=="" || tg == name || tg==get("target")) return true; return false; };
    /**
     Verifica se è attivo usando una get sull url: AYA
     */
    bool isAlive();
    
protected:
    /**
     Inizilizza le variabili che contengono le informazioni sull'agente remoto. Esse sono tutte
     contenute nelle variabili private della classe.
     @param n nome dell'agente remoto
     @return 1 se tutto ok
     @see Params::get()
     */
	int init(std::string&);
    /**
     @var std::string
     @brief path di destinazione
     */
	std::string post;
	/**
     @var std::string
     @brief host di destinazione
     */
    std::string host;
	/**
     @var std::string
     @brief useragent da utilizzare
     */
    std::string useragent;
	/**
     @var std::string
     @brief valore del campo pragma nel messaggio
     */
    std::string pragma;
	/**
     @var std::string
     @brief valore del campo Content-type del messaggio
     */
    std::string contenttype;
    
	bool messageComplete(Message&);
    
    bool messageComplete(MMessage&);
};

/** @brief Insieme di agenti remoti
 
 Insieme di agenti remoti (istanze di RemoteAgent). Gestisce una coda di messaggi (istanze di 
 Message) e il loro inoltro alle code dei vari RemoteAgent presenti.
 @author Attilio Giordana
 @date Novembre 2011
 */
class RemoteAgentSet : public setof<RemoteAgent> {
public:
    /**
     Inizializza l'insieme di RemoteAgent recuperando le informazioni per ognuno di essi da un file 
     XML. Esiste un file XML per ogni RemoteAgent presente nel sistema che si trovano nella 
     directory passata come parametro
     @param dir directory dove si trovano i file XML con le informazioni sui RemoteAgent
     @see RemoteAgent::RemoteAgent(std::string)
     */
    RemoteAgentSet(std::string);
    /**
     Estrae i messaggi dalla coda (una alla volta) e li invia al RemoteAgent specificato nel 
     messaggio
     @see MsgQueue<Message>::receive()
     */
    int exec();
    /**
     Inserisce un messaggio nella coda
     @see MsgQueue<Message>::send()
     */
	int send(Message& m){input.send(m); return 1;};
	
protected:
    /**
     @var MsgQueue<Message>
     @brief coda di messaggi diretti ai RemoteAgent
     */
	MsgQueue<Message> input;
};

#endif
