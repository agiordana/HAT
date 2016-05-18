/* 
 * File:   ObserverThread.h
 *
 */

#ifndef OBSERVERTHREAD_H
#define	OBSERVERTHREAD_H

#include "agentlib.h"
#include "Observer.h"

class ObserverThread: public Observer {
public:
    bool notify(MMessage&);
    ObserverThread();
    
    /**
     * Metodo utilizzato per inizializzare thread di servizio ed eventi da 
     * osservare. Carica dal file di configurazione XML le informazioni dei nodi
     * della categoria observe
     * 
     * @return true se tutto ok, false altrimenti
     */
    //virtual bool start() = 0;
    MMessage receive_message();
    virtual bool getObservations(MParams& obs);
    virtual bool getSubjects(MParams& subj);
    bool init();
    bool start();
    virtual void start_service_threads();
    virtual void stop_service_thread();
    std::string get_name(){return id;}
protected:
    
    /**
     * @var MMessageQueue<MMessage>*
     * @brief puntatore alla coda dei messaggi che vengono inviati per le notifiche
     *        di avvenuto evento
     */
    setof<MAttribute> observes;
    setof<MAttribute> subjects;
    std::vector<Thread*> service_thread;
    virtual void do_work(ObserverThread *obj);
    virtual void do_work1(ObserverThread *obj);
};

#endif	/* OBSERVERTHREAD_H */

