//
//  iplocation.h
//  phoneagent
//
//  Created by Davide Monfrecola on 11/15/11.
//  Copyright 2011 Università del Piemonte Orientale. All rights reserved.
//

#ifndef phoneagent_iplocation_h
#define phoneagent_iplocation_h

/** @brief Gestione connessioni
 
 Metodi e attributi utilizzati da tutte le classi per la gestione delle connessioni e delle
 informazioni ad esse correlate
 @author Attilio Giordana
 @date Novembre 2011
 */
class IPlocation: public Net {
public:
    /**
     @var std::string
     @brief numero ip 
     */
    std::string targetip;
	/**
     @var std::string
     @brief numero porta 
     */
    std::string targetport;
	/**
     @var std::string
     @deprecated
     */
    std::string sources;
	/**
     @var sockaddr_in
     @brief 
     */
    struct sockaddr_in addr;
	/**
     @var int
     @brief socket descriptor
     */
    int sd;
    
    /**
     Costruttore della classe, inizializza i valori dell'indirizzo IP, della porta utilizzata, 
     imposta il socket descriptor con valore negativo e il codice di errore a 0.
     @param ip indirizzo IP
     @param port porta di ascolto
     */
	IPlocation(std::string ip, std::string port) {targetip = ip; targetport = port; sd = -1; myerr=0;};
	/**
     Costruttore della classe, inizializza i valori dell'indirizzo IP e della porta ricavandolo
     dalla stringa passata come parametro. Inizializza anche il valore del socket descriptor, del
     codice di errore e delle sorgenti.
     @param addr indirizzo IP, porta ???
     @deprecated
     */
    IPlocation(std::string addr) {targetip = getip(addr); targetport = getport(addr); sources = getsources(addr); sd = -1; myerr=0;};
	/**
     Costruttore della classe, inizializza a stringa vuota l'indirizzo IP e la porta. Il socket 
     descriptor e il codice di errore vengono inizializzati a 0 e -1.
     @param indirizzo IP
     @param porta di ascolto
     */
    IPlocation(){targetip = ""; targetport=""; sd = -1; myerr=0;};
	/**
     Stampa sullo standard output il valore dell'indirizzo IP, della porta e del socket descriptor
     @return 1 se tutto ok
     */
    int print() { std::cout<<"    ip-address = "<<targetip<<":"<<targetport<< ", socket = " <<sd<<std::endl; return 1;};
	/**
     Aggiunge alla variabile di tipo ofstream passata come parametro i valori dell'indirizzo IP, 
     della porta e del socket descriptor
     @param out ofstream su cui eseguirela scrittura
	 @return 1 se tutto ok
     */
    int log(std::ofstream& out) { out<<"    ip-address = "<<targetip<<":"<<targetport<< ", socket = " <<sd<<std::endl; return 1;};
	/**
     Imposta i valori dei campi contenuti nella struttura addr
     @return 1 se tutto ok
     */
    int setaddr();
	/**
     Inizializza le informazioni su indirizzo IP, porta e socket descriptor
     @param addr indirizzo utilizzato per l'inizializzazione
	 @return 1 se tutto ok
     */
    int init(std::string addr) {targetip = getip(addr); targetport = getport(addr); sd = -1; return 1;};
	/**
     Setup della connessione. Richiama la funzione socket impostando il valore del socket 
     descriptor e richiama la connect. Se il valore del sd > -1 chiude prima la connessione già 
     esistente
     @return il valore del socket descriptor se tutto va a buon fine, il codice di errore
     (memorizzato nella variabile myerr) altrimenti
     */
    int connectSetup();
	/**
     Chiude la connessione attiva
     @return 1 se tutto ok
     */
    int connectClose();
	/**
     Invia un comando utilizzando la connessione attiva e attende una risposta
     @param buffer buffer che contiene il messaggio da inviare
     @return 1 se tutto ok, il codice di errore (memorizzato nella variabile myerr) altrimenti
     */
    int sendCmd(unsigned char* buffer);
	/**
     Invia un comando utilizzando la connessione attiva e attende una risposta che sia di almeno 
     bsize caratteri
     @param buffer buffer dal quale leggere (per la write) e scrivere (per la read)
     @param bsize dimensione minima del buffer
     @return 1 se tutto ok
     */
    int sendCmd(char* buffer, unsigned bsize=MAXB);
    
    int sendCmd(std::string& msg);
	/**
     Controlla lo stato della connessione
     @return 1 se il socket descriptor è settato e se la variabile che contiene il codice di errore
               è settata o è uguale a zero, 0 altrimenti
     */
    int connectON();
	/**
     Imposta le variabili necessarie per eseguire una bind su uno specifico socket descriptor e si
     mette in attesa di connessioni su quest'ultimo 
     @return 1 se tutto ok, il codice di errore (memorizzato nella variabile myerr) altrimenti
     */
    int rvportSetup();
    /**
     Ricerca se è presente la sorgente passata come parametro all'interno della variabile sources
     @param s sorgente da ricercare
     @return 1 se è presente, 0 altrimenti
     @deprecated
     */
	int issource(std::string&);
	/**
     Sfrutta le proprietà della select per utilizzare il timeout per le letture da rete
     @param fd file descriptor
     @return 1 se è presente, 0 altrimenti
     */
    int dataAvailable(int);
    
private:
    /** 
     @var int 
     @brief codice dell'errore. Se non se ne è verificato nessuno viene impostato a 0
     */
	int myerr;
    
};


#endif
