//
//  alarm.cpp
//  phoneagent
//
//  Created by Davide Monfrecola on 11/15/11.
//  Copyright 2011 Università del Piemonte Orientale. All rights reserved.
//

using namespace std;

#include "agentlib.h"

//--------------------------- Class Alarm ----------------------------------

int Alarm::xmlPrint(ostringstream& out, int md) {
	
    if(md > 0) out << "<?xml version=\"1.0\" ?>" << endl;
	
    if(phonelist.size() > 0) {
		out << "<alarm name=\"" << name << "\" priority=\"" << priority << "\" nrepeat=\"" << nrepeat << "\">" << endl;
		phonelist.xmlPrint(out, 0);
		out << "</alarm>" << endl;
	}
	else 
        out << "<alarm name=\"" << name << "\" priority=\"" << priority << "\" nrepeat=\"" << nrepeat << "\"/>" << endl;
	return 1;
}

string Alarm::xmlEncode(int md) {
	string r = "";
	
    if(md > 0) 
        r = "<?xml version=\"1.0\" ?>";
	
    if(phonelist.size() > 0) {
		r = r + "<alarm name=\"" + name + "\" priority=\"" + hsrv::int2a(priority) + "\" nrepeat=\"" + hsrv::unsigned2a(nrepeat) + "\">";
		r += phonelist.xmlEncode();
		r += "</alarm>";
	}
	else 
        r = r + "<alarm name=\"" + name + "\" priority=\"" + hsrv::int2a(priority) + "\" nrepeat=\"" + hsrv::unsigned2a(nrepeat) + "\"/>";
	return r;
}

int Alarm::xmlDecode(string& xmldesc) {
	TiXmlDocument doc;
	TiXmlNode* prg;
	TiXmlElement* pElement;
	string ts;
	const char* pTest = doc.Parse(xmldesc.c_str(), 0 , TIXML_ENCODING_UTF8);
    
	if(pTest!=NULL) 
        return 0;
    
	prg = doc.FirstChild();
	prg = prg->NextSibling();
	pElement = prg->ToElement();
	name = hsrv::getAttrbyName(pElement, "name");
	ts = hsrv::getAttrbyName(pElement, "priority");
	priority = hsrv::a2int(ts);
	ts = hsrv::getAttrbyName(pElement, "nrepeat");
	nrepeat = hsrv::a2unsigned(ts);
	phonelist.xmlDecode(prg->FirstChild());
	
    return 1;
}

int Alarm::xmlDecode(TiXmlNode* desc) {
	TiXmlElement* pElement;
	string ts;
	
    if(desc == NULL) 
        return 0;
	
    pElement = desc->ToElement();
	
    if(pElement == NULL) 
        return 0;
    
	name = hsrv::getAttrbyName(pElement, "name");
	ts = hsrv::getAttrbyName(pElement, "priority");
	priority = hsrv::a2int(ts);
	ts = hsrv::getAttrbyName(pElement, "nrepeat");
	nrepeat = hsrv::a2unsigned(ts);
	phonelist.xmlDecode(desc->FirstChild());
	
    return 1;
}

int Alarm::xmlLoad(string name) {
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


int Alarm::xmlSave(string name) {
	ofstream out;
	string r;
    
	out.open(name.c_str());
	r = xmlEncode(1);
	out << r << endl;
	out.close();
	
    return 1;
}


int Alarm::initPhoneList(setof<Phone>& ph) {
    // elimina la lista di telefoni precedentemente impostata
	phonelist.clear();
	for(unsigned i=0; i<ph.size(); i++)
		if(ph[i].member(name)) phonelist.push_back(Phone(ph[i].ty, ph[i].name, ph[i].number));
	return 1;
}


