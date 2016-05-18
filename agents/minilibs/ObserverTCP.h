/* 
 * File:   ObserverTCP.h
 * Author: trampfox
 *
 * Created on November 9, 2012, 9:27 AM
 */

#ifndef OBSERVERTCP_H
#define	OBSERVERTCP_H


class ObserverTCP: public MRemoteAgent, public Observer {
public:
    ObserverTCP(std::string, std::string);
    
    bool notify(MMessage&);
    
    MMessage receive_message();
    
    void dispatcher(ObserverTCP*);
    
    /**
     * Metodo utilizzato per inizializzare thread di servizio ed eventi da 
     * osservare. Carica dal file di configurazione XML le informazioni dei nodi
     * della categoria observe
     * 
     * @return true se tutto ok, false altrimenti
     */
    bool start();
    /**
     * Metodo utilizzato per fermare thread di servizio e rimuovere gli eventi da 
     * osservare.
     * 
     * @return true se tutto ok, false altrimenti
     */
    bool stop();
    
protected:
        
    std::vector<Thread*> service_threads;
    
    setof<MAttribute> observes;
};

#endif	/* OBSERVERTCP_H */

