//
//  hsrv.h
//  phoneagent
//
//  Created by Davide Monfrecola on 11/15/11.
//  Copyright 2011 Università del Piemonte Orientale. All rights reserved.
//


#ifndef phoneagent_hsrv_h
#define phoneagent_hsrv_h

#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/condition.hpp>

#define CONFENABLED "/home/horus/CONFIGURE"
#define CFWINDOW 180

class TiXmlNode;
class TiXmlElement;
class HorusLogger;
class ArchiveManager;
class RoutingMap;
class RpcWaitTable;

/** @brief Classe di servizio "Horus service"
 
 Metodi e attributi utilizzati da tutte le classi per la gestione delle informazioni di servizio
 @author Attilio Giordana
 @date Novembre 2011
 */
class hsrv {
public:
    /**
     Controlla se la stringa passata corrisponde ad un file
     @param fname nome del file
     @param type tipo
     @return 1 se è un file, 0 altrimenti
     @deprecated utilizzare FileManager::isFile()
     */
	static int isfile(std::string&, std::string);
    /**
     Controlla se la stringa passata corrisponde ad un file
     @param fname nome del file
     @return 1 se è un file, 0 altrimenti
     @deprecated utilizzare FileManager::isFile()
     */
    static int isfile(std::string&);
    /**
     Controlla se la stringa è in formato XML valido
     @param s stringa XML da controllare
     @return 1 se è una stringa valida, 0 altrimenti
     */
	static int isXmlstring(std::string&);
    /**
     Controlla se la stringa passata corrisponde ad una directory
     @param path pathname da controllare
     @return 1 se è una directory, 0 altrimenti
     @deprecated utilizzare FileManager::isDir()
     */
	static int isdir(std::string&);
    /**
     Esegue un dump sullo standard output di un nodo XML
     @param pParent puntatore al nodo XML
     @param indent numero di spazi da usare per l'indentazione
     @see dump_attribs_to_stdout(), getIndentAlt()
     */
	static void dump_to_stdout( TiXmlNode*, unsigned int);
    /**
     Esegue un dump degli attributi del nodo XML sullo standard output
     @param pElement puntatore al nodo XML
     @param indent numero di spazi da usare per l'indentazione
     @return numero di attributi, 0 se il nodo non esiste
     @see getIndent()
     */
	static int dump_attribs_to_stdout(TiXmlElement*, unsigned int);
    /**
     ??
     @param numIndents ?? 
     @return puntatore a caratteri
     */
	static const char *getIndentAlt(unsigned int);
    /**
     ??
     @param numIndent ??
     @return puntatore a caratteri
     */
	static const char *getIndent(unsigned int);
    /**
     Recupera il valore di un attributo da un elemento XML in base al suo nome
     @param pElement puntatore all'elemento XML di tipo TiXmlElement
     @param name stringa che identifica il nome dell'attributo
     @return valore dell'attributo
     */
	static std::string getAttrbyName(TiXmlElement*, std::string);
    /**
     Recupera il valore di un parametro da un nodo XML in base al suo nome
     @param prg puntatore al nodo XML di tipo TiXmlNode
     @param name stringa che identifica il nome del parametro
     @return valore del parametro
     @see getAttrbyName()
     */
	static std::string getParambyName(TiXmlNode*, std::string);
    /**
     Stampa sullo standard output la stringa memorizzata in una std::ostringstream
     @return 1 se tutto ok
     */
	static int print();
    /**
     Stampa una std::ostringstream su file
     @param outfile file sul quale stampare la std::ostringstream
     @see printstring()
     @return 1 se tutto ok
     */
	static int print(std::ofstream&);
    /**
     Stampa la stringa "OK" sulla std::ostringstream passata come parametro
     @param out std::ostringstream
     @see printstring()
     @return 1 se tutto ok
     */
	static int printstring(std::ostringstream&);
	/**
	 Fornisce il tempo corrente approssimato al micro-secondo
	 @return double contenete il valore attuale del tempo
	 */
	static double gettime(); 
    /**
     Recupera le informazioni sul tempo corrente e le inserisce in un contenitore di tipo
     vector
     @param d numero di giorni da utilizzare come offset
     @return vettore di interi contenente le informazioni sulla data e l'ora corrente
     */
	static std::vector<int> CurrentTime(int d=0);
    /**
     Recupera la parte finale di un path
     @param path path da analizzare 
     @return vettore di interi contenente le informazioni sulla data e l'ora corrente
     @deprecated utilizzare FileManager::getTail()
     */
	static std::string tailof(std::string&);
    /**
     Converte una stringa contenente un orario nel formato "hh:mm:ss" o "hh.mm.ss" in secondi
     @param s stringa contenente l'orario nei formati specificati 
     @return intero corrispondente al numero di secondi
     */
	static int string2second(std::string&);
    /**
     Converte un orario in secondi
     @param m vettore di interi formato da tre elementi: ore, minuti, secondi 
     @return intero corrispondente al numero di secondi
     */
	static int daysecond(std::vector<int>&);
    /**
     Controlla se l'estensione del file passato corrisponde all'estensione passata come secondo
     parametro
     @param fname file sul quale eseguire il controllo
     @param ext estensione su cui fare il matching
     @return 1 se l'estensione corrisponde, 0 altrimenti
     */
	static int matchfile(std::string, std::string);
    /**
     Recupera tutti i file all'interno di una cartella che hanno una specifica estensione
     @param dir path della directory
     @param ext estensione dei file da elencare 
     @return vettore di stringhe contenente i nomi dei file recuperati
     */
	static std::vector<std::string> getlistof(std::string, std::string);
    /**
     Recupera le informazioni sull'ora attuale e le memorizza in una stringa
     @return data attuale nel formato "hh:mm:ss"
     */
	static std::string getasciitimeday();
    /**
     Recupera le informazioni complete sulla data attuale e le memorizza in una stringa
     @return data attuale completa con informazione su giorno della settimana, data e ora
     */
	static std::string getasciitimefull();
	/**
     Recupera le informazioni complete sulla data attuale e le memorizza in una stringa
     @return data attuale completa con informazione su giorno della settimana, data e ora, in forma compatta
     */	
	static std::string getasciitimecompact();
    /**
     Recupera le informazioni sulla data attuale nel formato DATE di MySQL e le memorizza in
     una stringa
     @return stringa che rappresenta una data nel formato aaaa_mm_dd nella quale
     *       devono essere sotituiti i caratteri che separano le cifre con il 
     *       carattere -
     */
	static std::string mySQLdate(std::string d="");
    /**
     Sostituzione dell'occorrenza di una stringa all'interno di una stringa data
     @param s stringa nel quale cercare l'occorrenza della stringa passata come secondo parametro
     @param dst sottostringa da ricercare all'interno della stringa passata come primo parametro
     @param src stringa da sostituire all'occorrenza trovata nella stringa passata come primo
            parametro
     @return 1 se tutto ok, 0 altrimenti
     */
    static int strReplace(std::string&, std::string, std::string);

    static std::string filterChar(std::string);
    /**
     Upcase di tutti i caratteri della stringa
     @param a stringa sulla quale effettuare l'upcase
     @return 1 se tutto ok
     */
	static int upcase(std::string&);
    /**
     Converte un double in stringa
     @param n double da convertire
     @return double convertito in stringa
     */
	static std::string double2a(double);
    /**
     Converte un long in stringa
     @param n long da convertire
     @return long convertito in stringa
     */
	static std::string long2a(long);
    /**
     Converte un intero in stringa
     @param n intero da convertire
     @return intero convertito in stringa
     */
	static std::string int2a(int);
    /**
     Converte un unsigned in stringa
     @param n unsigned da convertire
     @return unsigned convertito in stringa
     */
	static std::string unsigned2a(unsigned); 
    /**
     Converte una stringa in double
     @param n stringa da convertire
     @return stringa convertita in double
     */
	static double a2double(std::string&);
    /**
     Converte una stringa in long
     @param n stringa da convertire
     @return stringa convertita in long
     */
	static long a2long(std::string&);
    /**
     Converte una stringa in intero
     @param n stringa da convertire
     @return stringa convertita in intero
     */
	static int a2int(std::string&);
    /**
     Converte una stringa in unsigned
     @param n stringa da convertire
     @return stringa convertita in unsigned
     */
	static unsigned a2unsigned(std::string&);
    /**
     Legge un file memorizzato su disco
     @param name nome del file
     @return stringa il contenuto del file se l'operazione è andata a buon fine, una vuota 
             altrimenti
     */
	static std::string readFile(std::string&);
	
	/**
	Verifica se la string signature è contenuta  nella stringa s
	@param s stringa oggetto, signature stringa da cercare
	@return 1 se successo 0 se fallimento
	*/
	static int isInString(std::string& s, std::string signature);
	/**
	L'argomento di ingresso è un pathname.
	Ritorna la "tail", cioè il nome del file
	@ param pathhname
	@ return il nome local del file
	*/
	static std::string getTail(std::string);
    
    /**
     * Splitta la stringa in base al carattere passato come parametro
     * 
     * @param string stringa da splittare 
     * @param separator carattere separatore utilizzato per lo split della stringa
     * @param limit limite massimo di split (parole da ritornare)  
     * @return 
     */
    static std::vector<std::string> split(std::string, char, int = '0');
	
	static int charExplode(std::string& s) {for(unsigned i=0; i<s.size(); i++) std::cout<<s[i]<<'-'; std::cout<<std::endl; return 1;};
	static int charPrintf(const char* b) {printf("%s\n", b); return 1;};
	/**
     Recupera le informazioni sulla data attuale
     @return stringa con la data nel formato "aaaa_mm_gg"
     */
	static std::string getday(time_t delta=0);
	/**
     Log di un messaggio su file
     @deprecated utilizzare HorusLogger
     @param m stringa da loggare
     @return 1 se tutto ok
    */
	static int logMessage(std::string);
    /**
     Estrae dalla stringa il nome del binario dell'agente
     @param agentname path binario agente
     @return stringa con il nome del binario dell'agente se lo trova, vuota altrimenti
     */
    static std::string extractBin(std::string agentname);
    /**
     Esegue un comando utilizzando la popen e ritorna l'output
     @param cmd comando da lanciare
     @return stringa contenente l'output ritornato dal comando
     */
	static std::string cmdExec(std::string);
	/**
	 Esegue la ricerca del processo il cui nome contiene la stringa in argomento
	 @param comando (anche parziale) di attivazione del processo
	 @return il pid del processo
	 */
	static pid_t getprocessid(std::string&);
	
	static int convertSpecialChar(std::string&);
    
    /**
     * Controlla se il file ha l'estensione richiesta
     * 
     * @param fname nome del file
     * @param ext estensione del file
     * @return true se ha l'estensione richiesta, false altrimenti
     */
    static int matchextension(std::string fname, std::string ext);
    
    /**
     * Crea una lista dei file contenuti nella directory specificata che hanno 
     * l'estensione specificata
     * 
     * @param dir nome directory
     * @param ext estensione dei file
     * @return lista dei file contenuti nella directory che hanno l'estensione
     *         specificata. Se non ci sono file ritorna NULL.
     */
    static std::vector<std::string>* getdirlist(std::string dir, std::string ext);
    
    /**
     * Salva su file il contenuto passato come parametro
     * 
     * @param ty tipologia del file che identifica la directory dove andare
     *           a salvare il file (Es: phones, programs)
     * @param name nome del file
     * @param body contenuto del file
     * @return true se l'operazione va a buon fine, false altrimenti
     */
    static bool saveFile(std::string& ty, std::string& name, std::string& body);
    
    static bool deleteFile(std::string& ty, std::string& name);
    /**
      @param path = pathname relativo alla homedir
      @param items = la lista di items da memorizzare
      @return true se l'operazione va a buon fine, false altrimenti
    */
    static bool saveItemList(std::string name, std::string path, std::vector<std::string>& items);   
    static bool saveItemList(std::string name, std::string path, std::vector<std::pair<std::string, std::string> >& items);
    /**
     @var static std::string
     @brief Home directory dell'agente
     */
    static std::string homedir;
    /**
     @var static std::string
     @brief Directory dei file di configurazione dell'agente
     */
	static std::string configdir;
    /**
     @var static std::string
     @brief Directory con i file binari dell'agente
     */
	static std::string bindir;
    /**
     @var static std::string
     @brief file con la descrizione della confiurazione della rete di agenti
     */
    static std::string agentnet;
    /**
     @var static std::string
     @brief Directory dei file di log
     */
	static std::string logfile;
    /**
     @var static std::string
     @brief Tipo dell'agente (definisce la funzionalità che implementa)
     */
    static std::string agenttype;
	/**
     @var static std::string
     @brief Nome dell'agente
     */
    static std::string agentname;
    /**
     @var static std::string
     @brief modalita` di comportamento dell'agente
     */
    static std::string behaviour;	
    static std::string configurator;	
    /**
     @var static HorusLogger*
     @brief Puntatore ad una istanza del logger
     */
    static HorusLogger* logger;

    /**
     @var static ArchiveManager*
     @brief Gestore dell'archivio
     */
    static ArchiveManager* archive;

    /**
     @var static RoutingMap*
     @brief Rest Router
    */
    static RoutingMap* router;

    /**
     @var static RpcWaitTable*
     @brief Rpc wait for answer
    */
    static RpcWaitTable* rpctab;

    /**
     carica i parametri globali dal file "$configdir/global.xml"
     @return  true se ha successo flse se non trova il file
     */
    static bool load(std::string ar0, std::string ar1);
    /**
     agente responsabile delle web services
     */
    /**
     pagina dinamica per l'accesso allo stato dell'agente (I/O)
     */
    static std::string xmlIndex();
    static std::string xmlIndex(std::string);
    static std::string jsonIndex();
    static std::string jsonIndex(std::string);
    static bool publish(std::string, std::string);
    static bool indexClear();
    static bool indexClear(std::string);
    static std::string getDynamicPage(std::string);
    static boost::mutex mutexclusion;
    static bool checkConfigureEnabled();
    static bool rpcNotify(std::string, std::string, std::string);
    static int debug;
private:
    static std::map<std::string,std::string> index;
    static std::string getHostName();
    static std::string jsonItem(std::string&);
};


#endif
