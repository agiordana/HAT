//
//  message.cpp
//  phoneagent
//
//  Created by Davide Monfrecola on 11/15/11.
//  Copyright 2011 Università del Piemonte Orientale. All rights reserved.
//

using namespace std;

#include "agentlib.h"

//------------------------- Class Message ----------------------------------

int Message::xmlPrint(ostringstream& out, int md) {
	map<string, string>::iterator it;
    
	if(md > 0) 
        out << "<?xml version=\"1.0\" ?>" << endl;
	
    out << "<message type=\"" + mtype + "\">" << endl;
	
    for(it = (*this).begin(); it!=(*this).end(); it++) 
        out << "<field name=\"" << it->first << "\" value=\"" << it->second << "\"/>" << endl;
	out << "</message>" << endl;
    
	return 1;
}

string Message::xmlEncode(int md) {
	map<string, string>::iterator it;
	string r = "";
    
	if(md > 0) 
        r = "<?xml version=\"1.0\" ?>";
    
	r = r + "<message type=\"" + mtype + "\">";
    
	for(it = (*this).begin(); it != (*this).end(); it++) r = r + "<field name=\"" + it->first + "\" value=\"" + it->second + "\"/>";
	
    r += "</message>";
	return r;
}

string Message::htmlEncode() {
	string r="<html><body>";
	map<string,string>::iterator it;
	
    if((it=find("body"))!=end()) {
		r = it->second;
		return r;
	}
    
	for( it=begin() ; it != end(); it++ ) 
        r = r + it->first + "=" + it->second + "<br>";
	
    r += "</body></html>";
	
    return r;
}

string Message::htmlPostEncode() {
	string r = "";
	map<string,string>::iterator it;
    
	if((it = find("body"))!=end()) {
		r = it->second;
		return r;
	}
    
	for( it = begin() ; it != end(); it++ ) 
		if(it == begin()) 
            r = r+it->first + "=" + it->second;
        else 
            r = r + "&" + it->first + "=" + it->second;
    
	return r;
}

int Message::xmlAppend(string& xmldesc) {
	TiXmlDocument doc;
	TiXmlNode* desc;
	TiXmlNode* pChild;
	TiXmlElement* pElement;
	
    doc.Parse(xmldesc.c_str(), 0 , TIXML_ENCODING_UTF8);
	desc = doc.FirstChild();
	desc = desc->NextSibling();
    
	if(desc == NULL) 
        return 0;
	
    mtype = hsrv::getAttrbyName(desc->ToElement(), "type");
	
    for (pChild = desc->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		pElement = pChild->ToElement();
        // aggiunge una chiave alla mappa per ogni nodo 
		(*this)[hsrv::getAttrbyName(pElement, "name")] = hsrv::getAttrbyName(pElement, "value");
	}
    
	return 1;
}

int Message::xmlDecode(string& xmldesc) {
	TiXmlDocument doc;
	TiXmlNode* desc;
	TiXmlNode* pChild;
	TiXmlElement* pElement;
    
	doc.Parse(xmldesc.c_str(), 0 , TIXML_ENCODING_UTF8);
	desc = doc.FirstChild();
	desc = desc->NextSibling();
    
	if(desc == NULL) 
        return 0;
	
    mtype = hsrv::getAttrbyName(desc->ToElement(), "type");
	(*this).clear();
	for (pChild = desc->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		pElement = pChild->ToElement();
		(*this)[hsrv::getAttrbyName(pElement, "name")] = hsrv::getAttrbyName(pElement, "value");
	}
	return 1;
}


int Message::xmlDecode(TiXmlNode* desc) {
	TiXmlNode* pChild;
	TiXmlElement* pElement;
    
	if(desc == NULL) 
        return 0;
	
    pElement = desc->ToElement();
	(*this).clear();
	mtype = hsrv::getAttrbyName(pElement, "type");
	
    for (pChild = desc->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
        pElement = pChild->ToElement();
        
        if(pElement == NULL) 
            return 0;
		
        (*this)[hsrv::getAttrbyName(pElement, "name")] = hsrv::getAttrbyName(pElement, "value");
	}
	return 1;
}

int Message::htmlAppend(string& m) {
	string tname = "";
	string tvalue = "";
	int flag = 0;
    
	for(unsigned i = 0; i < m.size(); i++) {
		switch (m[i]) {
			case '=':
				flag = 1;
				break;
			case ' ':
				break;
			case '&':
				flag = 0;
				
                if(tname == "type") mtype = tvalue;
                else (*this)[tname] = tvalue;
				
                tname = tvalue="";
				break;
			default:
				if(flag) tvalue += m[i];
				else tname += m[i];
				break;
		}
	}
	if(tname=="type") 
        mtype=tvalue;
	else {
		if(tname != "" && tvalue != "") 
            (*this)[tname] = tvalue;
	}
	return 1;
}
