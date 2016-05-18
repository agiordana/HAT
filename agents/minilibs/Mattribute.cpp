/* 
 * File:   Mattribute.cpp
 * Author: attilio
 * 
 * Created on November 22, 2012, 11:30 AM
 */


using namespace std;

#include "agentlib.h"

//------------------------- Class MAttribute --------------------------------

string MAttribute::xmlEncode(int md) {
	string r="";
	if(md > 0) r = "<?xml version=\"1.0\" ?>";
	r = r+"<attribute name=\""+name+"\" category=\""+category+"\" value=\""+value+"\"/>"; 
	return r;
}

int MAttribute::xmlDecode(TiXmlNode* desc) {
	TiXmlElement* pElement;
	
	if(desc == NULL) 
		return 0;
	pElement = desc->ToElement();
	
	if(pElement == NULL) 
		return 0;
		
	name = hsrv::getAttrbyName(pElement, "name");
        category = hsrv::getAttrbyName(pElement, "category");
        if(category=="*") category = "generic";
	value = hsrv::getAttrbyName(pElement, "value");
	
	return 1;
}

int MAttribute::xmlDecode(string& xmldesc) {
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
        value = hsrv::getAttrbyName(pElement, "category");
        if(category=="*") category = "generic";
	value = hsrv::getAttrbyName(pElement, "value");
	
	return 1;
}

int MAttribute::xmlLoad(string name) {
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


int MAttribute::xmlSave(string name) {
	ofstream out;
	string r;
	
	out.open(name.c_str());
	r = xmlEncode(1);
	out << r << endl;
	out.close();
	
	return 1;
}


