//
//  attribute.cpp
//  phoneagent
//
//  Created by Davide Monfrecola on 11/15/11.
//  Copyright 2011 Università del Piemonte Orientale. All rights reserved.
//


using namespace std;

#include "agentlib.h"

//------------------------- Class Attribute --------------------------------

int Attribute::xmlDecode(TiXmlNode* desc) {
	TiXmlElement* pElement;
	
	if(desc == NULL) 
		return 0;
	pElement = desc->ToElement();
	
	if(pElement == NULL) 
		return 0;
		
	name = hsrv::getAttrbyName(pElement, "name");
	value = hsrv::getAttrbyName(pElement, "value");
	
	return 1;
}

int Attribute::xmlDecode(string& xmldesc) {
	TiXmlDocument doc;
	TiXmlNode* prg;
	TiXmlElement* pElement;
	const char* pTest = doc.Parse(xmldesc.c_str(), 0 , TIXML_ENCODING_UTF8);
	
	if(pTest != NULL) 
		return 0;
	
	prg = doc.FirstChild();
	
	if(prg == NULL) 
		return 0;
	
	prg = prg->NextSibling();
	
	if(prg == NULL) 
		return 0;
	
	pElement = prg->ToElement();
	
	if(pElement == NULL) 
		return 0;
	
	name = hsrv::getAttrbyName(pElement, "name");
	value = hsrv::getAttrbyName(pElement, "value");
	
	return 1;
}

int Attribute::xmlLoad(string name) {
	bool res;
	TiXmlDocument* doc;
	
	if(!hsrv::isfile(name, "xml")) 
		return 0;
	
	doc = new TiXmlDocument(name.c_str());
	res = doc->LoadFile();
	
	if(!res) 
		return 0;
	
	return xmlDecode(doc->FirstChild()->NextSibling());
}


int Attribute::xmlSave(string name) {
	ofstream out;
	string r;
	
	out.open(name.c_str());
	r = xmlEncode(1);
	out << r << endl;
	out.close();
	
	return 1;
}
