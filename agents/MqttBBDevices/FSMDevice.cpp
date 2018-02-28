//
//  FSMDevice.cpp
//  ioagent
//
//  Created by Attilio Giordana on 3/6/13.
//  Copyright (c) 2013 Università del Piemonte Orientale. All rights reserved.
//

#include "FSMDevice.h"

using namespace std;


FSMCondition::FSMCondition(TiXmlNode* root) {
    TiXmlElement* pElement;
    pElement = root->ToElement();
    this->type = hsrv::getAttrbyName(pElement,"type");
    this->subtype = hsrv::getAttrbyName(pElement,"subtype");
    this->value = hsrv::getAttrbyName(pElement,"value");
    this->key = type+"_"+subtype+"_"+value;
}

bool FSMCondition::operator == (FSMCondition c) {
    return (type == c.type && subtype == c.subtype && value == c.value);
}

bool FSMCondition::operator != (FSMCondition c) {
    return (type != c.type || subtype != c.subtype || value != c.value);
}

bool FSMCondition::operator < (FSMCondition c) {
    if(type < c.type) return true;
    if(type == c.type && subtype < c.subtype) return true;
    if(type == c.type && subtype == c.subtype && value < c.value) return true;
    return false;
}

bool FSMCondition::operator <=(FSMCondition c) {
    return (*this < c || *this==c);
}


bool FSMCondition::operator >(FSMCondition c) {
    if(type > c.type) return true;
    if(type == c.type && subtype > c.subtype) return true;
    if(type == c.type && subtype == c.subtype && value > c.value) return true;
    return false;
}

bool FSMCondition::operator >=(FSMCondition c) {
    return (*this > c || *this==c);
}

bool FSMCondition::input(setof<MAttribute>& input_set) {
    if(type=="local_event") input_set.push_back(MAttribute(subtype, "input", "active"));
    return true;
}

Action::Action(TiXmlNode* root) {
    TiXmlElement* pElement;
    pElement = root->ToElement();
    this->type = hsrv::getAttrbyName(pElement,"type");
    this->subtype = hsrv::getAttrbyName(pElement,"subtype");
    this->value = hsrv::getAttrbyName(pElement,"value");
}

bool Action::subjects(MParams& subj) {
    subj.add(subtype, "subject", type);
    return true;
}

bool Action::output(setof<MAttribute>& output_set) {
    if(type=="devcmd") output_set.push_back(MAttribute(subtype, "output", "active"));
    return true;
}

Transition::Transition(TiXmlNode* root) {
    TiXmlNode* pChild;
    TiXmlElement* pElement;
    pElement = root->ToElement();
    if(!pElement) return;
    this->name = hsrv::getAttrbyName(pElement, "name");
    this->newstate = hsrv::getAttrbyName(pElement, "to");
    condition = FSMCondition(root->FirstChild());
    for ( pChild = root->FirstChild()->NextSibling()->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
        action.push_back(Action(pChild));
    }
}

bool Transition::print(){
    cout <<"    Transition: "<<name<<" newstate="<<newstate<<endl;
    this->condition.print();
    for (size_t i=0; i<this->action.size(); i++) {
        this->action[i].print();
    }
    return true;
}

bool Transition::input(setof<MAttribute>& input_set) {
    condition.input(input_set);
    return true;
}

bool Transition::output(setof<MAttribute>& output_set) {
    for(size_t i=0; i<action.size(); i++) action[i].output(output_set);
    return true;
}

bool Transition::observations(MParams& obs) {
    obs.add(condition.subtype, "observe", condition.type);
    return true;
}

bool Transition::subjects(MParams& subj) {
    for(size_t i=0; i<action.size(); i++) action[i].subjects(subj);
    return (subj.size()>0);
}

bool Transition::activeFSMConditions(std::vector<FSMCondition>& c) {
    for (size_t i=0; i<c.size(); i++) {
        if(condition == c[i]) return true;
    }
    c.push_back(condition);
    return true;
}

bool Transition::exec(FSMCondition& c, std::vector<MMessage>& m, size_t& ns) {
    MMessage am;
    if (c == condition) {
        for (size_t i=0; i<action.size(); i++) {
            am.mtype = action[i].type;
            am.msubtype = action[i].subtype;
            am.add("value", action[i].value);
            m.push_back(am);
        }
        ns = nextstate;
        return true;
    }
    return false;
}

bool Transition::compileNumeric(string sn, size_t nn){
    if (newstate == sn) nextstate = nn;
    return true;
}

State::State(TiXmlNode* root) {
    TiXmlNode* pChild;
    TiXmlElement* pElement;
    pElement = root->ToElement();
    if(!pElement) return;
    this->name = hsrv::getAttrbyName(pElement, "name");
    if(hsrv::getAttrbyName(pElement, "name") == "TRUE") this->start = true;
        else this->start = false;
    for ( pChild = root->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
        this->push_back(Transition(pChild));
    }
}

bool State::print() {
    cout<<"  State: "<<name<<" ["<<endl;
    for (size_t i = 0; i<this->size(); i++) {
        (*this)[i].print();
    }
    cout<<"  ]"<<endl;
    return true;
}

bool State::observations(MParams& obs) {
    for(size_t i=0; i<this->size(); i++) (*this)[i].observations(obs);
    return (obs.size()>0);
}

bool State::input(setof<MAttribute>& input_set) {
    for(size_t i=0; i<this->size(); i++) (*this)[i].input(input_set);
    return (input_set.size()>0);
}

bool State::output(setof<MAttribute>& output_set) {
    for(size_t i=0; i<this->size(); i++) (*this)[i].output(output_set);
    return (output_set.size()>0);
}

bool State::subjects(MParams& subj) {
    for(size_t i=0; i<this->size(); i++) (*this)[i].subjects(subj);
    return (subj.size()>0);
}

bool State::activeFSMConditions(std::vector<FSMCondition>& c) {
    for(size_t i=0; i<this->size(); i++)
	 (*this)[i].activeFSMConditions(c);
    return c.size()>0;
}

bool State::exec(FSMCondition& c, std::vector<MMessage>& m, size_t& ns) {
    bool done = false;
    for(size_t i=0; i<this->size()&&!done; i++)
        done = (*this)[i].exec(c, m, ns);
    return done;
}

bool State::compileNumeric(string sn, size_t nn){
    if(sn == name) numeric_name = nn;
    for(size_t i=0; i<this->size(); i++)
        (*this)[i].compileNumeric(sn, nn);
    return true;
}

FSMDevice::~FSMDevice() {
}

FSMDevice::FSMDevice(){
    name = "";
    doc = NULL;
    current = oldstate = 0;
    currentValue = "OFF";
    publish();
}

FSMDevice::FSMDevice(string dir, string fname) {
    bool res;
    string pathname;
    TiXmlNode* pChild;
    TiXmlNode* dev;
    TiXmlElement* pElement;
    pathname = hsrv::configdir+"/"+ dir + "/" + fname +".xml";
    current = 0;
    oldstate = 0;
    doc = NULL;
    name = fname;
    this->load(pathname);
    this->activeFSMConditions(expected);
    currentValue = "OFF";
    publish();
}

bool FSMDevice::remove() {
   if (doc!=NULL) delete doc;
   doc = NULL;
   string pathname;
   pathname = hsrv::configdir+"/devices/"+name + ".xml";
   if(hsrv::isfile(pathname)) FileManager::deleteFile(pathname);
   return true;
}

bool FSMDevice::load(string fname) {
    bool res;
    string pathname;
    string tname;
    TiXmlNode* pChild;
    TiXmlNode* dev;
    TiXmlElement* pElement;
    current = 0;
    pathname = fname;
 //   pathname = hsrv::configdir+"/devices/"+fname;
    this->clear();
    if (doc!=NULL) delete doc;
    doc = NULL;
    if(hsrv::isfile(pathname)) {
            doc = new TiXmlDocument(pathname.c_str());
            res = doc->LoadFile();
            if(res) {
                dev = doc->FirstChild();
                dev = dev->NextSibling();
                pElement = dev->ToElement();
                if(hsrv::getAttrbyName(pElement, "name") == "*") {
		   dev = dev->NextSibling();
                   pElement = dev->ToElement();
		}
                tname = hsrv::getAttrbyName(pElement, "name");
    		alias = hsrv::getAttrbyName(pElement, "alias");
                for ( pChild = dev->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
                    this->push_back(State(pChild));
                }
                for (size_t i=0; i<this->size(); i++) {
                    for (size_t j=0; j<this->size(); j++) {
                        (*this)[j].compileNumeric((*this)[i].name, i);
                    }
                }
                for(size_t i=0; i<this->size(); i++) if((*this)[i].start) current=oldstate=i;
	        status = "ON";
                return true;
	    }
	    else return false;
	}
	return false;
}

bool FSMDevice::getObservations(MParams& obs) {
    for (size_t i=0; i<this->size(); i++) {
        (*this)[i].observations(obs);
    }
    return obs.size()>0;
}

bool FSMDevice::activeObservations(MParams & obs) {
    return (*this)[current].observations(obs);
}

bool FSMDevice::getSubjects(MParams& subj){
    for (size_t i=0; i<this->size(); i++) {
        (*this)[i].subjects(subj);
    }
    return subj.size()>0;
}

setof<MAttribute> FSMDevice::input() {
    setof<MAttribute> input_set;
    for (size_t i=0; i<this->size(); i++) 
	(*this)[i].input(input_set);
    return input_set;
}

setof<MAttribute> FSMDevice::output() {
    setof<MAttribute> output_set;
    for (size_t i=0; i<this->size(); i++) 
	(*this)[i].output(output_set);
    return output_set;
}

bool FSMDevice::print() {
    cout<<"DEVICE: "<<name<<"  {"<<endl;
    for(size_t i=0; i<this->size(); i++) {
        (*this)[i].print();
    }
    cout <<"  Current_state; "<<(*this)[current].name<<endl;
    cout <<"  Previous_state; "<<(*this)[oldstate].name<<endl;
    cout<<"}"<<endl;
    return true;
}

size_t FSMDevice::exec(FSMCondition& c, std::vector<MMessage>& m) {
    oldstate = current;
    return (*this)[current].exec(c, m, current);
}

bool FSMDevice::execcmd(MMessage& msg) {
    vector<MMessage> res;
    size_t i;
    FSMCondition c(msg.mtype, msg.msubtype, msg.getString("value"));
    for(i=0; i<expected.size()&&c!=expected[i]; i++);
    if(i<expected.size()) {
        exec(c, res);
        expected.clear();
        activeFSMConditions(expected);
        for(size_t j=0; j<res.size(); j++) {
	    if(res[j].mtype == "event" && res[j].msubtype.find(name)!= string::npos) {
		currentValue = res[j].getString("value");
	    }
            SubjectSet::notify(res[j]);
        }
	hsrv::rpcNotify(msg.getString("tag"), "value", currentValue);
	publish();
        if(hsrv::mqtt == "ON") {
            MMessage mqtt_mess = mkMqttmessage("event", name, "value="+currentValue, msg.getString("mqtttag"));
            SubjectSet::notify(mqtt_mess); 
        }
        return true;
    }
    hsrv::rpcNotify(msg.getString("tag"), "value", currentValue);
    if(hsrv::mqtt == "ON" && msg.getString("mqtt") != "") {
        MMessage mqtt_mess = mkMqttmessage("event", name, "value="+currentValue, msg.getString("mqtttag"));
        SubjectSet::notify(mqtt_mess); 
    }
    return false;
}

bool FSMDevice::update(string channel, string value) {
    vector<MMessage> res;
    size_t i;
    FSMCondition c("local_event", channel, value);
    for(i=0; i<expected.size()&&c!=expected[i]; i++);
    if(i<expected.size()) {
        exec(c, res);
        expected.clear();
        activeFSMConditions(expected);
        for(size_t j=0; j<res.size(); j++) {
            SubjectSet::notify(res[j]);
        }
        return true;
    }
   return false;
}

bool FSMDevice::publish() {
   hsrv::publish(name, "type=\"target\" value=\""+currentValue+"\" status=\"ON\"");
   if(alias.length()>3) {
      NameList aliases;
      aliases.init(alias,'+');
      for(size_t j=0; j<aliases.size(); j++)
	 hsrv::publish(aliases[j], "type=\"source\" value=\""+currentValue+"\" status=\"ON\"");
   }
   return true;
}

