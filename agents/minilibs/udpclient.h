#ifndef agentnet_UDPClient_h
#define agentnet_UDPClient_h

//#include "agentlib.h"

/**
@brief Client UDP
client UDP per invio di messaggi cifrati o in chiaro ad un determinato indirizzo IP/numero di porta
@author Valeria Mussa
@date Maggio 2012
*/
class UDPClient {

	private:
		
		/*************************************************************
		******************** attributi della classe ******************
		**************************************************************/
		
		/**
		@var int
		@brief socket descriptor
		*/
		int sockfd;
		
		/**
		@var std::string
		@brief stringa contenente l'indirizzo IP a cui inviare messaggi
		       (nell'implementazione, indirizzo IP di multicast)
		*/
		std::string ip;

		/*
		@var std::string
                @brief stringa contenente la porta a cui inviare messaggi
                       (nell'implementazione, porta di multicast del gruppo di agenti)
                */

		std::string mport;
		
		/**
		@var struct sockaddr_in
		@brief parametri di spedizione
		*/
		struct sockaddr_in sendaddr;
		
		/**
		@enum connection_type
		@brief tipo di comunicazione effettuata (ENCRYPT, PLAINTEXT)
		*/
		/**
		@var enum connection_type
		@brief contiene il tipo di comunicazione effettuata  (ENCRYPT=cifrata, PLAINTEXT=in chiaro). 
		       Impostata automaticamente in base al valore della chiave di cifratura.
		*/
		enum connection_type {ENCRYPT, PLAINTEXT} type;
		
		/**
		@var std::string
		@brief se la comunicazione è cifrata, contiene il path del file .pem che contiene la chiave di cifratura;
		       in caso contrario, la stringa è vuota
		*/
		std::string key;
		
		/**
		@var int
		@brief contiene il codice di errore
		*/
		int err;
		


		/*************************************************************
		*********************** metodi privati ***********************
		**************************************************************/
		
		/**
		Imposta i parametri di connessione in base ai valori presenti negli attributi ip e nel parametro port.
		@return TRUE se va a buon fine, FALSE altrimenti
		*/		
		bool setaddr(int port);
		
	public:
	
		/*************************************************************
		************************* costruttori ************************
		**************************************************************/
		
				
		/**
		Costruttore: imposta sockfd=-1, err=0, key="", ip al parametro passato
		La comunicazione avviene in chiaro
		@param ipstr stringa contenente l'indirizzo IP a cui inviare i messaggi
		*/
		UDPClient(std::string ipstr, std::string port) { 
			sockfd=-1; err=0; ip=ipstr; mport=port; key=""; type=PLAINTEXT; 
		};
		
		
		/*************************************************************
		********************** metodi pubblici ***********************
		**************************************************************/
		
		/**
		Crea una nuova socket e inserisce il descrittore in sockfd
		Fallisce se è già aperta una connessione o se non si può creare una nuova socket
		@return TRUE se va a buon fine, FALSE altrimenti
		*/
    bool start(std::string);
    
  //  bool start();
		
		/**
		Invia un messaggio in base ai parametri impostati
		@param port intero contenente il numero di porta a cui inviare i messaggi
		@param m messaggio da inviare
		@return numero di caratteri inviati, -1 in caso di errore
		@see setddr()
		*/
		int sendMsg(std::string m);
		
		/**
		Chiude la comunicazione
		@return TRUE
		*/
		bool stop();

		/**
		Costruisce un messaggio di errore
		@return stringa contenente il messaggio di errore
		*/
		std::string getErr();
		
		/**
		Ritorna il codice dell'errore
		@return errno per errori di sistema, -1 per errori generici, WRONG_KEY per errore di cifratura se la comunicazione è cifrata
		*/
		int getErrno();
		
};

#endif

