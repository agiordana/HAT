//
//  period.cpp
//  Period
//
//  Created by Davide Monfrecola on 12/7/11.
//  Copyright 2011 Università del Piemonte Orientale. All rights reserved.
//

#include "agentlib.h"

using namespace std;

int Period::getEvents(TiXmlNode* desc) {
	TiXmlNode* pChild;
	int n = 0;
    
	for(pChild = desc->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		event.insert(event.end(), hsrv::getAttrbyName(pChild->ToElement(), "name"));
		n++;
	}
    
	return n;
}


Period::Period(TiXmlNode* desc) {
	xmldesc = desc;
    
	name = desc->Value();
	t0 = hsrv::getAttrbyName(desc->ToElement(), "from");
	t1 = hsrv::getAttrbyName(desc->ToElement(), "to");
	getEvents(desc->FirstChild());
}

int Period::printstring(ostringstream& out) {
	out << "  " << name << "(" << t0 << ", " << t1 << ");  events:";
	
    for(unsigned i = 0; i < event.size(); i++) {
        out << " " << event[i];
    }
	
    out << endl;
	return 1;
}

int Period::xmlprint(ostringstream& out) {
	out<<xmldesc;
	return 1;
}

vector<string>* Period::isActive(int t) {
	vector<string>* tmp = new vector<string>;
	int tt, tt0, tt1;
    
	tt = t;
	tt1 = hsrv::string2second(t1);
	tt0 = hsrv::string2second(t0);
	
    if(tt >= tt0 && tt <= tt1 && event.size() > 0) {
		
        for(unsigned i=0; i<event.size(); i++) {
			tmp->insert(tmp->end(), event[i]);
		}
        
		return tmp;
	}
	else {
		delete tmp;
		return NULL;
	}
    
}