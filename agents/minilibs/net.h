//
//  net.h
//  phoneagent
//
//  Created by Davide Monfrecola on 11/15/11.
//  Copyright 2011 Università del Piemonte Orientale. All rights reserved.
//

#ifndef phoneagent_net_h
#define phoneagent_net_h

/** @brief Gestione informazioni di rete
 
 Gestione delle informazioni di rete (porta, indirizzo IP, controllo IP)
 @author Attilio Giordana
 @date Novembre 2011
 */
class Net {
public:
    /**
     Estrae l'indirizzo ip dalla stringa passata
     @param s stringa nel formato <indirizzo ip>:<porta>
     @return indirizzo ip
     */
    std::string getip(std::string&);
	/**
     Estrae l'indirizzo ip dalla stringa passata
     @param ip stringa nel formato <indirizzo ip>
     @return indirizzo ip di tipo unsigned
     */
    unsigned getipaddress(std::string&);
	/**
     Estrae la porta dalla stringa passata
     @param s stringa contenente indirizzo ip e porta
     @return porta
     */
    std::string getport(std::string&);
    /**
     Estrae la porta dalla stringa passata
     @param port stringa con la porta da convertire
     @return porta di tipo unsigned short
     */
	unsigned short getportaddr(std::string&);
    /**
	 @param s
     @deprecated
     */
	std::string getsources(std::string&);

protected:
    /**
     Controlla che l'indirizzo ip passato sia un indirizzo IP valido
     @param adr indirizzo IP
     @return 1 se l'indirizzo IP è in un formato valido, 0 altrimenti
     */
	int checkip(std::string&);
    /**
     Recupera l'indirizzo IP della macchina identificata dal nome passato
     @param adr hostname del quale recuperare l'IP
     @return indirizzo IP se è un hostname valido o se è già un indirizzo IP, NULL
             altrimenti
     */
	std::string getIp(std::string&);	
};


#endif
