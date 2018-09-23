//
//  pwrprogram.cpp
//  pwragent
//
//  Created by Attilio Giordana on 2/23/12.
//  Copyright (c) 2012 Università del Piemonte Orientale. All rights reserved.
//

using namespace std;

#include "agentlib.h"

FeatureValue::FeatureValue(double v, string t1, string t2) {
    value = v; 
    beginTime = t1;
    endTime = t2;
}

FeatureValue::FeatureValue(string v, string t1, string t2) {
    value = hsrv::a2double(v); 
    beginTime = t1;
    endTime = t2;
}


PwrProgram::PwrProgram() {
    doc = NULL;
}

bool PwrProgram::install() {
    DIR *dirp;
    ofstream cp;
    string home = hsrv::homedir;
    string path = home+"/programs";
    string cmd = "mkdir "+path;
    string offprog = path+"/OFF";
    string onprog = path+"/ON";
    string autoprog = path+"/AUTO";
    string manualprog = path+"/MANUAL";
    string currentp = home+"/CURRENTP";
    string previousp = home+"/PREVIOUSP";
    dirp = opendir(path.c_str());
    if(dirp == NULL) {
        system(cmd.c_str());
        cp.open(offprog.c_str());
        cp << "<?xml version=\"1.0\" ?>" << endl;
        cp << "<programs name=\"OFF\" version=\"0\"> <areas> </areas></programs>" << endl;
        cp.close();
        cp.clear();
        cp.open(manualprog.c_str());
        cp << "<?xml version=\"1.0\" ?>" << endl;
        cp << "<programs name=\"MANUAL\" version=\"0\"><areas><area name=\"ALL\"><default> <periods><period id=\"1\" from=\"00:00:00\" to=\"24:00:00\" value=\"20\"/></periods></default></area></areas></programs>" << endl;
        cp.close();
        cp.clear();
        cp.open(onprog.c_str());
        cp << "<?xml version=\"1.0\" ?>" << endl;
        cp << "<programs  name=\"ON\" version=\"0\"><areas><area name=\"ALL\"><default> <periods><period id=\"1\" from=\"00:00:00\" to=\"24:00:00\" value=\"20\"/></periods></default></area></areas></programs>" << endl;;
        cp.close();
        cp.clear();
        cp.open(autoprog.c_str());
        cp << "<?xml version=\"1.0\" ?>" << endl;
        cp << "<programs  name=\"AUTO\" version=\"0\"><areas><area name=\"ALL\"><default> <periods><period id=\"1\" from=\"00:00:00\" to=\"24:00:00\" value=\"20\"/></periods></default></area></areas></programs>" << endl;
        cp.close();
        cp.clear();
        cp.open(currentp.c_str());
        cp << "OFF" << endl;
        cp.close();
        cp.clear();
        cp.open(previousp.c_str());
        cp << "NULL" << endl;
        cp.close();
    }
    else {
        closedir(dirp);
    }
    return true;
}


bool PwrProgram::load(string fname) {
    bool res = false;
    string name;
    current = fname;
    name = hsrv::homedir+"/programs/"+fname;
    if (doc!=NULL) delete doc;
    doc = NULL;
    clear();
    areas.clear();
    if(hsrv::isfile(name, "xml")) {
        doc = new TiXmlDocument(name.c_str());
        res = doc->LoadFile();
        getDefault();
        getTargets();
    }
    return res;
}

bool PwrProgram::load() {
	ifstream prg;
	string curprog, prgname, prgpath;
	prgname = hsrv::homedir+"/CURRENTP";
	prg.open(prgname.c_str());
	if(prg.is_open()) prg >> curprog;
	else curprog = "OFF";
	prg.close();
	return load(curprog);
}


bool PwrProgram::update() {
    clear();
    areas.clear();
    getDefault();
    getTargets();
    return true;
}

FeatureValue PwrProgram::getCurrentTarget(string feature) {
    FeatureValue offValue(OFFVALUE, "00:00:00", "23::59::00");
    string key = "current_" + feature;
    if(count(key)>0) return (*this)[key];
    else if(count("current_default")>0) return (*this)["current_default"];
    else return offValue;
}

FeatureValue PwrProgram::getNextTarget(string feature) {
    FeatureValue offValue(OFFVALUE, "00:00:00", "23::59::00");
    string key = "next_" + feature;
    if(count(key)>0) return (*this)[key];
    else if(count("next_default")>0) return (*this)["next_default"];
    else return offValue;
}


bool PwrProgram::getDefault() {
    string key = "";
    string nkey = "";
    string currentFrom = "";
    string currentTo = "";
    string nextFrom = "";
    string nextTo = "";
    double cval = 0;
    double nval = 0;
    string tmpFrom = "";
    string tmpTo = "";
    string cfeature;
    TiXmlNode* pChild;
    TiXmlNode* ppChild;
    TiXmlNode* p;
    FeatureValue entry;
    string ctime = hsrv::getasciitimeday();
    if((p = doc->FirstChild()->NextSibling("programs")->FirstChild("areas"))==NULL) return false;
    for (pChild=p->FirstChild(); pChild!=NULL; pChild = pChild->NextSibling()) {
        cfeature = pChild->ToElement()->Attribute("name");
        if(cfeature!="") areas.push_back(cfeature);
        if((ppChild=pChild->FirstChild("default")) != NULL) {
            key = "default_" + cfeature;
            nkey = "ndfault_" + cfeature;
            if(getPeriods(ppChild, ctime, key)) {
                if(!getPeriods(ppChild,(*this)[key].endTime,nkey)&&count(key)>0)
                    (*this)[nkey] = (*this)[key];
            }
        }
    }
    return true;
}

bool PwrProgram::getTargets() {
    vector<int> ct;
    string key = "";
    string nkey = "";
    string dkey = "";
    string currentFrom = "";
    string currentTo = "";
    string nextFrom = "";
    string nextTo = "";
    double cval = 0;
    double nval = 0;
    string tmpFrom = "";
    string tmpTo = "";
    string cfeature = "";
    string day = "";
    int wday;
    int nday;
    TiXmlNode* pChild;
    TiXmlNode* ppChild;
    TiXmlNode* p;
    FeatureValue entry;
    string ctime = hsrv::getasciitimeday();
    ct= hsrv::CurrentTime();
    if((p = doc->FirstChild()->NextSibling("programs")->FirstChild("areas"))==NULL) return false;
    for (pChild=p->FirstChild(); pChild!=NULL; pChild = pChild->NextSibling()) {
        cfeature = pChild->ToElement()->Attribute("name");
        for (ppChild=pChild->FirstChild("day"); ppChild!=NULL; ppChild=ppChild->NextSibling()) {
            day = ppChild->ToElement()->Attribute("weekday", &wday);
            if (wday == ct[6]) {
                key = "current_" + cfeature;
                if(getPeriods(ppChild, ctime, key)) {
                    if ((*this)[key].endTime.compare("24:00:00")<0) {
                        nkey = "next_" + cfeature;
                        getPeriods(ppChild,(*this)[key].endTime,nkey);
                    }
                }
            }
        }
    }
    for (pChild=p->FirstChild(); pChild!=NULL; pChild = pChild->NextSibling()) {
        cfeature = pChild->ToElement()->Attribute("name");
        nday = ct[6]+1;
        if(nday>7) nday = 1;
        for (ppChild=pChild->FirstChild("day"); ppChild!=NULL; ppChild=ppChild->NextSibling()) {
            day = ppChild->ToElement()->Attribute("weekday", &wday);
            if (wday == nday) {
                nkey = "next_" + cfeature;
                key = "current_" + cfeature;
                dkey = "default_" + cfeature;
                if (count(nkey)==0) {
                    if (count(key)>0) getPeriods(ppChild,(*this)[key].endTime,nkey);
                    else if(count(dkey) > 0) getPeriods(ppChild,(*this)[key].endTime,dkey);
                    else getPeriods(ppChild,"00:00:00",nkey);
                }
            }
        }
    }
    return true;
}

bool PwrProgram::getPeriods(TiXmlNode* daynode, string ctime, string feature) {
    string key = feature;
    TiXmlNode* p;
    TiXmlNode* pChild;
    string currentFrom = "";
    string currentTo = "";
    string tmpFrom = "";
    string tmpTo = "";
    double cval = 0;
    double nval = 0;
    string nextFrom = "";
    string nextTo = "";
    if((p = daynode->FirstChild("periods")) == NULL) return false;
    for (pChild=p->FirstChild(); pChild!=NULL&&currentTo==""; pChild = pChild->NextSibling()) {
        tmpFrom = pChild->ToElement()->Attribute("from");
        tmpTo = pChild->ToElement()->Attribute("to");
        if (isInside(ctime, tmpFrom, tmpTo)) {
            currentFrom = tmpFrom;
            currentTo = tmpTo;
            pChild->ToElement()->QueryDoubleAttribute("value", &cval);
        }
    }
    if(currentFrom==""||currentTo=="") return false;
    (*this)[key] = FeatureValue(cval, currentFrom, currentTo);
    return true;
}

bool PwrProgram::isInside(string t, string begin, string end) {
    if(t=="24:00:00" && begin == "00:00:00") return true;
    if(begin.compare(end) < 0) return (t.compare(begin)>=0 && t.compare(end)<0);
    else {
        if(t.compare(begin)>=0 && t.compare("24:00:00")<0) return true;
        if(t.compare("00:00:00")>=0 && t.compare(end)<0) return true;
    }
    return false;
}

bool PwrProgram::show() {
    PwrProgram::iterator it;
    for(it=begin(); it!=end(); it++) {
    cout<<(*it).first<<" = ("<<(*it).second.value<<", "<<(*it).second.beginTime<<", "<<(*it).second.endTime<<")"<<endl;
    }
    return true; 
}

bool PwrProgram::getProgramsList(map<string, string>& programs_list) {
    vector<string> programs = hsrv::getlistof("./programs", "");
    vector<string>::iterator it;
    string status;
    
    for(it = programs.begin();it < programs.end();it++) {
        if((*it) == this->current)
            status = "ON";
        else
            status = "OFF";
        
        programs_list.insert(pair<string, string>((*it), status));
    }
    
    return true;
}

string PwrProgram::readattr(string f, string name) {
    ifstream in;
    string word = "";
    size_t found;
    unsigned i;
    char c='\0';
    
    in.open(f.c_str(), ifstream::in);
    
    while(!in.eof() && c != '>') {
        in.get(c);
    }
    
    if(in.eof()) {
        return "*";
    }
    c = '\0';
    
    while(!in.eof()&&c!='>') {
        in.get(c);
        
        if(c!='>'&&c) {
            word+=c;
        }
    }
    
    in.close();
    found = word.find(name);
    
    if(found == string::npos) {
        return "*";
    }
    i = (unsigned)found;
    
    while(i < word.length() && word[i] != '"') {
        i++;
    }
    
    if(i>=word.length()) {
        return "*";
    }
    else {
        return word.substr(i);
    }
}


bool PwrProgram::getProgramsVersions(std::map<std::string,std::string>& programs_versions) {
    vector<string> programs = hsrv::getlistof("./programs", "");
    vector<string>::iterator it;
    string version("");
    
    for(it = programs.begin();it < programs.end();it++) {
        version = readattr("./programs/"+(*it), "version");
        // elimina i doppi apici (temporaneo, migliorare readattr)
        while(hsrv::strReplace(version, "\"", "") == 1);
        programs_versions.insert(pair<string, string>((*it), version));
    }
    
    return true;
}

