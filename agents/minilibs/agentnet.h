#ifndef agentnet_AgentNet_h
#define agentnet_AgentNet_h

//#include "agentlib.h"
//#include "ObserverTCP.h"
#include <openssl/rsa.h>

#define SEND_STATUS 1
#define RECV_STATUS 2
#define CHECK_STATUS 3

/**
@struct AgentInfo
@brief struttura che contiene i dati di un agente
*/
class AgentInfo: public MMessage {
public:
    time_t timestamp;
    std::string xmlformat;
    bool Print(){std::cout<<getString("agentname")<<", "<<getString("ip")<<", "<<getString("port")<<", "<<timestamp<<", "<<xmlformat<<std::endl; return true;};
    AgentInfo() {timestamp = 0;};
    AgentInfo(std::string a) {xmlformat = a; MMessage::xmlDecode(a); timestamp = 0;};
};

class NetSubjects {
public:
    static MParams importedSubjects;
    static MParams exportedSubjects;
    bool static initSubjects(std::string fname, MParams& subj, std::string ty="export");
    bool static clearSubjects(std::string ty);
    bool static print(std::string ty);
};
	
/**
@brief Housekeeping
Invia il proprio indirizzo IP al resto del sistema, riceve e memorizza l'indirizzo IP attuale degli altri agenti
@author Valeria Mussa
@date Maggio 2012
*/
class AgentNet {

private:
    static std::map<std::string, ObserverTCP*> tcp_observers;
    static int novelty;
    static MParams* netpar;
    static std::string mport;
    static std::string mgroup;
    static std::string public_key;
    static std::string private_key;
    static time_t last_subject_update;
	/*************************************************************
	******** parametri per il funzionamento dell'agente **********
	**************************************************************/
	
	/**
	@var int
	@brief tempo di validità in secondi dei dati nella tabella (default : 180 secondi)
	*/
	static int validityTime;
	/**
	@var int
	@brief tempo di attesa in secondi tra l'invio di un messaggio di stato e l'altro (default : 10 secondi)
	*/
	static int send_wait;
	/**
	@var int
	@brief tempo di attesa in secondi tra un controllo di validità e l'altro (default : 30 secondi)
	*/
	static int check_wait;	
	/**
	@var bool
	@brief vale TRUE se il debug è attivo, FALSE altrimenti (default : FALSE)
	*/
	static bool debug;
	/**
	@var std::map<std::string, AgentInfo>
	@brief tabella delle informazioni degli agenti
	*/
    static std::map<std::string, AgentInfo> foreign_agents;

    static std::string wsserver_ip;
    static std::string wsserver_port;
    static boost::mutex mutexclusion;
    
    static std::vector<unsigned> ports;
    
    static AgentInfo* myinfo;
    
    static std::string crypt;
    
    static bool send_agent_info;
    
    static std::vector<std::string> mysubjects;
    
    static  bool isIn(std::vector<std::string>&, std::string&);
    
    static std::vector<std::string> externsubjects;
    
	static bool MakePage();
    	
    /*************************************************************
    ****** dichiarazione dei thread (send, receive, check) *******
    **************************************************************/
    static std::vector<boost::thread*> service_thread;
    static boost::thread client;
    static boost::thread server;
    	/**
    	Il thread si occupa di inviare le informazioni sullo stato dell'agente al resto del sistema
    	@param n puntatore alla variabile che contiene il numero di secondi di attesa tra un invio del proprio stato e l'altro, castato a void*
    	@return NULL
    	@see retrieveInfo(), createMsg(), UDPClient
    	*/
	static void* send_status();
	/**
    	Il thread si occupa di ricevere le informazioni sullo stato del resto del sistema
    	@param n NULL
    	@return NULL
    	@see checkMsg(), update(), printMap(), UDPServer
    	*/
	static void* recv_status();
	/**
    	Il thread si occupa di controllare la validità delle informazioni degli agenti presenti nella mappa
    	@param n puntatore alla variabile che contiene il numero di secondi di attesa tra un controllo e l'altro, castato a void*
    	@return NULL
    	@see check()
    	*/
	static bool check_expired();
    
    static bool split(std::string&, std::string&, std::string&);



	/*************************************************************
	*********** metodi privati del thread check_status ***********
	**************************************************************/
	
    /**
	Controlla la validità dell'agente con il nome passato
	@param agentname nome dell'agente da controllare
	@return TRUE se l'agente è valido, FALSE altrimenti
	*/
    static bool check_agent_status(std::string agentname); //controlla la validita' dell'agente con il nome passato 




	/*************************************************************
	******************** metodi privati generici *****************
	**************************************************************/
	
	/**
	Recupera i dati associati all'agente con il nome passato
	@param agentname nome dell'agente di cui recuperare i dati
	@return struttura AgentInfo contenente i dati dell'agente, NULL se l'agente non esiste
	*/
	static AgentInfo *retrieveInfo(std::string agentname);
    static AgentInfo *retrieveInfo();
	/**
	Recupera l'indirizzo della macchina corrente
	@return stringa contenente l'indirizzo IP della macchina
	*/
	static std::string retrieve_my_ip();
	/**
	Controllo la correttezza dei campi del messaggio arrivato
	@param m messaggio da controllare, in formato XML
	@param recv_addr struttura contenente informazioni sulla macchina che ha inviato il messaggio
	@return TRUE se il messaggio è valido, FALSE in caso contrario
	*/	
	static bool checkMsg(MMessage m, struct sockaddr_in recv_addr);
	/**
	Aggiorno i dati dell'agente con nome agentname nella tabella
	@param agentname nome dell'agente di cui aggiornare i dati
	@param ipaddr nuovo indirizzo IP dell'agente
	@return TRUE
	*/
	static bool update(MMessage&);
        /** 
        Aggiorna ip e porta del wsserver rilevato osservando i messaggi UDP;
        */
	static bool wsServerUpdate(MMessage&);
	/**
	Crea una stringa contenente il messaggio XML con le proprie informazioni di stato
	@param agentname nome dell'agente che invia il messaggio
	@param ipaddr indirizzo IP dell'agente che invia il messaggio
	@param port numero di porta dell'agente che invia il messaggio
	@return stringa contenente il contenuto XML del messaggio
	@see Message
	*/
	static std::string createMsg(std::string agentname, std::string ipaddr, int port); //crea il messaggio xml contenente il nome dell'agente, l'indirizzo ip e la porta

    static std::string retrieve_my_port();
    
    static bool mkTcpObserverXML(MMessage&);
    
    static bool activateObserverTCP(std::string&);
    
    static bool disactivateObserverTCP(std::string&);
    
    static bool evFilter(std::vector<std::string>&);
    
    static bool addSubjects();
    
    static bool isStar(std::string&);

public:

	
	/**
	Costruttore: segnala che i thread non sono ancora partiti
	*/
//    AgentNet(std::string n){init(n);}; 

	/**
	Imposta tempo di validità dei dati all'interno della tabella (default: 180 secondi)
	@param time tempo di validità in secondi
	*/
	static void setValidityTime(int time) { validityTime=time; };	
	/**
	Attiva/disattiva modalità debug
	@param deb se vale TRUE, attiva debug, se vale FALSE, disattiva
	@return TRUE se il debug è attivato, FALSE altrimenti
	*/	
	static bool setDebug(bool deb) { debug=deb; return deb; };	    		
	/**
	Inizializza i dati all'interno della tabella e i tempi di attesa :
	- tempo di validità dei dati : 180 secondi
	- tempo di attesa tra un controllo di validità e l'altro : 30 secondi
	- tempo di attesa tra un invio del proprio stato e l'altro : 10 secondi
	@see Param
	*/
    static bool init(std::string);
    	/**
    	Fa partire il thread t, con tempo di attesa di default
    	@param t tipo di thread (SEND_STATUS, RECV_STATUS, CHECK_STATUS)
    	@return TRUE se l'operazione è andata a buon fine, FALSE altrimenti
    	@see send_status(), recv_status(), check_status()
    	*/
   static bool start();
	/**
	Stampa le informazioni sull'agente stesso e sugli agenti presenti nella tabella
	@return TRUE
	@see retrieveInfo()
	*/
    static bool printMap();
    /**
    Ritorna l'indirizzo ip attuale dell'agente con nome agentname
    @param agentname nome dell'agente
    @return stringa contenente l'indirizzo IP dell'agente
    @see check_agent_status()
    */
    static std::string getAgentIp(std::string agentname);

    static std::string getAgentIp();
    /**
     Ritorna la porta attuale dell'agente con nome agentname
     @param agentname nome dell'agente
     @return stringa contenente l'indirizzo IP dell'agente
     @see check_agent_status()
     */
    static std::string getAgentPort(std::string agentname);
    static std::string getAgentPort();
    static double getSendWait();
    
    static bool SubjectsUpdate();
    static bool SubjectDateUpdate();
    static bool SubjectRegister();

    /* Returns the ip:port of the WSServer, if it exists */
    static std::string getWSServer();
    /* Returns the ip:port of the agent that handle a specific command (cmd) */
    static std::string getAgentOf(std::string);
    static bool setNovelty(int n=2);
    static bool testNovelty();
    
};

#endif
