/*
 *  namelist.cpp
 *  mngagent
 *
 *  Created by Attilio Giordana on 1/13/12.
 *  Copyright 2012 Università del Piemonte Orientale. All rights reserved.
 *
 */

using namespace std;

#include "agentlib.h"

bool NameList::init(string names, char delim) {
	string tmp = names;
    if(names=="") return false;
	size_t found;
    clear();    
	while ((found = tmp.find(delim)) != string::npos) {
		push_back(tmp.substr(0,found));
		tmp = tmp.substr(found+1);
	}
    
	if(tmp != "") push_back(tmp);
    
	return size() > 0;
}

bool NameList::add(string name) {
    push_back(name);
    return true;
}

bool NameList::member(string name) {
    
	for(unsigned i=0; i<size(); i++) {
		if(name == (*this)[i]) 
            return true;
    }
    
	return false;
}

int NameList::find(string name) {
	
    for(unsigned i=0; i<size(); i++) {
		if(name == (*this)[i]) {
			return (int)i;
		}
    }
    
	return -1;
}