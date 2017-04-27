/*
 *  agentlib.cpp
 *  phoneagent
 *
 *  Created by Attilio Giordana on 10/27/11.
 *  Copyright 2011 Università del Piemonte Orientale. All rights reserved.
 *
 */


using namespace std;

#include "agentlib.h"

std::map<std::string,MMessage> AgentConf::urlindex;
boost::mutex AgentConf::mutexclusion;
int AgentConf::status;

//------------------------------------ Class AgentConf ------------------------------------------

AgentConf::AgentConf() {
    myerr=0; 
    maxfail=0; 
    callmade=0; 
    currentday = hsrv::getday();
    AgentConf::status=OFF;
    urlindex ["/status"] = getStatus("status");
    urlindex["/status.xml"] = getStatus("status.xml");
    urlindex["/AYA"] = getIAA();
}

int AgentConf::isNewDay() {
	string tmp = hsrv::getday();
	return currentday != tmp;
}

int AgentConf::mklock() {
	ofstream out;
	ifstream in;
	pid_t pid;
	string lockname = hsrv::homedir+"/.lock";
	    
    in.open(lockname.c_str());
	if(in != NULL) 
        return -1;
	in.close();
	pid = getpid();
	out.open(lockname.c_str());
	out << pid << endl;
	out.close();
    
	return 0;
}

pid_t AgentConf::getlockvalue() {
	pid_t pid;
	ifstream in;
	string lockname = hsrv::homedir+"/.lock";
	
    in.open(lockname.c_str());
	if(in != NULL) 
        return 0;
	in >> pid;
	in.close();
    
	return pid;
}

int AgentConf::saveFile(string& ty, string& name, string& body, bool raw) {
	ofstream out;
	string path = "./"+ty+"/"+name;
	int flag = 0;
    
	out.open(path.c_str());
	
    if(out == NULL) return 0;
    if(!raw) {
        for(unsigned i = 0; i < body.size(); i++) {
		if(body[i] == ']') flag=1;
		if(body[i] == '\\') {
			out << endl;
			if(flag) {
				out << "   ";
				flag = 0;
			}
		}
		else out << body[i];
	}
    }
    else {
	for(unsigned i = 0; i < body.size(); i++) out << body[i];
    }
    out.close();
    return 1;
}

int AgentConf::deleteFile(string& ty, string& name) {
	string path="./"+ty+"/"+name;
	ifstream in;
    
	in.open(path.c_str());
	
    if(in == NULL) 
        return 0;
	
    in.close();
	unlink(path.c_str());
	
    return 1;
}

int AgentConf::matchextension(string fname, string ext) {
	
    if(fname.size() <= ext.size()) 
        return 0;
	
    if(ext == "") {
		if(fname == ".") return 0;
		if(fname == "..") return 0;
		return 1;
	}
	
    return (fname.substr(fname.size() - ext.size(), ext.size()).compare(ext) == 0);
}

vector<string>* AgentConf::getdirlist(string dir, string ext) {
	DIR *dirp;
	struct dirent *dp;
	vector<string>* dlist;
    
	dirp = opendir(dir.c_str());
    
    if(dirp == NULL) 
        return NULL;
	
    dlist = new vector<string>;
	
    while((dp=readdir(dirp)) != NULL)
		if(matchextension(string(dp->d_name), ext)) dlist->push_back(string(dp->d_name));
	closedir(dirp);
	
    return dlist;
}

int AgentConf::getFilelist(string path, vector<string>* flist) {
	DIR *dirp;
	struct dirent *dp;
	string fname;
    
	dirp = opendir(path.c_str());
	
    if(dirp == NULL) 
        return 0;
	
    while((dp = readdir(dirp)) != NULL) {
		fname = string(dp->d_name);
		if(fname != "." && fname != "..") 
            flist->push_back(fname);
	}
    
	closedir(dirp);
	
    if(flist->size() == 0) 
        return 0;
	
    sort(flist->begin(), flist->end());
	return 1;
}



MMessage AgentConf::getExecM(MMessage* m) {
	MMessage answer;
    if(m->getString("url")=="/") return mkHomePage(m);
    return getPage(m);
}


MMessage AgentConf::mkUnknownM(MMessage* m) {
	MMessage answer("answer");
	string r = m->getString("language");
    string method = m->getString("method");
    string error;
	if(method == "get" || method == "GET" || method == "post" || method == "POST") error = "method";
        else error = "url";
    if(r == "xml") {
		answer.add("language", r);
        answer.add(error,"unknown");
        }
	else {
		answer.add("language", "html");
		answer.add("body", "unknown");
	}    
	return answer;
}

int AgentConf::sethomedir() {
	return (chdir(hsrv::homedir.c_str()) >= 0);
}

int AgentConf::saveCurrent(string file, string content) {
	ofstream out;
	string currentp = hsrv::homedir + "/" + file;
    
	out.open(currentp.c_str());
	out << content << endl;
	out.close();
	return 1;
} 

int AgentConf::setStatus() {
	string path = hsrv::homedir+"/CURRENTSTATUS";
	ifstream in;
	string st;
	if(hsrv::isfile(path)) {
		in.open(path.c_str());
		in>>st;
		in.close();
		if(st=="ON") AgentConf::status=ON;
		else AgentConf::status=OFF;
	}
	else {
		AgentConf::status=ON;
		saveCurrent("CURRENTSTATUS", "ON");
	}
	urlindex ["/status"] = AgentConf::getStatus("status");
	urlindex["/status.xml"] = AgentConf::getStatus("status.xml");
	return AgentConf::status;
}

	
int AgentConf::setStatus(int st) {
	AgentConf::status=st;
	saveCurrent("CURRENTSTATUS", (AgentConf::status==1 ? "ON" : "OFF"));
	urlindex ["/status"] = AgentConf::getStatus("status");
	urlindex["/status.xml"] = AgentConf::getStatus("status.xml");
	return AgentConf::status;
}

MMessage AgentConf::getStatus(string url) {
	MMessage answer("answer");
    string language;
	size_t found = url.find(".xml");
    
	if (found != string::npos) {
		language = "xml";
	}
    
	if(language == "xml") 
        answer.add("language", "xml");
    else 
        answer.add("language", "html");
	
    answer.add("status", AgentConf::status);
//    answer.add("myerr", myerr);
//    answer.add("maxfail", maxfail);
//    answer.add("currentday", currentday);
//    answer.add("callmade", callmade);
    return answer;
}

MMessage AgentConf::getIAA() {
	MMessage answer("answer");

    
    answer.add("body", "IAA");
    return answer;
}

MMessage AgentConf::mkHomePage(MMessage* m) {
    MMessage answer("answer");
    stringstream page;
    string buffer;
    string pagename = hsrv::configdir+"/index.html";
    ifstream pagefile(pagename.c_str());
    while (pagefile!=NULL && !pagefile.eof()) {
        pagefile >> buffer;
        page <<buffer<<" ";
    }
    if(page.str().size()<2) page<<"Not available";
    answer.add("language", "html");
    answer.add("body", page.str());
    return answer;
}


MMessage AgentConf::getPage(MMessage* m) {
    boost::unique_lock<boost::mutex> lock(AgentConf::mutexclusion);
    string url = m->getString("url");
    if (url=="/index" || url=="/index.xml" || url=="/index.html" || url=="/index.htm") {
        return mkIndexPage(url);
    }
    if(urlindex.count(url) > 0) {
        return urlindex[url];
    }
    else return mkUnknownM(m);
}

bool AgentConf::pageRegister(string url, MMessage& m){
    boost::unique_lock<boost::mutex> lock(AgentConf::mutexclusion);
    AgentConf::urlindex[url] = m;
    return true;
}

MMessage AgentConf::mkIndexPage(string url){
    map<string,MMessage>::iterator it;
    MMessage page("index");
    for (it = AgentConf::urlindex.begin(); it!= AgentConf::urlindex.end(); it++) {
        page.add(it->first, "ON");
    }
    if (url == "/index.xml") {
        page.add("language", "xml");
    }
    else page.add("language", "html");
    return page;
}

MMessage AgentConf::postExecM(MMessage* m) {
    MMessage answer("answer");
	MMessage msg;
    string dummy = "*";
	string ty = m->mtype;
    string subty = m->msubtype;
	string cmdname = m->getString("cmdname");
	string language = m->getString("language");
    if(language == "xml") {
        answer.add("language", "xml");
    }
    else {
        answer.add("language", "html");
    }
	if (SubjectSet::check_subject(ty, subty)||SubjectSet::check_subject(ty,dummy)) {
		msg = *m;
        SubjectSet::notify(msg);
        
		answer.add("result","done");
		return answer;
	}
    if(ty == "upload") {
	string filetype = m->getString("filetype");
	string filename = m->getString("filename");
	string filebody = m->getString("filebody");
        if(m->getString("encoding") == "hex" || m->getString("encoding")=="hexr") filebody = hex2ascii(filebody);
        else while(hsrv::strReplace(filebody, "'", "\"") == 1);
	if(saveFile(filetype, filename, filebody, m->getString("encoding")=="hexr")) answer.add("result","done");
        else answer.add("error", "unknown");
    }
    else if(ty == "delete") {
		string filetype = m->getString("filetype");
		string filename = m->getString("filename");
		if(deleteFile(filetype, filename))
			answer.add("result","done");
		else 
            answer.add("error", "unknown");
	}
	else if(ty == "cmd" && m->getString("cmdname") == "setstatus") {
		if (m->getString("status") == "ON") {
			setStatus(ON);
			answer.add("result","done");
		}
		else {
			if(m->getString("status") == "OFF") {
				setStatus(OFF);
				answer.add("result","done");
			}
			else {
				answer.add("error", "unknown");
			}
		}
	}
    else
        answer.add("error", "unknown");
	
    return answer;
}

bool AgentConf::start_rest_server() {
    rest_server = new RestService();
    rest_server->start();
    return true;
}


bool AgentConf::start_server() {
    MMessage* req;
	MMessage answer;
    string r;
        
    this->http_server = new HttpServer();
    
    if(this->http_server->setUp() < 0) {
        hsrv::logger->fatal("Randez-vous socket failed!", __FILE__, __FUNCTION__, __LINE__);
		return false;
    }
    
    while(1) {
		req = this->http_server->getRequestM();
            if(req) {
			if(_DEBUG_) {
                r = req->xmlEncode(1);
                hsrv::logger->info(r, __FILE__, __FUNCTION__, __LINE__);
            }
			
			if(req->getString("method") == "get" || req->getString("method") == "GET")
				answer = this->getExecM(req);
			else {
				if(req->getString("method") == "post" || req->getString("method") == "POST")
					answer = this->postExecM(req);
				else
					answer = this->mkUnknownM(req);
			}
			this->http_server->sendAnswerM(&answer);
			delete req;
			req = NULL;
		}
	}
    return true;
}

unsigned AgentConf::hexCharDecode(unsigned c) {
    unsigned v=0;
    switch(c) {
        case '0': v = 0;
            break;
        case '1': v = 1;
            break;
        case '2': v = 2;
            break;
        case '3': v = 3;
            break;
        case '4': v = 4;
            break;
        case '5': v = 5;
            break;
        case '6': v = 6;
            break;
        case '7': v = 7;
            break;
        case '8': v = 8;
            break;
        case '9': v = 9;
            break;
        case 'A':
        case 'a': v = 10;
            break;
        case 'B':
        case 'b': v = 11;
            break;
        case 'C':
        case 'c': v = 12;
            break;
        case 'D':
        case 'd': v = 13;
            break;
        case 'E':
        case 'e': v = 14;
            break;
        case 'F':
        case 'f': v = 15;
    }
    return v;
}

string AgentConf::hex2ascii(string str) {
    string tmp="";
    for(size_t i=0; i<str.size()-1; i=i+2) {
        tmp += (char) hexCharDecode(str[i])*16+hexCharDecode(str[i+1]);
    }
    return tmp;
}

bool AgentConf::install() {
   if(!FileManager::makeDir(hsrv::homedir, true)) return false;
   MParams global("global");
   global.load();
   for(size_t i = 0; i< global.size(); i++) {
      if(global[i].category == "configdir"||global[i].category == "homedir") {
	  NameList path;
	  path.init(global[i].value,'/');
          string cpath;
	  if(global[i].category == "configdir") cpath = hsrv::configdir;
	  else cpath = hsrv::homedir;
	  for(size_t j=0; j<path.size(); j++) {
	     cpath += "/";
	     cpath += path[i];
	     if(!FileManager::makeDir(cpath, true)) return false;
	  }
       }
   }
   return true;
}
