/*
 *  remoteagent.h
 *  phoneagent
 *
 *  Created by Attilio Giordana on 11/20/11.
 *  Copyright 2011 Universit√† del Piemonte Orientale. All rights reserved.
 *
 */

#ifndef MREMOTEAGENT_H
#define MREMOTEAGENT_H

//#include "agentlib.h"

/** @brief Informazioni sull'allarme
 
 Interfaccia per inviare comandi ad un agente esterno.
 @author Attilio Giordana
 @date Novembre 2011
 */
class MRemoteAgent: public IPlocation, public MParams {
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
	MRemoteAgent(std::string n): MParams(n), IPlocation() { init(n); };
	MRemoteAgent(std::string dir, std::string n): MParams(dir, n), IPlocation() { init(n); };
/*	name=n; post="/"; host="127.0.0.1";
		useragent="Mozilla/4.0"; pragma="no-cache";
		contenttype=" ";
	};
 */
    /**
     Carica i parametri dell'agente remoto dal file di configurazione in formato XML
     @return 1 se tutto ok
     @see MParams::load(), MParams::get()
     */
	int load();
    
    /**
     Prepara un messaggio in formato XML o HTML da inviare al client estraendo le informazioni 
     dall'istanza di Message che viene passata come parametro. 
     Il formato viene deciso in base al valore di language.
     @param m istanza di Message contenente le informazioni sul messaggio da inviare
     @return 1 se tutto ok, 0 se il nome dell'agente √® "none", -1 se la chiamata al combinatore
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
     @return 1 se tutto ok, 0 se il nome dell'agente √® "none", -1 se la chiamata al combinatore
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
     Verifica se il remote agent √® il destinatario di un dato messaggio
     @return true se il target passato come parametro √® un stringa vuota o il nome dell'agente
             o il target impostato nel file di configurazione dell'agente. Ritorna false se 
             tutte e tre le condizioni sono false
    */
	bool isTarget(std::string tg) {if(tg=="" || tg == name || tg == get("target")) return true; return false; };
    /**
     Verifica se √® attivo usando una get sull url: AYA
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
    
    bool messageComplete(MMessage&);
};

#endif
