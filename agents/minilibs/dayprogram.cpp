//
//  dayprogram.cpp
//  DayProgram
//
//  Created by Davide Monfrecola on 12/7/11.
//  Copyright 2011 Università del Piemonte Orientale. All rights reserved.
//

#include "agentlib.h"

using namespace std;

DayProgram* Program::getDay(vector<string>& k) {
	map<string,DayProgram>::iterator it;
	for(unsigned i=0; i<k.size(); i++)
		if((it = find(k[i])) != end()) return &(*this)[k[i]];
	return NULL;
}


DayProgram::DayProgram(string& yy, TiXmlNode* desc) {
	TiXmlNode* pChild;
	xmldesc = desc;
	year = yy;
    
	day = hsrv::getAttrbyName(desc->ToElement(), "day_number");
	month = hsrv::getAttrbyName(desc->ToElement(), "month_number");
	week = hsrv::getAttrbyName(desc->ToElement(), "weekday");
	
    for ( pChild = desc->FirstChild()->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		insert(end(), Period(pChild));
	}
	
}

int DayProgram::printstring(ostringstream& out) {
	out<<"Day: "<<day<<", month: "<<month<<", weekday: "<<week<<", year: "<<year<<endl;
	
    for(unsigned i=0; i<size(); i++) {
        (*this)[i].printstring(out);
    }
	
    return 1;
}

int DayProgram::xmlprint(ostringstream& out) {
	out << *xmldesc;
	
    return 1;
}

vector<string>* DayProgram::isActive(int t) {
	unsigned i;
	vector<string>* tmp;
    
	for(i = 0; i < size(); i++) {
		tmp = (*this)[i].isActive(t);
		if(tmp != NULL) { 
            return tmp;
        }
	}
    
	return NULL;
}
