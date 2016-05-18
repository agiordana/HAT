//
//  period.cpp
//  Period
//
//  Created by Davide Monfrecola on 12/7/11.
//  Copyright 2011 Università del Piemonte Orientale. All rights reserved.
//

#include "agentlib.h"

using namespace std;

Program::Program(string name) {
	bool res;
	TiXmlNode* pChild;
	TiXmlNode* prg;
	TiXmlElement* pElement;
	string day;
	string month;
	string week;
	string key;
    current = hsrv::tailof(name);
    this->name = name;
	if(hsrv::isfile(name, "xml")) {
		doc = new TiXmlDocument(name.c_str());
		
		res = doc->LoadFile();
		if(res) {
			prg = doc->FirstChild();
			prg = prg->NextSibling();
			pElement = prg->ToElement();
			year = hsrv::getAttrbyName(pElement, "year");
			version = hsrv::getAttrbyName(pElement, "version");
			for ( pChild = prg->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
				month = hsrv::getAttrbyName(pChild->ToElement(), "month_number");
				day = hsrv::getAttrbyName(pChild->ToElement(), "day_number");
				week = hsrv::getAttrbyName(pChild->ToElement(), "weekday");
				key = year+"_"+month+"_"+day+"_"+week;
				(*this)[key] = DayProgram(year, pChild);
			}
		}
	}
}

int Program::load(string fname) {
	bool res;
	string name;
	TiXmlNode* pChild;
	TiXmlNode* prg;
	TiXmlElement* pElement;
	string day;
	string month;
	string week;
	string key;
    current = fname;
    this->name = name;
	name = hsrv::homedir+"/programs/"+fname;
    if (doc!=NULL) {
		map<string, DayProgram>::clear();
        delete doc;
        doc = NULL;
	}
	if(hsrv::isfile(name, "xml")) {
		doc = new TiXmlDocument(name.c_str());
		
		res = doc->LoadFile();
		if(res) {
			prg = doc->FirstChild();
			prg = prg->NextSibling();
			pElement = prg->ToElement();
			year = hsrv::getAttrbyName(pElement, "year");
			version = hsrv::getAttrbyName(pElement, "version");
			for ( pChild = prg->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
				month = hsrv::getAttrbyName(pChild->ToElement(), "month_number");
				day = hsrv::getAttrbyName(pChild->ToElement(), "day_number");
				week = hsrv::getAttrbyName(pChild->ToElement(), "weekday");
				key = year+"_"+month+"_"+day+"_"+week;
				(*this)[key] = DayProgram(year, pChild);
			}
			return 1;
		}
		else return 0;
	}
	return 0;
}

int Program::load() {
	ifstream prg;
	string curprog, prgname, prgpath;
	prgname = hsrv::homedir+"/CURRENTP";
	prg.open(prgname.c_str());
	if(prg!=NULL) prg >> curprog;
	else curprog = "OFF";
	prg.close();
	return load(curprog);
}
	


int Program::printstring(ostringstream& out) {
	out<<"--- Program Begin"<<endl;
	map<string, DayProgram>::iterator it;
	for(it = (*this).begin(); it!=(*this).end(); it++)
		it->second.printstring(out);
	out<<"--- Program End"<<endl;
	return 1;
}

int Program::xmlprint( ostringstream& out, int md ) {
	TiXmlNode* prg;
	DayProgram* dd;
	vector<int> tm;
	vector<int> ctm;
	vector<string> kk;
	int delta;
    
	if (doc == NULL) {
		return 0;
	}
    
	switch(md) {
		case 0:
			prg = doc->FirstChild();
			if(prg) prg = prg->NextSibling();
			if(prg == NULL) return 0;
			out << (*prg);
			return 1;
		case WEEK:
			out << "<?xml version=\"1.0\" ?>" << endl;
			out << "<programs year=\"" << year << "\" version=\"" << version << "\">";
			tm = hsrv::CurrentTime();
			for(unsigned i = 1; i < 8; i++) {
				delta = (int)i - tm[6];
				ctm = hsrv::CurrentTime(delta);
				kk = mkaccesskey(ctm);
				dd = getDay(kk);
				if(dd) dd->xmlprint(out);
			}
			out << "</programs>" << endl;
			return 1;
		default:
			return 1;
	}
	
}

vector<string> Program::mkaccesskey(vector<int>& tm) {
	vector<string> km;
	ostringstream key;
    
	key << tm[5] << "_" << tm[4] << "_" << tm[3] << "_" << tm[6];
	km.insert(km.end(), key.str());
	key.str("");
	key << "*_" << tm[4] << "_" << tm[3] << "_" << tm[6];
	km.insert(km.end(), key.str());
	key.str("");
	key << "*_*_*_" << tm[6];
	km.insert(km.end(), key.str());
	key.str("");
	key << "*_" << tm[4] << "_*_*";
	km.insert(km.end(), key.str());
	key.str("");
	key << "*_*_*_*";
	km.insert(km.end(), key.str());
	
    return km;
}


int Program::install(string prg) {
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
	}
	else { 
           closedir(dirp);
        }
	if(!isFile(offprog)) {
	   cp.open(offprog.c_str());
	   cp << "<?xml version=\"1.0\" ?>" << endl;
	   cp << "<programs name=\"OFF\" version=\"0\"> <day> <periods> <P0 from=\"00:00:00\" to=\"23:59:00\"> <alarms /> </P0> </periods> </day> </programs>" << endl;
	   cp.close();
	   cp.clear();
	}
	if(!isFile(manualprog)) {
	   cp.open(manualprog.c_str());
	   cp << "<?xml version=\"1.0\" ?>" << endl;
	   cp << "<programs name=\"MANUAL\" version=\"0\"><day><periods><P0 from=\"00:00:00\" to=\"23:59:00\"><alarms><alarm name=\"ALL\" /></alarms></P0></periods></day></programs>" << endl;
	   cp.close();
	   cp.clear();
	}
	if(!isFile(onprog)) {
	   cp.open(onprog.c_str());
	   cp << "<?xml version=\"1.0\" ?>" << endl;
	   cp << "<programs  name=\"ON\" version=\"0\"><day><periods><P0 from=\"00:00:00\" to=\"23:59:00\"><alarms><alarm name=\"ALL\" /></alarms></P0></periods></day></programs>" << endl;
	   cp.close();
	   cp.clear();
	}
	if(!isFile(autoprog)) {
	   cp.open(autoprog.c_str());
	   cp << "<?xml version=\"1.0\" ?>" << endl;
	   cp << "<programs  name=\"AUTO\" version=\"0\"><day><periods><P0 from=\"00:00:00\" to=\"23:59:00\"><alarms><alarm name=\"ALL\" /></alarms></P0></periods></day></programs>" << endl;
	   cp.close();
	   cp.clear();
         }
	if(!isFile(currentp)) {
	   cp.open(currentp.c_str());
           if(prg == "")cp << "OFF" << endl;
           else cp<<prg<<endl;
           cp.close();
	   cp.clear();
	   cp.open(previousp.c_str());
	   cp << "NULL" << endl;
	   cp.close();
        }
	return 1;
}

int Program::log(ofstream& out) {
	out << "program" << endl;
    
	return 1;
}

int Program::mkxmlplist(vector<string>* pl, ostringstream& outs) {
	unsigned i;
	plistSort(pl);
    
	for(i=0; i<pl->size(); i++) {
        outs << "<program>" << "<name>" << (*pl)[i] << "</name>";
		if ((*pl)[i] == current) { 
            outs << "<status>" << 1 << "</status>";
        }
		else {
            outs << "<status>" << 0 << "</status>";
        }
		outs << "</program>" << endl;
	}
    
	return 1;
}

bool Program::getProgramsList(map<string, string>& programs_list) {
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

int Program::plistSort(vector<string>* pl) {
	string tmp;
	unsigned k = 0;
	int flag = 0;
    
	for(unsigned i = 0; i < pl->size(); i++) {
        
		if((*pl)[i] == "ON" || (*pl)[i] == "on" || (*pl)[i] == "On") {
			tmp = (*pl)[k];
			(*pl)[k] = (*pl)[i];
			(*pl)[i] = tmp;
			flag = 1;
		}
        
	}
	
    if(flag) { 
        k++;
    }
	flag = 0;
	
    for(unsigned i = 0; i < pl->size(); i++) {
		if((*pl)[i] == "OFF" || (*pl)[i] == "off" || (*pl)[i] == "Off") {
			tmp = (*pl)[k];
			(*pl)[k] = (*pl)[i];
			(*pl)[i] = tmp;
			flag = 1;
		}
	}
    
	if(flag) { 
        k++;
    }
	flag = 0;
    
	for(unsigned i = 0; i < pl->size(); i++) {
		if((*pl)[i] == "DEFAULT" || (*pl)[i] == "default" || (*pl)[i] == "Default") {
			tmp = (*pl)[k];
			(*pl)[k] = (*pl)[i];
			(*pl)[i] = tmp;
		}
	}
    
	return 1;
}

vector<string>* Program::isActive() {
	vector<int> tm = hsrv::CurrentTime();
	vector<string> kk;
    
	kk = mkaccesskey(tm);
	DayProgram* dd = getDay(kk);
    
    if(!dd) { 
        return NULL;
    }
    
	return (*dd).isActive(hsrv::daysecond(tm));
}

string Program::isalarmon() {
    vector<string>* activelist;
    activelist = isActive();
    if(!activelist) return "NO";
    if (activelist->size()==0) {
        delete activelist;
        return "NO";
    }
    else {
        delete activelist;
        return "YES";
    }
}

int Program::prgversions( ostringstream& out) {
	vector<string> prg = hsrv::getlistof("./programs", "");
	
    for(unsigned i = 0; i < prg.size(); i++)
		out << "<program name=\"" << prg[i] << "\" version=" << readattr("./programs/"+prg[i], "version") << " />";
	
    return 1;
}

bool Program::getProgramsVersions(std::map<std::string,std::string>& programs_versions) {
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

string Program::readattr(string f, string name) {
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

bool Program::isFile(string path) {
   fstream in(path.c_str());
   if(in.good()) {
      in.close();
      return true;
   }
   else return false;
}
