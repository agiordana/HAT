/* 
 * File:   agent.h
 * Author: Davide Monfrecola<davide.monfrecola@gmail.com>
 *
 * Created on March 1, 2013, 11:09 AM
 */

#ifndef ARCHIVETHREAD_H
#define	ARCHIVETHREAD_H

#include "agentlib.h"
#include "ObserverThread.h"

class ArchiveThread: public ObserverThread {
public:
    ArchiveThread(std::string name);
    
    ArchiveThread(const ArchiveThread& orig);
    
    virtual ~ArchiveThread();
    
    /**
     * Carica eventi da osservare e fa partire le thread di servizio
     * 
     * @see ArchiveThread::start_service_thread()
     * @return true
     */
    void start_service_threads();
    
private:
    
    /**
     * @var std::string
     * @brief nome observer
     */
    
	std::string workingdir;

	std::string horus_status;
    
    std::string day;
    
    /**
     * @var Params
     * @brief Istanza della classe Params che contiene tutti i parametri di 
     *        configurazione letti dal file XML
     */
    MParams* conf;
    
    /**
     * Metodo che viene eseguito da una thread che contiene il codice per la 
     * gestione dei messaggi ricevuti
     * 
     * @param puntatore all'istanza dell'oggetto
     */
    void do_work(ArchiveThread* obj);
    
    /**
     * Controlla se esistono le directory richieste dall' archivio e se non le trova le crea.
     * Se non riesce a crearle termina l'esecuzione del programma. Se l'insieme delle telecamere
     * non è vuoto crea una directory, nella working directory, per ogni telecamera presente
     * 
     * @param working directory (stringa vuota di default)
     * @return 1 se tutto ok
     * @see hsrv::isdir(), mkdir()
     */
    bool install(std::string wd = "");

    /**
     * Controlla se esistono le directory per il giorno corrente e se non le trova le crea.
     * Crea una directory per ogni telecamera presente nel sistema.
     * 
     * @param puntatore ad una istanza di Params
     * @param working directory dell'archivio (stringa vuota di default)
     * @return 1 se tutto ok
     * @see install(), hsrv::isdir(), mkdir()
     */
	bool installday();
    
    /**
     * Purge delle directory che contengono i video. Elimina tutti i file e le 
     * directory nella working dir in base alla loro data di creazione. Per ogni archivio viene
     * impostato per quanti giorni i video devono essere tenuti su disco. Recupera questa 
     * informazione, recupera la lista delle directory da eliminare e procede
     * @param puntatore ad una istanza di Attribute che contiene la lista delle directory
              presenti
     * @return 1 se tutto ok
     * @see hsrv::a2unsigned, getCameralist()
     */
	bool purge();
    
    /**
     * Update delle directory utilizzate per la memorizzazione dei video. Recupera il giorno 
     * corrente e richiama installday()
     * 
     * @param fl puntatore ad una istanza di Attribute che contiene la lista delle direcotry
            presenti
     * @return 1 se tutto ok
     * @see installday(), hsrv::getday(), purge()
     */
	bool update();
    
    double getTimeInSecond(std::string&);
    std::string mkDayTime(double);
    std::string mkDate(std::string);
    bool mkHorusStatusPage(std::string&, std::string&, std::string&);
    MMessage* page;
    double bias;
    bool writelog(std::string, MMessage);
    MParams* trace;
    bool isToTrace(std::string);
    std::string mkItem(MMessage&);
};

#endif	/* ARCHIVETHREAD_H */

