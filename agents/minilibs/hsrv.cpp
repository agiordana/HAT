//
//  hsrv.cpp
//  phoneagent
//
//  Created by Davide Monfrecola on 11/15/11.
//  Copyright 2011 Università del Piemonte Orientale. All rights reserved.
//

using namespace std;

#include "agentlib.h"

string hsrv::agenttype = "*";
string hsrv::agentname = "*";
string hsrv::homedir = "";
string hsrv::behaviour = "*";
string hsrv::mqtt = "OFF";
string hsrv::mqtt_domain = "*";
string hsrv::configurator = "*";
string hsrv::configdir = "";
string hsrv::logfile = "LOG";
string hsrv::agentnet = "";
string hsrv::bindir = "";
HorusLogger* hsrv::logger = new HorusLogger("test/path");
ArchiveManager* hsrv::archive;
RpcWaitTable* hsrv::rpctab = new RpcWaitTable();
RoutingMap *hsrv::router;
int hsrv::debug = 0;

map<string,string> hsrv::index;

boost::mutex hsrv::mutexclusion;

// ----------- Class hsrv ---------------------
bool hsrv::load(string path0, string path1) {
    string hname = getHostName();
    configdir = path1;
    bindir = extractBin(string(path0));
    if (!FileManager::isDir(configdir)) {
        return false;
    }
    MParams global("global");
    global.load();
    homedir = global.get("homedir");
    string roothomedir = FileManager::getRoot(homedir);
    if(!FileManager::isDir(roothomedir))
	homedir = global.get("emergency_homedir");
    logfile = global.get("logfile");
    agenttype = global.get("agenttype");
    agentname = global.get("agentname");
    hsrv::strReplace(agentname, "$HOST", hname);
    behaviour = global.get("behaviour");
    mqtt = global.get("mqtt");
    if(mqtt!="ON") mqtt = "OFF";
    mqtt_domain = global.get("mqtt_domain");
    if(mqtt_domain == "") mqtt_domain = "local";
    configurator = global.get("configurator");
    string debugstr = global.get("debug");
    if(debugstr != "") debug = hsrv::a2int(debugstr);
    agentnet = global.get("agentnet");
    if(agentnet=="") agentnet = "agentnet";
    if (behaviour == "") behaviour = "*";

    return true;
}

int hsrv::isfile(string& fname, string type) {
	string fline;
	ifstream in(fname.c_str());
	if(in==NULL) return 0;
	in>>fline;
	in.close();
	if(fline.find(type)==string::npos) return 0;
	else return 1;
}

int hsrv::isfile(string& fname) {
	ifstream in(fname.c_str());
	if(in==NULL) return 0;
	in.close();
	return 1;
}

int hsrv::isdir(string& path) {
	DIR *dirp;
	dirp = opendir(path.c_str());
	if(dirp==NULL) return 0;
	else {
		closedir(dirp);
	}
	return 1;
}


int hsrv::isXmlstring(string& s) {
	size_t found;   
	string xmlsignature = "xml version=";
	
    found = s.find(xmlsignature);
	
    if(found != string::npos) 
        return 1;
	else 
        return 0;
}

int hsrv::print() {
	ostringstream out;
    printstring(out);
	cout << out.str();
	return 1;
}

int hsrv::print(ofstream& outfile) {
	ostringstream out;
	printstring(out);
	outfile << out.str();
	return 1;
}

int hsrv::printstring(ostringstream& out) {
	out << "OK" << endl;
	return 1;
}

string hsrv::tailof(string& path) {
	unsigned i = path.size();
    
	while(i > 0 && path[i] != '/') 
        i--;
	
    if(path[i] == '/') 
        return path.substr(i+1);
	else 
        return path;
}

double hsrv::gettime() {
	timeval tp;
	double mtime;
	if(gettimeofday(&tp, NULL)<0) return 0;
	mtime = mtime = tp.tv_sec+(double)tp.tv_usec/1000000;
	return mtime;
}


vector<int> hsrv::CurrentTime(int d) {
	vector<int> t;
	struct tm* ptm;
	time_t ptime;
    
	ptime = time(NULL) + d*24*3600;
	ptm = localtime(&ptime);
	
    t.insert(t.end(), ptm->tm_sec);
	t.insert(t.end(), ptm->tm_min);
	t.insert(t.end(), ptm->tm_hour);
	t.insert(t.end(), ptm->tm_mday);
	t.insert(t.end(), ptm->tm_mon+1);
	t.insert(t.end(), ptm->tm_year+1900);
	t.insert(t.end(), (ptm->tm_wday==0 ? 7 : ptm->tm_wday));
	t.insert(t.end(), ptm->tm_yday+1);
	t.insert(t.end(), ptm->tm_isdst);
	
    return t;
}

int hsrv::daysecond(vector<int>& tm) {
	return tm[2]*3600+tm[1]*60+tm[0];
}

vector<string> hsrv::getlistof(string dir, string ext) {
	DIR *dirp;
	struct dirent *dp;
	vector<string> dlist;
    
	dirp = opendir(dir.c_str());
	
    if(dirp == NULL) 
        return dlist;
	
    while((dp=readdir(dirp)) != NULL)
		if(matchfile(string(dp->d_name), ext)) 
            dlist.push_back(string(dp->d_name));
	
    closedir(dirp);
	
    return dlist;
}

int hsrv::matchfile(string fname, string ext) {
	if(fname.size() <= ext.size()) 
        return 0;
	
    if(ext == "") {
		
        if(fname==".") 
            return 0;
		if(fname=="..") 
            return 0;
		
        return 1;
	}
	
    return (fname.substr(fname.size() - ext.size(), ext.size()).compare(ext) == 0);
}

const char * hsrv::getIndent( unsigned int numIndents ) {
	static const char * pINDENT = "                                      + ";
	static const unsigned int LENGTH = strlen( pINDENT );
	unsigned int n = numIndents*NUM_INDENTS_PER_SPACE;
	
    if ( n > LENGTH ) 
        n = LENGTH;
	
	return &pINDENT[ LENGTH-n ];
}

const char * hsrv::getIndentAlt( unsigned int numIndents ) {
	static const char * pINDENT = "                                        ";
	static const unsigned int LENGTH = strlen( pINDENT );
	unsigned int n = numIndents*NUM_INDENTS_PER_SPACE;
	
    if ( n > LENGTH ) 
        n = LENGTH;
	
	return &pINDENT[ LENGTH-n ];
}

int hsrv::dump_attribs_to_stdout(TiXmlElement* pElement, unsigned int indent) {
	if ( !pElement ) return 0;
	
	TiXmlAttribute* pAttrib=pElement->FirstAttribute();
	int i=0;
	int ival;
	double dval;
	const char* pIndent = getIndent(indent);
	printf("\n");
	while (pAttrib) {
		printf( "%s%s: value=[%s]", pIndent, pAttrib->Name(), pAttrib->Value());
		if (pAttrib->QueryIntValue(&ival)==TIXML_SUCCESS)    printf( " int=%d", ival);
		if (pAttrib->QueryDoubleValue(&dval)==TIXML_SUCCESS) printf( " d=%1.1f", dval);
		printf( "\n" );
		i++;
		pAttrib=pAttrib->Next();
	}
	return i;
}

void hsrv::dump_to_stdout( TiXmlNode* pParent, unsigned int indent = 0 ) {
	if ( !pParent ) return;
	
	TiXmlNode* pChild;
	TiXmlText* pText;
	int t = pParent->Type();
	printf( "%s", getIndent(indent));
	int num;
	
	switch ( t ) {
		case TiXmlNode::TINYXML_DOCUMENT:
			printf( "Document" );
			break;
			
        case TiXmlNode::TINYXML_ELEMENT:
			printf( "Element [%s]", pParent->Value() );
			num=dump_attribs_to_stdout(pParent->ToElement(), indent+1);
			switch(num) {
				case 0:  printf( " (No attributes)"); break;
				case 1:  printf( "%s1 attribute", getIndentAlt(indent)); break;
				default: printf( "%s%d attributes", getIndentAlt(indent), num); break;
			}
			break;
			
        case TiXmlNode::TINYXML_COMMENT:
			printf( "Comment: [%s]", pParent->Value());
			break;
			
        case TiXmlNode::TINYXML_UNKNOWN:
			printf( "Unknown" );
			break;
        case TiXmlNode::TINYXML_TEXT:
			pText = pParent->ToText();
			printf( "Text: [%s]", pText->Value() );
			break;
			
        case TiXmlNode::TINYXML_DECLARATION:
			printf( "Declaration" );
			break;
        default:
			break;
	}
	printf( "\n" );
	for ( pChild = pParent->FirstChild(); pChild != 0; pChild = pChild->NextSibling())
	{
		dump_to_stdout( pChild, indent+1 );
	}
}

string hsrv::getAttrbyName(TiXmlElement* pElement, string name) {
	
    if(!pElement) 
        return "*";
	
    TiXmlAttribute* pAttrib=pElement->FirstAttribute();
	
    while (pAttrib && pAttrib->Name() != name) 
        pAttrib = pAttrib->Next();
	
    if(pAttrib) 
        return pAttrib->Value();
	else 
        return "*";
}

string hsrv::getParambyName(TiXmlNode* prg, string name) {
	TiXmlNode* pChild;
	string tmp = "*";
	
    if(!prg) 
        return tmp;
	for ( pChild = prg->FirstChild(); pChild != 0 && tmp=="*"; pChild = pChild->NextSibling())
		tmp = getAttrbyName(pChild->ToElement(), name);
	return tmp;
}

int hsrv::string2second(string& s) {
	string hh, min, ss;
	int sec = 0;
	unsigned i;
	
    hh="";
	for(i=0; i<s.size() && s[i]!=':' && s[i]!='.'; i++) 
        hh += s[i];
	
    if(hh!="") 
        sec += (atoi(hh.c_str()))*3600;
	if(i==s.size()) 
        return sec;
	i++;
    
	min = "";
	for(; i<s.size() && s[i]!=':' && s[i]!='.'; i++) 
        min += s[i];
	if(min!="") 
        sec += (atoi(min.c_str()))*60;
	if(i==s.size()) 
        return sec;
	i++;
    
	ss = "";
	for(; i<s.size() && s[i]!=':' && s[i]!='.'; i++) 
        ss += s[i];
	if(ss!="") 
        sec += (atoi(ss.c_str()));
	return sec;
}

int hsrv::logMessage(string m) {
	ofstream out;
	out.open(logfile.c_str(), ios::app);
	out.setf(ios::fixed, ios::floatfield);
	out.setf(ios::showpoint);
	out<<m<<" "<<hsrv::getasciitimefull()<<endl;
	out.close();
	return 1;
}

string hsrv::getasciitimeday() {
	timeval tp;
	string tmp;
    
	if(gettimeofday(&tp, NULL) <0) 
        return "";
	
    tmp = string(ctime(&tp.tv_sec));
	
    return tmp.substr(11,8);
}

string hsrv::getasciitimefull() {
	timeval tp;
	string tmp;
    
	if(gettimeofday(&tp, NULL) <0) 
        return "";
	
    tmp = string(ctime(&tp.tv_sec));
	
    for(unsigned i=0; i<tmp.size(); i++) 
        if(tmp[i]==' '||tmp[i]=='\t') 
            tmp[i]='_';
	
    return tmp;
}

string hsrv::getasciitimecompact() {
	char tmp[MAXB];
	timeval tp;
	struct tm* tc;
	if(gettimeofday(&tp, NULL) <0) return "";
	tc = localtime(&tp.tv_sec);
	sprintf(tmp,"%02d%02d%02d%02d%02d%02d", (tc->tm_year+1900)%100, tc->tm_mon+1, tc->tm_mday, tc->tm_hour, tc->tm_min, tc->tm_sec);
	return string(tmp);
}

string hsrv::mySQLdate(string date) {
	char tmp[MAXB];
	timeval tp;
	struct tm* tc;
    
    if(date != "") {
		while(hsrv::strReplace(date, "_", "-") == 1);
		return date;
	}
	
	if(gettimeofday(&tp, NULL) < 0) 
        return "";
	
    tc = localtime(&tp.tv_sec);
	sprintf(tmp,"%04d-%02d-%02d", (tc->tm_year+1900), tc->tm_mon+1, tc->tm_mday);
	
    return string(tmp);
}

int hsrv::strReplace(string& s, string dst, string src) {
	size_t pos;
	
    if((pos = s.find(dst))!=string::npos) {
        s.replace(pos, dst.size(), src);
        return 1;
	}
	else 
        return 0;
}

int hsrv::upcase(string& a) {
	for(unsigned i=0; i<a.size(); i++) 
        if(a[i]<='z' && a[i]>='a') 
            a[i] = 'A'+a[i]-'a';
	return 1;
}

string hsrv::double2a(double n) {
	stringstream tmp;
    tmp.precision(5);
    tmp.setf(ios::fixed,ios::floatfield);
	tmp << n;
    string s(tmp.str());
    
    return s;
}

string hsrv::long2a(long n) {
	stringstream tmp;
	tmp << n;
    string s(tmp.str());
    
    return s;
}

string hsrv::int2a(int n) {
	stringstream tmp;
	tmp << n;
    string s(tmp.str());
    
    return s;
}

string hsrv::unsigned2a(unsigned n) {
	stringstream tmp;
	tmp << n;
    string s(tmp.str());
    
    return s;
}

double hsrv::a2double(string& n) {
	double d;
    sscanf(n.c_str(), "%lf", &d);
	
    return d;
}

long hsrv::a2long(string& n) {
	long d;
	
    sscanf(n.c_str(), "%ld", &d);
	
    return d;
}

int hsrv::a2int(string& n) {
	int d;
	
    sscanf(n.c_str(), "%d", &d);
	
    return d;
}

unsigned hsrv::a2unsigned(string& n) {
	unsigned d;
	
    sscanf(n.c_str(), "%u", &d);
	
    return d;
} 

int hsrv::isInString(std::string& s, std::string signature) {
        size_t found = s.find(signature);
        return (found != string::npos);
}


string hsrv::readFile(string& name) {
	ifstream in;
	string r="";
	string tmp;
    
	in.open(name.c_str());
	if(in == NULL) 
        return r;
    
	while(!in.eof()) {
		in >> tmp;
        
		if(!in.eof()) 
            r = r + tmp;
	}
	in.close();
	return r;
}

string hsrv::getday(time_t delta) {
	char tmp[MAXB];
	timeval tp;
	struct tm* tc;
	
	if(gettimeofday(&tp, NULL) <0) return "";
	tp.tv_sec += delta;

	tc = localtime(&tp.tv_sec);
	sprintf(tmp,"%04d_%02d_%02d", (tc->tm_year+1900), tc->tm_mon+1, tc->tm_mday);
	
	return string(tmp);
}

string hsrv::extractBin(string agentname) {
	unsigned i;
	
	if(agentname == "") 
		return "";
	
	for(i = agentname.size()-1; i > 0 && agentname[i] != '/'; i--);
	
	if(i > 0) 
		return agentname.substr(0,i);
	else 
		return "";
}

std::string hsrv::cmdExec(string command) {
    FILE* in = NULL;
    char rbuf[MAXB];
    size_t result = 0;
    int n = 0, i;

    if(debug>1) {
	cout<<command<<endl;
        return "done";
    }
    in = popen(command.c_str(), "r");
    
    if(in == NULL){
        hsrv::logger->error("Error executing command -> " + command);
    }
    else {
        result = fread(rbuf, 1, (sizeof rbuf)-1, in);       
        n = pclose(in);
    }
    // pulisce la stringa da eventuali caratteri non necessari
    for(i = (int)result;i < MAXB; i++) {
        rbuf[i] = '\0';
    }

	string output(rbuf);
    
    return output;
}

pid_t hsrv::getprocessid(string& cmd) {
	size_t pos;
	size_t j;
	pid_t pid;
	string res = cmdExec("ps ax");
	pos = res.find(cmd);
	if(pos == string::npos) return 0;
	for(j = pos; res[j]!='\n' && j>0; j--);
	sscanf(res.substr(j, pos-j).c_str(), "%d", &pid);
	return pid;
}

int hsrv::convertSpecialChar(string& s) {
	string tmp="";
	for (unsigned i=0; i<s.length(); i++) {
		if (i<s.length()-2&&s[i]=='\\'&&s[i+1]=='r') {
			tmp += '\r';
			i++;
		}
		else if(s[i]=='\\'&&s[i+1]=='n') {
			tmp += '\n';
			i++;
		}
		else tmp+=s[i];
	}
	s = tmp;
	return 1;
}

vector<string> hsrv::split(string s, char separator, int limit) {
    int starting_char = 0;
    int split_number = 0;
    vector<string> splitted;
    
    for(int i = 0;i < s.size();i++) {
        if((s[i]) == separator ){
            splitted.push_back(string(s.substr(starting_char, (i - starting_char))));
            starting_char = i+1;
            split_number++;
            
            if(split_number == limit) {
                return splitted;
            }
        }
        
    }
    
    splitted.push_back(string(s.substr(starting_char)));
    
    return splitted;
}

vector<string>* hsrv::getdirlist(string dir, string ext) {
	DIR *dirp;
	struct dirent *dp;
	vector<string>* dlist;
    
	dirp = opendir(dir.c_str());
    
    if(dirp == NULL) 
        return NULL;
	
    dlist = new vector<string>;
	
    while((dp=readdir(dirp)) != NULL)
		if(hsrv::matchextension(string(dp->d_name), ext)) dlist->push_back(string(dp->d_name));
	closedir(dirp);
	
    return dlist;
}

int hsrv::matchextension(string fname, string ext) {
	
    if(fname.size() <= ext.size()) 
        return 0;
	
    if(ext == "") {
		if(fname == ".") return 0;
		if(fname == "..") return 0;
		return 1;
	}
	
    return (fname.substr(fname.size() - ext.size(), ext.size()).compare(ext) == 0);
}

bool hsrv::saveFile(string& ty, string& name, string& body) {
	ofstream out;
	string path = "./"+ty+"/"+name;
	int flag = 0;
    
	out.open(path.c_str());
	
    if(out == NULL) 
        return false;
	
    for(unsigned i = 0; i < body.size(); i++) {
		if(body[i] == ']') 
            flag=1;
		
        if(body[i] == '\\') {
			out << endl;
			if(flag) {
				out << "   ";
				flag = 0;
			}
		}
		else 
            out << body[i];
	}
	out.close();
	return true;
}

bool hsrv::deleteFile(string& ty, string& name) {
	string path = hsrv::homedir + "/"+ty+"/"+name;
	ifstream in;
    
	in.open(path.c_str());
	
    if(in == NULL) 
        return 0;
	
    in.close();
	unlink(path.c_str());
	
    return 1;
}

string hsrv::filterChar(string src) {
   string answer = "";
   size_t i;
   if(src.size()<1) return answer;
   for(i=0; i<src.size()-1; i++)
      if(src[i] =='\\' && src[i+1]=='r') answer += '\r';
      else if(src[i] =='\\' && src[i+1]=='n') { answer += '\n'; ++i; }
      else if(src[i] =='\\' && src[i+1]=='t') { answer += '\t'; ++i; }
      else if(src[i] =='\\' && src[i+1]=='\\') { answer += '\\'; ++i; }
      else answer += src[i];
   if(i<src.size()) answer += src[i];
   return answer;
}

bool hsrv::saveItemList(string name, string path, vector<string>& items) {
   ofstream out("tmp.xml");
   out  << "<set type=\""+ name+ "\">"<<endl;
   for(size_t i=0; i<items.size(); i++) {
      out<<"<attribute name=\""<<items[i]<<"\" value=\"ON\"/>"<<endl;
   }
   out << "</set>"<<endl;
   out.close();
   string pathname = hsrv::homedir +"/tmp.xml";
   string localurl = path + "/" + name + ".xml";
   hsrv::archive->addResource(localurl, pathname, true);
   FileManager::deleteFile(hsrv::homedir, "tmp.xml");
   return true;
}

bool hsrv::saveItemList(string name, string path, vector<pair<string,string> >& items) {
   ofstream out("tmp.xml");
   out  << "<set type=\""+ name+ "\">"<<endl;
   for(size_t i=0; i<items.size(); i++) {
      out<<"<attribute name=\""<<items[i].first<<"\" value=\""+items[i].second + "\"/>"<<endl;
   }
   out << "</set>"<<endl;
   out.close();
   string pathname = hsrv::homedir +"/tmp.xml";
   string localurl = path + "/" + name + ".xml";
   hsrv::archive->addResource(localurl, pathname, true);
   FileManager::deleteFile(hsrv::homedir, "tmp.xml");
   return true;
}

string hsrv::xmlIndex() {
   stringstream page;
   boost::unique_lock<boost::mutex> lock(hsrv::mutexclusion);
   map<string,string>::iterator it;
   page<<"<?xml version=\"1.0\" ?>\n";
   page<<"<list name=\"index\">\n";
   for(it=index.begin(); it!=index.end(); it++) {
      if(it->second.find("=") == string::npos) page<<"<item name=\""<<it->first<<"\" value=\""<<it->second<<"\"/>\n";
      else page<<"<item name=\""<<it->first<<"\" "<<it->second<<"/>\n";
   }
   page<<"</list>\n";
   return page.str();
}

string hsrv::xmlIndex(string name) {
   boost::unique_lock<boost::mutex> lock(hsrv::mutexclusion);
   if(index.count(name) == 0 ) return "Not Found";
   stringstream item;
   if(index[name].find("=") == string::npos) item << "<item name=\"" <<name << "\" value=\""<<index[name]<<"\"/>\n";
     else item << "<item name=\"" <<name << "\" "<<index[name]<<"/>\n";
   return item.str();
} 

string hsrv::jsonIndex() {
   stringstream page;
   map<string,string>::iterator it;
   boost::unique_lock<boost::mutex> lock(hsrv::mutexclusion);
   page<<"{\"list\": [\n";
   for(it=index.begin(); it!=index.end(); it++)
      if(it == index.begin()) page<<"   {\""<<it->first<<"\":" << jsonItem(it->second) << "}";
      else page<<",\n   {\""<<it->first<<"\":"<<jsonItem(it->second)<<"}";
   page<<"  ]\n";
   page<<"}\n";
   return page.str();
}

string hsrv::jsonItem(string& src) {
   string item;
   if(src.find("=") == string::npos) item = "\"" + src +"\"";
   else {
	NameList itemlist;
	NameList itempair;
	itemlist.init(src,' ');
        for(size_t i=0; i<itemlist.size(); i++) 
	  if(itemlist[i] !="") {
	   itempair.init(itemlist[i],'=');
	   if(itempair.size() == 2) {
	      if(i == 0) item += "{\"";
	        else item += ",\"";
	      item += itempair[0];
	      item += "\":";
	      item += itempair[1];
	   }
	 }
	item += "}";
   }
   return item;
}

string hsrv::jsonIndex(string name) {
   boost::unique_lock<boost::mutex> lock(hsrv::mutexclusion);
   if(index.count(name) == 0 ) return "Not Found";
   stringstream item;
   item << "{\"" <<name << "\":"<<jsonItem(index[name])<<"}\n";
   return item.str();
}

bool hsrv::indexClear() {
   boost::unique_lock<boost::mutex> lock(hsrv::mutexclusion);
   index.clear();
   return true;
}

bool hsrv::indexClear(string name) {
   map<string,string>::iterator it;
   boost::unique_lock<boost::mutex> lock(hsrv::mutexclusion);
   if(index.count(name)==0) return false;
   it = index.find(name);
   index.erase(it);
   return true;
}

bool hsrv::publish(string name, string value) {
   boost::unique_lock<boost::mutex> lock(hsrv::mutexclusion);
   index[name] = value;
   return true;
}

string hsrv::getDynamicPage(string uri) {
   NameList name;
   NameList item;
   name.init(uri, '/');
   if(name.size()<1) return "Forbidden";
   item.init(name[name.size()-1], '.');
   if(item.size()<2) return "Not Found";
   if(item[0]=="DynamicPage" && item[1] == "xml") return xmlIndex();
   else if(item[0]=="DynamicPage" && item[1] == "json") return jsonIndex();
   else if(item[0]!="DynamicPage" && item[1] == "xml") return xmlIndex(item[0]);
   else if(item[0]!="DynamicPage" && item[1] == "json") return jsonIndex(item[0]);
   else return "Not Found";
}

string hsrv::getHostName() {
  string hostname = hsrv::cmdExec("/bin/hostname");
  string answer = "";
  for(size_t i=0; i<hostname.size(); i++)
    if(hostname[i] != '\n' && hostname[i] != '\r') answer += hostname[i];
  return answer;
}

bool hsrv::checkConfigureEnabled() {
  double ctime = hsrv::gettime();
  if(hsrv::configurator == "ON") return true;
  string cmd = "cat ";
  cmd += CONFENABLED;
  string conftime = hsrv::cmdExec(cmd);
  hsrv::logger->info(conftime);
  double cftime = hsrv::a2double(conftime);
  if(cftime + CFWINDOW > ctime) return true;
  else return false;
}

bool hsrv::rpcNotify(string tag, string field, string value) {
   if(hsrv::rpctab == NULL) return false;
   MMessage m("rpc","answer");
   m.add("answer_tag", tag);
   m.add(field, value);
   hsrv::rpctab->signal(m);
   return true;
}

int hsrv::parseLine(char* line){
    // This assumes that a digit will be found and the line ends in " Kb".
    int i = strlen(line);
    const char* p = line;
    while (*p <'0' || *p > '9') p++;
    line[i-3] = '\0';
    i = atoi(p);
    return i;
}

int hsrv::getMemorySize() {//Note: this value is in KB!
    FILE* file = fopen("/proc/self/status", "r");
    int result = -1;
    char line[128];

    while (fgets(line, 128, file) != NULL){
        if (strncmp(line, "VmSize:", 7) == 0){
            result = parseLine(line);
            break;
        }
    }
    fclose(file);
    return result;
}

