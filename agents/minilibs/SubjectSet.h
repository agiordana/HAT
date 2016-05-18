/* 
 * File:   SubjectSet.h
 * Author: Davide Monfrecola<davide.monfrecola@gmail.com>
 *
 * Created on November 13, 2012, 9:58 AM
 */

#ifndef SUBJECTSET_H
#define	SUBJECTSET_H

//#include "agentlib.h"
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp> 
#include <boost/thread/locks.hpp> 
#include <boost/thread/condition.hpp> 


class Observer;
class SubjectSet;
class MMessage;

class SubjectSet {

public:
    
    /**
     * Aggiunge un soggetto alla mappa dei soggetti
     * 
     * @param std::string tipo del Subject 
     * @param std::string sottotipo del Subject
     * @param Subject istanza del Subject 
     */
    static void add_subject(std::string, std::string);
    /**
     @param std::string tipo del Subject 
     @param std::string sottotipo del Subject
     @return true se il subject appartiene a quelli dichiaraty, false altrimenti
     */
    static bool check_subject(std::string&, std::string&);
    /**
     * Aggiunge un observer al Subject identificato dal tipo e dal sottotipo passato
     * 
     * @param std::string tipo del Subject 
     * @param std::string sottotipo del Subject 
     * @param Observer istanza dell'Observer "interessato" al Subject
     * @return true se il tipo e il sottotipo esistono, false altrimenti
     */
    static bool add_observer(std::string, std::string, Observer*);
    /**
     * Rimuove un observer dal Subject identificato dal tipo e dal sottotipo passato
     * 
     * @param std::string tipo del Subject 
     * @param std::string sottotipo del Subject 
     * @param Observer istanza dell'Observer "interessato" al Subject
     * @return true se il tipo e il sottotipo esistono, false altrimenti
     */
    static bool remove_observer(Observer*);
    static bool remove_observer(std::string id);
    /**
     * Notifica gli Observer "interessati" al Subject specificato
     * 
     * @param std::string tipo del Subject 
     * @param std::string sottotipo del Subject 
     * @param Message istanza del messaggio da passare agli Observer
     * @return 
     */
    static bool notify(std::string, std::string, MMessage&);
    /**
     * Notifica gli Observer "interessati" al Subject specificato
     * In questo caso le chiavi di identificazione del subject sono lette dall'header del messsaggio
     * @param Message istanza del messaggio da passare agli Observer
     * @return 
     */
    static bool notify(MMessage&);
    
    /**
     * Metodo di debug
     */
    static void printSet();
    
    static std::map<std::pair<std::string,std::string>, Subject> subjects;
private:
    static boost::mutex mutexclusion;
    static bool isDummy(std::string&);
};

#endif	/* SUBJECTSET_H */

