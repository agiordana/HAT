/* 
 * File:   MMessage.cpp
 * Author: attilio
 * 
 * Created on November 10, 2012, 6:06 PM
 */

#include "agentlib.h"

using namespace std;

// --------------- class MstringList methods --------------------------

MstringList::MstringList() {
    mytype = "MstringList";
}

MstringList::MstringList(vector<string>& v) {
    mytype = "MstringList";
    for(unsigned i=0; i<v.size(); i++) insert(end(), v[i]);
}

MstringList::MstringList(string v) {
    mytype = "Mstring";
    this->push_back(v);
}

string MstringList::getString() {
    if(size()>0) return (*this)[0];
    else return "";
}

string MstringList::getString(unsigned n) {
    if(size()>n) return (*this)[n];
    else return "";
}

bool MstringList::add(string v) {
    if(mytype=="MstringList") {
        insert(end(),v);
    }
    else {
        (*this)[0] = v;
    }
    return true;
}

bool MstringList::add(vector<string>& v) {
    if (mytype!="MstringList") {
        return false;
    }
    for(unsigned i=0; i<v.size(); i++) insert(end(),v[i]);
    return true;
}

vector<string> MstringList::getList() {
    return vector<string> (*this);
}

string MstringList::htmlEncode(string name) {
    string r;
    if (mytype=="Mlist") {
        r = "<field name=\"" + name + "\" type=\""+mytype+ "\"  value=\"" + (*this)[0] + "\"/>";
        return r;
    }
    r = name + "={";
    for(unsigned i=0; i<size(); i++) {
        r+=(*this)[i];
        if(i<size()-1)r+=",";
    }
    r+="}";
    return r;
}

string MstringList::xmlEncode(string name) {
    string r;
    if(mytype=="Mstring") {
        r = "<field name=\"" + name + "\" type=\""+mytype+ "\"  value=\"" + (*this)[0] + "\"/>";
        return r;
    }
    r = "<field name=\"" + name + "\" type=\"" + mytype + "\">";
    for(unsigned i=0; i<size(); i++) {
        r = r+ "<item value=\""+(*this)[i]+"\"/>";
    }
    r+="</field>";
    return r;
}

bool MstringList::xmlDecode(std::string xmldesc) {
    TiXmlDocument doc;
    TiXmlNode* desc;
    TiXmlNode* pChild;
    TiXmlElement* pElement;
    doc.Parse(xmldesc.c_str(), 0 , TIXML_ENCODING_UTF8);
    desc = doc.FirstChild();
    desc = desc->NextSibling();
    if(desc == NULL) return false;
    (*this).clear();
    if(hsrv::getAttrbyName(desc->ToElement(), "type")!=mytype) return false;
    for (pChild = desc->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
        pElement = pChild->ToElement();
        if(pElement == NULL) return false;
        insert(end(), hsrv::getAttrbyName(pElement, "value"));
    }
    doc.Clear();
    return true;
}

bool MstringList::xmlDecode(TiXmlNode* desc) {
    TiXmlNode* pChild;
    TiXmlElement* pElement;
    if(desc == NULL) return false;
    (*this).clear();
    for (pChild = desc->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
        pElement = pChild->ToElement();
        if(pElement == NULL) return false;
        insert(end(), hsrv::getAttrbyName(pElement, "value"));
    }
    return true;
}


// --------------- class MMessage methods --------------------------

bool MMessage::add(string f, string v){
    std::map<std::string,MstringList>::iterator it;
    if((it=find(f))!=end()){
        it->second.add(v);
        return true;
    }
    this->insert(std::pair<string,MstringList>(f,MstringList(v))); 
    return true;
}

bool MMessage::add(string f, int v){
     std::map<std::string,MstringList>::iterator it;
     string r = hsrv::int2a(v);
    if((it=find(f))!=end()){
        it->second.add(r);
        return true;
    }
    this->insert(std::pair<string,MstringList>(f,MstringList(r))); 
    return true;
}

bool MMessage::add(string f, long v){
     std::map<std::string,MstringList>::iterator it;
     string r = hsrv::long2a(v);
    if((it=find(f))!=end()){
        it->second.add(r);
        return true;
    }
    this->insert(std::pair<string,MstringList>(f,MstringList(r))); 
    return true;
}

bool MMessage::add(string f, unsigned v){
    std::map<std::string,MstringList>::iterator it;
    string r = hsrv::unsigned2a(v);
    if((it=find(f))!=end()){
        it->second.add(r);
        return true;
    }
    this->insert(std::pair<string,MstringList>(f,MstringList(r))); 
    return true;
}

bool MMessage::add(string f, double v){
    std::map<std::string,MstringList>::iterator it;
    string r = hsrv::double2a(v);
    if((it=find(f))!=end()){
        it->second.add(r);
        return true;
    }
    this->insert(std::pair<string,MstringList>(f,MstringList(r))); 
    return true;
}

bool MMessage::add(string f, vector<string>& v){
    std::map<std::string,MstringList>::iterator it;
    if((it=find(f))!=end()){
        if(it->second.hasType()!="MstringList") return false;
        else {
            it->second.add(v);
        }
        return true;
    }
    this->insert(std::pair<string,MstringList>(f,MstringList(v))); 
    return true;
}

map<string, string> MMessage::getElements() {
    map<string, MstringList>::iterator it;
    map<string, string> elements;
    
    for(it = (*this).begin(); it != (*this).end(); it++) {
        if(it->second.hasType() == "Mstring") {
            elements.insert(pair<string, string>(it->first, it->second.getString()));
        }
    }
    
    return elements;
}

string MMessage::getString(std::string f){
    std::map<std::string,MstringList>::iterator it;
    if((it=find(f))!=end()) {
        return (*it).second.getString();
    }
    else return "";
}

int MMessage::getInt(std::string f){
    string r;
    std::map<std::string,MstringList>::iterator it;
    if((it=find(f))!=end()) {
        r = (*it).second.getString();
        return hsrv::a2int(r);
    }
    else return 0;
}

unsigned MMessage::getUnsigned(std::string f){
    string r;
    std::map<std::string,MstringList>::iterator it;
    if((it=find(f))!=end()) {
        r = (*it).second.getString();
        return hsrv::a2unsigned(r);
    }
    else return 0;
}

long MMessage::getLong(std::string f){
    string r;
    std::map<std::string,MstringList>::iterator it;
    if((it=find(f))!=end()) {
        r = (*it).second.getString();
        return hsrv::a2long(r);
    }
    else return 0;
}

double MMessage::getDouble(std::string f){
    string r;
    std::map<std::string,MstringList>::iterator it;
    if((it=find(f))!=end()) {
        r = (*it).second.getString();
        return hsrv::a2double(r);
    }
    else return 0;
}

std::vector<std::string> MMessage::getList(std::string f){
    std::vector<std::string> r;
    std::map<std::string,MstringList>::iterator it;
    if((it=find(f))!=end()){
        if((*it).second.hasType()=="MstringList"){
            return (*it).second.getList();        
        }
    }
    return r;
}

bool MMessage::remove(std::string f) {
    std::map<std::string,MstringList>::iterator it;
    if((it=find(f))!=end()) {
        erase(it);
        return true;
    } 
    return false;
 }

string MMessage::htmlEncode() {
    string r="<html><body>";
    map<string,MstringList>::iterator it;

    if((it=find("body"))!=end()) {
        r = it->second.getString();
        return r;
    }
   
    for( it=begin() ; it != end(); it++ )
        r = r + it->second.htmlEncode(it->first) +"<br>";

    r += "</body></html>";

    return r;
}

string MMessage::htmlPostEncode() {
    string r = "";
    map<string,MstringList>::iterator it;

    if((it = find("body"))!=end()) {
        r = it->second.getString();
        return r;
    }

    for( it = begin() ; it != end(); it++ )
        if(it == begin())
            r = r+it->second.htmlEncode(it->first);
        else
            r = r + "&" + it->second.htmlEncode(it->first);

    return r;
}

string MMessage::xmlEncode(int md) {
    map<string, MstringList>::iterator it;
    string r = "";
   
    if(md > 0)
    r = "<?xml version=\"1.0\" ?>";
   
    r = r + "<message type=\"" + mtype + "\" subtype=\""+msubtype+"\">";

    for(it = (*this).begin(); it != (*this).end(); it++) r = r + it->second.xmlEncode(it->first);

    r += "</message>";
    return r;
}


bool MMessage::xmlDecode(string& xmldesc) {
    TiXmlDocument doc;
    TiXmlNode* desc;
    TiXmlNode* pChild;
    TiXmlElement* pElement;
    //hsrv::charExplode(xmldesc);
    //hsrv::charPrintf(xmldesc.c_str());
   
    doc.Parse(xmldesc.c_str(), 0 , TIXML_ENCODING_UTF8);
    desc = doc.FirstChild();
    desc = desc->NextSibling();
    string ndtype;
   
    if(desc == NULL)
    return false;

    mtype = hsrv::getAttrbyName(desc->ToElement(), "type");
    msubtype = mtype = hsrv::getAttrbyName(desc->ToElement(), "subtype");
    myclear(1);
    for (pChild = desc->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
           pElement = pChild->ToElement();
           ndtype = hsrv::getAttrbyName(pElement, "type");
           if(ndtype=="Mstring")
               (*this)[hsrv::getAttrbyName(pElement, "name")] = MstringList(hsrv::getAttrbyName(pElement, "value"));
           else {
               if(ndtype=="MstringList") {
                   (*this)[hsrv::getAttrbyName(pElement, "name")] = MstringList();
                   (*this)[hsrv::getAttrbyName(pElement, "name")].xmlDecode(pChild);
               }
                   
           }
    }
    desc->Clear();
    return true;
}


bool MMessage::xmlDecode(TiXmlNode* desc) {
    TiXmlNode* pChild;
    TiXmlElement* pElement;
    
    string ndtype;
   
    if(desc == NULL) return false;
    
    mtype = hsrv::getAttrbyName(desc->ToElement(), "type");
    msubtype = mtype = hsrv::getAttrbyName(desc->ToElement(), "subtype");
    myclear();
    for (pChild = desc->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
           pElement = pChild->ToElement();
           ndtype = hsrv::getAttrbyName(pElement, "type");
           if(ndtype=="Mstring")
               (*this)[hsrv::getAttrbyName(pElement, "name")] = MstringList(hsrv::getAttrbyName(pElement, "value"));
           else {
               if(ndtype=="MstringList") {
                   (*this)[hsrv::getAttrbyName(pElement, "name")] = MstringList();
                   (*this)[hsrv::getAttrbyName(pElement, "name")].xmlDecode(pChild->FirstChild());
               }
                   
           }
    }
    return true;
}

bool MMessage::xmlPrint(ostringstream& out, int md) {
    map<string, MstringList>::iterator it;

    if(md > 0)
    out << "<?xml version=\"1.0\" ?>" << endl;

    out << "<message type=\"" + mtype + "\" subtype=\"" + msubtype +"\">" << endl;

    for(it = (*this).begin(); it!=(*this).end(); it++)
        out << "<field name=\"" << it->first << "\" value=\"" << it->second.getString() << "\"/>" << endl;
    out << "</message>" << endl;

    return true;
}


bool MMessage::xmlAppend(string& xmldesc) {
    TiXmlDocument doc;
    TiXmlNode* desc;
    TiXmlNode* pChild;
    TiXmlElement* pElement;
    string ndtype;

    doc.Parse(xmldesc.c_str(), 0 , TIXML_ENCODING_UTF8);
    desc = doc.FirstChild();
    desc = desc->NextSibling();
   
    if(desc == NULL)
    return false;

    mtype = hsrv::getAttrbyName(desc->ToElement(), "type");
    msubtype = hsrv::getAttrbyName(desc->ToElement(), "subtype");
    
    for (pChild = desc->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
           pElement = pChild->ToElement();
           ndtype = hsrv::getAttrbyName(pElement, "type");
        if(ndtype == "Mstring" || ndtype == "*") {
            
            (*this)[hsrv::getAttrbyName(pElement, "name")] = MstringList(hsrv::getAttrbyName(pElement, "value"));
        }
        else {
            if(ndtype=="MstringList") {
                if (this->count(hsrv::getAttrbyName(pElement, "name")) > 0) {
                }
                (*this)[hsrv::getAttrbyName(pElement, "name")] = MstringList();
                (*this)[hsrv::getAttrbyName(pElement, "name")].xmlDecode(pChild->FirstChild());
            }
        }
    }
    doc.Clear();
    return true;
}

bool MMessage::htmlAppend(string& m) {
        string tname = "";
        string tvalue = "";
        int flag = 0;
        msubtype = "*";

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
                else {
                    if(tname == "subtype") msubtype = tvalue;
                    else (*this)[tname] = MstringList(tvalue);
                }

                tname = tvalue="";
                                break;
                        default:
                                if(flag) tvalue += m[i];
                                else tname += m[i];
                                break;
                }
        }
        if(tname=="type") mtype=tvalue;
        else {
            if(tname == "subtype") msubtype = tvalue;
                else {
                        if(tname != "" && tvalue != "") (*this)[tname] = MstringList(tvalue);
                }
        }
        return true;
}


bool MMessage::myclear(int md) {
    this->clear();
    return true;
}
