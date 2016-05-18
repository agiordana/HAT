/* 
 * File:   Subject.h
 * Author: Davide Monfrecola<davide.monfrecola@gmail.com>
 * Copyright: 2012 Università del Piemonte Orientale. All rights reserved.
 *
 * Created on November 8, 2012, 3:31 PM
 */

#ifndef SUBJECT_H
#define	SUBJECT_H

#include "agentlib.h"

class Observer;
class SubjectSet;

/**
 * 
 */
class Subject {
public:
    Subject(std::string, std::string);
    Subject(const Subject& orig);
    virtual ~Subject();
    
    /**
     * Registra un nuovo Observer, che verrà notificato per tutti gli eventi 
     * generati dal Subject
     * 
     * @param Observer* puntatore all'istanza dell'observer che richiede la 
     *                  notifica degli eventi.
     * @param std::string tipo del subject per il quale registrarsi
     * @param std::string sottotipo del subject per il quale registrarsi
     */
    void register_observer(Observer* observer);
    /**
     * Deregistra un Observer, dalla lista associata al Subject
     * 
     * @param Observer* puntatore all'istanza dell'observer che richiede la 
     *                  rimozione.
     */
    void remove_observer(Observer* observer);
    void remove_observer(std::string id);
    /**
     * Invia una notifica di evento a tutti gli observer registrati
     * 
     * @param Message istanza di messaggio che contiene le informazioni sul 
     *                messaggio da inviare agli Observer
     */
    void notify_observers(MMessage&);
    
    std::string get_type() { return this->type; };
    
    std::string get_subtype() { return this->subtype; };
    
protected:
    
    std::vector<Observer*> observers;
private:
    /**
     * @var std::string
     * @brief tipo del Subject
     */
    std::string type;
    
    /**
     * @var std::string
     * @brief sottotipo del Subject
     */
    std::string subtype;
    bool isNotRegistered(Observer*);
};

#endif	/* SUBJECT_H */

