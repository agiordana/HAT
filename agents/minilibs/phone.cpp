//
//  phone.cpp
//  phoneagent
//
//  Created by Davide Monfrecola on 11/15/11.
//  Copyright 2011 Università del Piemonte Orientale. All rights reserved.
//

using namespace std;

#include "agentlib.h"

//-------------------------- Class Phone ---------------------------------

Phone::Phone(TiXmlNode* prg) {
	TiXmlElement* pElement;
	if(prg != NULL) {
		pElement = prg->ToElement();
		ty = hsrv::getAttrbyName(pElement, "type");
		name = hsrv::getAttrbyName(pElement, "name");
		number = hsrv::getAttrbyName(pElement, "number");
		alarms.clear();
		alarms.xmlDecode(prg->FirstChild());
	}
}


int Phone::xmlPrint(ostringstream& out, int md) {
	if(alarms.size() == 0) {
		if(md > 0) 
            out <<"<?xml version=\"1.0\" ?>";
		out << "<phone type=\"" << ty << "\" name=\"" << name <<"\" number=\"" << number << "\"/>";
		return 1;
	}
	out << "<phone type=\"" << ty << "\" name=\""<< name << "\" number=\"" << number << "\">" << endl;
	alarms.xmlPrint(out);
	out << "</phone>";
	return 1;
}

string Phone::xmlEncode(int md){
	string r = "";
    
	if(md > 0) 
        r = "<?xml version=\"1.0\" ?>";
	if(alarms.size() == 0) 
        return (r + "<phone type=\"" + ty + "\" name=\"" + name + "\" number=\"" + number + "\"/>");
	
    r = r + "<phone type=\"" + ty + "\" name=\"" + name + "\" number=\"" + number + "\">";
	r += alarms.xmlEncode();
	r += "</phone>";
	
    return r;
}

int Phone::xmlDecode(string& xmldesc) {
	TiXmlDocument doc;
	TiXmlNode* prg;
	TiXmlElement* pElement;
	const char* pTest = doc.Parse(xmldesc.c_str(), 0 , TIXML_ENCODING_UTF8);
    
	if(pTest!=NULL) 
        return 0;
    
	prg = doc.FirstChild();
	prg = prg->NextSibling();
	pElement = prg->ToElement();
	name = hsrv::getAttrbyName(pElement, "name");
	number = hsrv::getAttrbyName(pElement, "number");
	ty = hsrv::getAttrbyName(pElement, "type");
	alarms.xmlDecode(prg->FirstChild());
	return 1;
}

int Phone::xmlDecode(TiXmlNode* desc) {
	TiXmlElement* pElement;
	
    if(desc == NULL) 
        return 0;
    
	pElement = desc->ToElement();
	
    if(pElement == NULL) 
        return 0;
    
	name = hsrv::getAttrbyName(pElement, "name");
	number = hsrv::getAttrbyName(pElement, "number");
	ty = hsrv::getAttrbyName(pElement, "type");
	alarms.xmlDecode(desc->FirstChild());
	
    return 1;
}

int Phone::xmlLoad(string name) {
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


int Phone::xmlSave(string name) {
	ofstream out;
	string r;
    
	out.open(name.c_str());
	r = xmlEncode(1);
	out << r << endl;
	out.close();
	return 1;
}

int Phone::member(string al){
	return alarms.member(al);
	return 0;
}
