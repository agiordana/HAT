/* 
 * File:   Thread.h
 * Author: trampfox
 *
 * Created on November 15, 2012, 11:39 AM
 */

#ifndef THREAD_H
#define	THREAD_H

//#include "agentlib.h"

class Thread: boost::thread {
public:
    
    Thread() {};
    
    /**
     * Crea un nuovo thread, utilizzando le librerie boost::thread.
     * 
     * @param worker puntatore al metodo che deve essere eseguita dalla
     *               thread appena istanziata
     * @param obj puntatore all'oggetto, utilizzato per specificar il contesto di
     *            esecuzione della thread.
     */
    template <class F, class O>
    Thread(F worker, O* obj) {
        m_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(worker, obj, obj )));
    };
    
    ~Thread() {m_thread->interrupt();};

private:
    boost::shared_ptr<boost::thread> m_thread;
};

#endif	/* THREAD_H */

