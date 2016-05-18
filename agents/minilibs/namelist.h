/*
 *  namelist.h
 *  mngagent
 *
 *  Created by Attilio Giordana on 1/13/12.
 *  Copyright 2012 Università del Piemonte Orientale. All rights reserved.
 *
 */

#ifndef _name_list_
#define _name_list_

//#include "agentlib.h"

/** @brief 
 
 
 @author Attilio Giordana
 @date Gennaio 2012
 */

class NameList: public std::vector<std::string> {
public:
    /**
     Crea un vettore di stringhe a partire dalla stringa passata come primo parametro. Ogni elemento
     corrisponde ad una sottostringa formata dallo split in base al dilimitatore scelto (+ è quello
     di default)
     @param names stringa che contiene gli elementi con i quali formare il vettore
     @param delim delimitatore da considerare per formare le varie sottostringhe
     */
	bool init(std::string names, char delim = '+');
    /**
     Aggiunge un nome alla lista
     @param name stringa che contiene il nome da aggiungere
     @return true
     */
    bool add(std::string name);
    /**
     Controlla se la stringa passata come parametro appartiene al vettore
     @param name stringa da ricercare nel vettore
     @return true se l'elemento viene trovato, false altrimenti
     */
	bool member(std::string);
    /**
     Controlla se la stringa passata come parametro appartiene al vettore
     @param name stringa da ricercare nel vettore
     @return indice dell'elemento se viene trovato, -1 altrimenti
     */
	int find(std::string);
};

#endif