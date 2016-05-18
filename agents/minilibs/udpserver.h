#ifndef agentnet_UDPServer_h
#define agentnet_UDPServer_h

//#include "agentlib.h"

#include <poll.h>

/**
@brief Server UDP
Server UDP per la ricezione di messaggi, cifrati o in chiaro, in multicast su più numeri di porta
@author Valeria Mussa
@date Maggio 2012
*/
class UDPServer {

	private:
	
		/*************************************************************
		******************** attributi della classe ******************
		**************************************************************/
		
		/**
		@var std::string
		@brief indirizzo IP da cui si aspettano risposte
		*/
		std::string mip;
		
		/**
		@var int
		@breif numero di porta da cui ricevere le risposte
		*/
		std::string port;
		
		/**
		@var struct sockaddr_in
		@brief contiene i parametri di connessione
		*/
		struct sockaddr_in addr;
		
		/**
		@var int
		@brief socket descriptor del server
		*/
		int sockfd;
		
		/**
		@var std::string
		@brief se la comunicazione è cifrata, contiene il path del file .pem che contiene la chiave di cifratura;
		       in caso contrario, la stringa è vuota
		*/
		std::string key;
		
		/**
		@enum connection_type
		@brief tipo di comunicazione effettuata (ENCRYPT, PLAINTEXT)
		*/
		/**
		@var enum connection_type
		@brief contiene il tipo di comunicazione effettuata  (ENCRYPT=cifrata, PLAINTEXT=in chiaro). 
		       Impostata automaticamente in base al valore della chiave di cifratura.
		*/
		enum connection_type { ENCRYPT, PLAINTEXT } type;
		
		/**
		@var int
		@brief se si è verificato un errore, contiene il codice di errore
		*/
		int err;	
		
		/**
		@var struct sockaddr_in
		@brief contiene i parametri della risposta ricevuta
		*/		
		struct sockaddr_in last_reply;
		
		
		/*************************************************************
		*********************** metodi privati ***********************
		**************************************************************/
		
		/**
		Imposta i parametri di connessione in base ai valori presenti negli attributi mip e port
		@return TRUE
		*/
		bool setaddr();		
		
		/**
		Apre una nuova socket
		@return TRUE in caso di successo, FALSE altrimenti
		*/
		bool open_socket();
		
		/**
		Esegue il bind() della connessione
		@return TRUE se va a buon fine, FALSE altrimenti
		@see setaddr()
		*/
		bool do_bind();
		
		/**
		Imposta i parametri di multicast
		@return TRUE se va a buon fine, FALSE altrimenti
		*/	
		bool setMulticast();
		/**
		Imposta la porta di recesione multicast come "reuse"
                @return TRUE se va a buon fine, FALSE altrimenti
                */
		bool setPortReuse();
	
	public:

		/*************************************************************
		************************* costruttori ************************
		**************************************************************/
		
		/**
		Costruttore: imposta err=0, key="", mip e port ai parametri passati
		La comunicazione avviene in chiaro
		@param ipstr stringa contenente l'indirizzo IP di connessione
		@param iport intero contenente il numero di porta della connessione
		*/
		UDPServer(std::string ipstr, std::string mport) {
			mip=ipstr; err=0; port=mport; type=PLAINTEXT; key="";sockfd=0;
		};
		
		
		/*************************************************************
		********************** metodi pubblici ***********************
		**************************************************************/
		
		/**
		Crea connessione, esegue il bind e setta le impostazioni di multicast sulla porta del server
		@return TRUE se l'operazione è avvenuta con successo, FALSE altrimenti
		@see open_socket(), do_bind(), setMulticast()
		*/			
        bool start(std::string crypt);
		
		/**
		Memorizza nel parametro m il messaggio ricevuto
		@param m stringa in cui finirà il messaggio ricevuto
		@return numero di caratteri letti, -1 in caso di errore
		*/
		int recvMsg(std::string& m, int w);	
		
		/**
		Ritorna i parametri dell'agente che ha inviato il messaggio, presenti nell'attributo last_reply
		@return last_reply
		*/
		struct sockaddr_in getReplyAddress();
		
		/**
		Chiude la connessione se aperta
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
