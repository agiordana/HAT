/* 
 * File:   MMessageQueue.h
 * Author: Davide Monfrecola<davide.monfrecola@gmail.com>
 *
 * Created on November 19, 2012, 12:14 PM
 */

#ifndef MMESSAGEQUEUE_H
#define	MMESSAGEQUEUE_H

#define MAXQLENGTH 6

//#include "agentlib.h"
//#include <boost/thread.hpp>
//#include <boost/thread/mutex.hpp> 
//#include <boost/thread/locks.hpp> 
//#include <boost/thread/condition.hpp> 

/** @brief Template per coda di messaggi
 
 Template per una coda di messaggi con priorità. Viene gestita anche la mutua esclusione attraverso 
 l'utilizzo del meccanismo dei semafori.
 @author Attilio Giordana
 @date Novembre 2011
 */
template<class Q> class MMessageQueue: public std::vector<Q> {
public:
    /**
     Crea una nuova istanza di una coda di messaggi di oggetti di tipo Q. Inizializza la variabile
     di condizione e quella di mutua esclusione usate dalle varie thread
     */
	MMessageQueue(){ 
		maxlength = 0;
	};
	
	MMessageQueue(int n){ 
		if(n>0) maxlength = n;
		else maxlength = 0;
	};
	
    /**
     Elimina la coda di messaggi e le variabili di condizione e di mutua esclusione usate dalle 
     thread
     */
	~MMessageQueue() {};
    /**
     Aggiunge un'istanza della classe Q al vettore in mutua esclusione
     @param m istanza della classe Q da inserire nel vettore
     @return 1 se tutto ok
     */
	int send(Q& m) {
        boost::unique_lock<boost::mutex> lock(mut);
		this->push_back(m);
		purge();
		cond.notify_one();
        return 1;
	};
    /**
     Elimina tutti gli elementi dal vettore
     @return 1 se tutto ok
     */
	int reset() {
		while((*this).size()>0) 
                        erase((*this).begin());
		return 1;
	};
	/**
     Estrae un'istanza di un oggetto Q in mutua esclusione e lo elimina dal vettore.
     Gli oggetti vengono estratti in base alla loro priorità
     @return oggetto con priorità più alta
     */
	Q receive() {
		boost::unique_lock<boost::mutex> lock(mut);
		Q v;
		unsigned i;
		unsigned r = 0;

        if((*this).size() == 0) 
            cond.wait(lock);

        for(i = 0; i < (*this).size(); i++) { 
            if((*this)[i].priority > (*this)[r].priority) 
                r = i;
        }
        
		v = (*this)[r];
		this->erase((*this).begin()+r);

		return v;
	};
    /**
     Ritorna il valore della priorità più alta cercando tra tutti gli oggetti presenti nella coda
     @return valore priorità più alta
     */
	int priority() {
		int c = -1;
        boost::unique_lock<boost::mutex> lock(mut);
		
                for(unsigned i=0; i<(*this).size(); i++) {
                        if((*this)[i].priority > c) 
                                c = (*this)[i].priority;
                }
        
		return c;
	};
     
private:
    
    /**
     @var boost::mutex 
     @brief variabile utilizzata per la mutua esclusione 
     */
    boost::mutex mut;
    
	/**
     @var boost::condition_variable 
     @brief variabile di condizione 
     */
    boost::condition_variable cond; 
    
	int maxlength;
	
	int purge() {
		unsigned n = 0;
        
		if (maxlength == 0 || maxlength >= (*this).size()) 
            return 0;
        
		for(unsigned i=0; i<(*this).size(); i++) {
            if((*this)[i].priority<(*this)[n].priority) 
                n = i;
        }
        
		(*this).erase((*this).begin()+n);
        return 1;
	};
};

#endif	/* MMESSAGEQUEUE_H */

