/*
 *  namelist.h
 *  mngagent
 *
 *  Created by Attilio Giordana on 1/13/12.
 *  Copyright 2012 Università del Piemonte Orientale. All rights reserved.
 *
 */

#ifndef _math_expression_
#define _math_expression_

#include "agentlib.h"

/** @brief 
 
 
 @author Attilio Giordana
 @date Gennaio 2012
 */
class MathExpression: public std::vector<std::string> {
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
    bool parse(std::string);
    bool fillTable(std::map<std::string, double>&);
    bool match(std::map<std::string, double>&);
    std::string eval(std::map<std::string, double>&);
    bool showExpression();
    bool showMap(std::map<std::string, double>&);
protected:
    std::string getSymbol(std::string);
    bool isOperator(std::string s){return (isMathOperator(s) || isBoolOperator(s));};
    bool isMathOperator(std::string);
    bool isBoolOperator(std::string);
    int tokenize(std::string, std::vector<std::string>&);
    bool isDelim(char);
    int precedence(std::string, std::string);
    bool isSymbol(std::string);
    bool specialSymbol(std::string);
    double evaluate(std::map<std::string, double>&);
    double evalOP(std::string, double, double);
};

#endif