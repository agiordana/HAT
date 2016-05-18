#include "agentlib.h"
#include <ctime>

using namespace std;

boost::mutex ArchiveManager::mutexLock;
std::vector<int> ArchiveManager::MDAYS = std::vector<int>(12, 31);

string ArchiveManager::getParentDirectory(string filename) {
    unsigned pos;
  
    pos = filename.find_last_of('/');
    if (pos != std::string::npos) {
        return filename.substr(0, pos);
    } else {
        return ""; 
    }    
} 



bool ArchiveManager::isDate(string date, char separator) {
    NameList d1;
    int dd, mm;
    
    d1.init(date, separator);
    if (d1.empty() or d1.size() != 3) {
        return false;
    }
   
    mm = atoi(d1[1].c_str());
    dd = atoi(d1[2].c_str());
    
    if ( mm > 0 and mm <= 12 and dd > 0 and dd <= 31) {
        return true;
    } else {
        return false;
    }
}


string ArchiveManager::getCurrentDay(string &day, string &month, string &year) {
    time_t t;
    struct tm *lt;
    stringstream tmp;
    stringstream today;

    t = time(NULL);
    lt = localtime(&t);
    today.str("");

    tmp.str("");
    tmp << lt->tm_year+1900;
    year = tmp.str();    
    today << lt->tm_year+1900;

    tmp.str("");
    if (lt->tm_mon < 10) {
        today << 0;
        tmp << 0;
    }
    tmp << lt->tm_mon+1;
    month = tmp.str();    
    today << lt->tm_mon+1;

    tmp.str("");
    if (lt->tm_mday < 10) {
        today << 0;
        tmp << 0;
    }
    today << lt->tm_mday;
    tmp << lt->tm_mday;
    day = tmp.str();    

    return today.str();
}


int ArchiveManager::getDateDifference(string date1, string date2, char separator) {
    NameList d1, d2;
    
    d1.init(date1, separator);
    d2.init(date2, separator);
    
    boost::gregorian::date now(boost::gregorian::date(atoi(d1[0].c_str()), 
                                                    atoi(d1[1].c_str()),
                                                    atoi(d1[2].c_str())));
    
    boost::gregorian::date then(boost::gregorian::date(atoi(d2[0].c_str()), 
                                                    atoi(d2[1].c_str()), 
                                                    atoi(d2[2].c_str())));
    
    boost::gregorian::date_period diff(then, now);
    
    return diff.length().days();
}


std::ostream& operator<<(std::ostream& os, const RootDescriptor &D) {
    os << "tw: " << D.temporalWindow << ", visibility: ";
    for (int i=0; i<(int)D.visibility.size(); ++i) {
        os << D.visibility[i] << " "; 
    }
    
    return os;
}



bool ArchiveManager::checkPermission(vector<string> url, int mode) {
    bool permission = false;
    string archiveName = url.front();
    RootDescriptor  d;
    string subdir = ".";
    
    
    if ((url.size() == 1) and (mode==ArchiveManager::READ)) {
        return true;
    
    } else if ((url.size() == 1) and (mode==ArchiveManager::WRITE)) {
        return false;
    
    } else {
        subdir = url[1];
    }
    
    if (accessInfo.count(archiveName) == 0) {
        return false; 
    }
    
    d = accessInfo.at(archiveName);
    if ( !fullAccess(archiveName) ) { //check if subdir is accessible
        vector<string>::iterator it;
        it = std::find(d.visibility.begin(), d.visibility.end(), subdir);
        if (it != d.visibility.end()) {
            permission = true;
        }
        
    } else {
        permission = true;
    }
    
    return permission;
}



bool ArchiveManager::init() {
    setof<MAttribute> roots, confs;
    NameList subdir;
    
    ArchiveManager::MDAYS[1] = 28; //february
    ArchiveManager::MDAYS[3] = 30; //april
    ArchiveManager::MDAYS[5] = 30; //june
    ArchiveManager::MDAYS[8] = 30; //september
    ArchiveManager::MDAYS[10] = 30; //november; 

    if ( !MParams::load() ) {
        std::cout << "Failed to load configuration file" << std::endl;
        return false;
    }
    
    
    roots = MParams::extractCategory("root");
    for (int i=0; i<(int)roots.size(); ++i) {
        string rootdir;
        rootdir = hsrv::homedir +"/"+roots[i].name;
        if (!FileManager::isDir(rootdir)) {
            FileManager::makeDir(rootdir, true);  
	}
        
        RootDescriptor descriptor;
        string w = MParams::get(roots[i].name, "history");
        descriptor.temporalWindow = atoi(w.c_str());
	descriptor.prefix = hsrv::homedir;

        subdir.init(MParams::get(roots[i].name, "root"));
        if (!subdir.member("*")) {
            for (int j = 0; j < (int) subdir.size(); ++j) {
                string subpath = rootdir + "/" + subdir[j];
                if (!FileManager::isDir(subpath)) {                 
		   FileManager::makeDir(subpath, true);
                }

                descriptor.visibility.push_back(subdir[j]);
            }
        } else {
            descriptor.visibility.push_back("*");
        }
        
        pair<string,RootDescriptor> p(roots[i].name, descriptor);
        accessInfo.insert(p);
    }
    
    confs = MParams::extractCategory("conf");
    for (int i=0; i<(int)confs.size(); ++i) {
        string rootdir = hsrv::configdir+"/"+confs[i].name;
        if (!FileManager::isDir(rootdir)) {
            FileManager::makeDir(rootdir, true);  
	}
        
        RootDescriptor descriptor;
        string w = MParams::get(confs[i].name, "history");
        descriptor.temporalWindow = atoi(w.c_str());
	descriptor.prefix = hsrv::configdir;

        subdir.init(MParams::get(confs[i].name, "conf"));
        if (!subdir.member("*")) {
            for (int j = 0; j < (int) subdir.size(); ++j) {
                string subpath = rootdir + "/" + subdir[j];
                if (!FileManager::isDir(subpath)) {                 
		   FileManager::makeDir(subpath, true);
                }

                descriptor.visibility.push_back(subdir[j]);
            }
        } else {
            descriptor.visibility.push_back("*");
        }
	
	pair<string,RootDescriptor> p(confs[i].name, descriptor);
        accessInfo.insert(p);
    }

    methods.push_back("first");
    methods.push_back("last");
    methods.push_back("all");

#ifdef DEBUG
    map<string,RootDescriptor>::iterator it;
    for (it=accessInfo.begin(); it!=accessInfo.end(); ++it) {
        std::cout << it->first << ": " << it->second << std::endl;
    }
#endif
    
    return true;
} 


std::string ArchiveManager::getRootPath(std::string uri) {
    NameList nl;
    std::string rname;

    nl.init(uri, '/');
    if (nl[0] == "") {
      nl.erase(nl.begin());
    }

    rname = nl[0];    
    if (accessInfo.count(rname) == 0) {
   	return archiveHome;
    
    } else {
	return (accessInfo.at(rname)).prefix;
    } 
}


std::string ArchiveManager::getExec(MMessage& m) {
    string url, fullPath, archiveName, subdir, method;
    NameList resPath;
    bool isRoot;
    
    url = m.getString("url"); 
    fullPath = getRootPath(url)+"/"+url;
    resPath.init(url,'/');

    if (resPath[0] == "" and resPath.size() > 1) {
      resPath.erase(resPath.begin());
      archiveName = resPath[0];    
    } else {
      archiveName = "";
    }

    isRoot = (resPath.size() == 1);
    /*if (!FileManager::isDir(fullPath) and 
            !isValidMethod(resPath.back()) and
                            !isInstance(fullPath)) {
	return "Not Found";
    }*/

    if (!isRoot and (accessInfo.count(archiveName) == 0)) {
    	return "Not Found";
    }
    
    if (isRoot) {
        subdir = ".";
    } else {
        subdir = resPath[1];
    }
    
    bool permission = checkPermission(resPath, READ);
    if (!permission) {
	return "Forbidden";
    } else {
	string dirPath;
        int pos = fullPath.rfind("/");
        dirPath = fullPath.substr(0, pos);
	return dirPath;
    }
}


std::string ArchiveManager::getList(MMessage m) {
    string url, fullPath, archiveName, subdir, method;
    NameList resPath;
    bool isRoot;
    
    url = m.getString("url"); 
    fullPath = getRootPath(url)+"/"+url;
    resPath.init(url,'/');

    if (resPath[0] == "" and resPath.size() > 1) {
      resPath.erase(resPath.begin());
      archiveName = resPath[0];    
    } else {
      archiveName = "";
    }

    isRoot = (resPath.size() == 1);
    if (!FileManager::isDir(fullPath)) {
	return "Not Found";
    }

    if (!isRoot and (accessInfo.count(archiveName) == 0)) {
    	return "Not Found";
    }
    
    if (isRoot) {
        subdir = ".";
    } else {
        subdir = resPath[1];
    }
    
    bool permission = checkPermission(resPath, READ);
    if (!permission) {
	return "Forbidden";
    } else {
	std::string response;
	std::vector<std::string> list;
	getSubPaths(url, list);

	for (int i=0; i<(int)list.size();++i) {
	    response += "<item>"+list[i]+"</item>";
	}
	
	return response;
    }
}


MMessage ArchiveManager::postExec(MMessage& m) {
    MMessage response;
    NameList resPath;
    string url, body, fullPath, mtype, msubtype;
    string dummy = "*";
    
    restify(m);
    url = m.getString("url");
    resPath.init(url,'/');
    response.mtype = "answer";    
    m.erase("url");

    mtype = m.mtype;
    msubtype = m.msubtype;

    if (resPath[0] == "" and resPath.size() > 1) {
    	resPath.erase(resPath.begin());
    }
    fullPath = archiveHome+"/"+url;    

   
    if (checkPermission(resPath, WRITE) and !FileManager::isDir(fullPath)) {
        ofstream f;
        addResource(url);
        body = m.xmlEncode();
	
        mutexLock.lock();       //critical section -- start
	if (FileManager::isFile(fullPath)) {
            f.open(fullPath.c_str(), ofstream::app);
        } else {
            f.open(fullPath.c_str(), ofstream::out);
        }

	if(SubjectSet::check_subject(mtype, msubtype) || SubjectSet::check_subject(mtype, dummy)){
	    m.add("method", "POST"); 
	    m.add("url", url); 
	    SubjectSet::notify(m);        
	}

        f << body << std::endl;
	f.close();
        mutexLock.unlock();     //critical section -- end

        response.add("body", "OK");
    
    } else {
        response.add("body", "Forbidden");
    }
    
    return response;
}


MMessage ArchiveManager::putExec(MMessage& m) {
    MMessage response;
    NameList resPath;
    string url, data, fullPath, mtype, msubtype;
    string dummy = "*";
    
    restify(m);
    url = m.getString("url");
    resPath.init(url,'/');
    response.mtype = "answer";    
    m.erase("url"); 
 
    mtype = m.mtype;
    msubtype = m.msubtype;	

    if (resPath[0] == "" and resPath.size() > 1) {
    	resPath.erase(resPath.begin());
    }
    fullPath = archiveHome+"/"+url;    
    
    if (checkPermission(resPath, WRITE) and !FileManager::isDir(fullPath)) {
        ofstream f;
        addResource(url);
        data = m.xmlEncode();
  
        mutexLock.lock();     //critical section -- start
        f.open(fullPath.c_str());       
	if(SubjectSet::check_subject(mtype, msubtype) || SubjectSet::check_subject(mtype, dummy)){
	    m.add("method", "PUT"); 
	    m.add("url", url); 
	    SubjectSet::notify(m);        
	}
	
	f << data << std::endl;
	f.close();
        mutexLock.unlock();   //critical section -- end
 
        response.add("result", "OK");
    
    } else {
        response.add("result", "Forbidden");
    }
    
    return response;
}




MMessage ArchiveManager::putConf(MMessage& m) {
    MMessage response;
    NameList resPath;
    string url, fullPath, mtype, msubtype;
    string dummy = "*";
    
    restify(m, true);
    url = m.getString("url");
    resPath.init(url,'/');
    response.mtype = "answer";    
    m.erase("url");
	
    m.mtype = mtype = "cmd";
    m.msubtype = msubtype = "putConf";	

    if (resPath[0] == "" and resPath.size() > 1) {
    	resPath.erase(resPath.begin());
    }
    fullPath = getRootPath(url)+"/"+url;    
    
    if (checkPermission(resPath, WRITE) and !FileManager::isDir(fullPath)) {
        ofstream f;
	std::string body;
        addResource(url);
        
        mutexLock.lock();        //critical section -- start
        f.open(fullPath.c_str());
        if (!f.is_open()) {
           mutexLock.unlock();
	   response.add("body", "Forbidden");
	   return response;
	}
        body = m.getString("body");
	f << body << std::endl;
	f.close();
        mutexLock.unlock();     //critical section -- end

	if(SubjectSet::check_subject(mtype, msubtype) || SubjectSet::check_subject(mtype, dummy)){
	    m.add("method", "PUT"); 
	    m.add("url", url); 
	    SubjectSet::notify(m);        
	}
	
//	body = m.getString("body");	
        if(FileManager::getExt(fullPath) != "json") response.add("body", "OK");
	else response.add("body","{\"response\":{\"message\":\"ok\",\"upload\":\"true\"}}");
    
    } else {
        if(FileManager::getExt(fullPath) != "json") response.add("body", "Forbidden");
	else response.add("body","{\"response\":{\"message\":\"ok\",\"upload\":\"false\"}}");
    }
    
    return response;
}



MMessage ArchiveManager::putLog(MMessage& m) {
    MMessage response;
    NameList resPath;
    string url, fullPath, mtype, msubtype;
    string dummy = "*";
    
    restify(m);
    url = m.getString("url");
    resPath.init(url,'/');
    response.mtype = "answer";    
    m.erase("url");
	
    m.mtype = mtype = "cmd";
    m.msubtype = msubtype = "putConf";	

    if (resPath[0] == "" and resPath.size() > 1) {
    	resPath.erase(resPath.begin());
    }

    fullPath = getRootPath(url)+"/"+url;    
    if (checkPermission(resPath, WRITE) and !FileManager::isDir(fullPath)) {
        ofstream f;
	std::string body;
        if (!addResource(url)) {
           response.add("body", "Forbidden");
	   return response;
	}

        mutexLock.lock();       //critical section -- start
	f.open(fullPath.c_str());
	if (!f.is_open()) {
           mutexLock.unlock();
	   response.add("body", "Forbidden");
	   return response;
	}
	body = m.getString("body");
	f << body << std::endl;
	f.close();
        mutexLock.unlock();     //critical section -- end	

        if(SubjectSet::check_subject(mtype, msubtype) || SubjectSet::check_subject(mtype, dummy)){
	    m.add("method", "PUT"); 
	    m.add("url", url); 
	    SubjectSet::notify(m);        
	}
	
	body = m.getString("body");	
        response.add("body", "OK");
    
    } else {
        response.add("body", "Forbidden");
    }
    
    return response;
}




MMessage ArchiveManager::postLog(MMessage& m) {
    MMessage response;
    NameList resPath;
    string url, data, fullPath, mtype, msubtype;
    string dummy = "*";
    
    restify(m);
    url = m.getString("url");
    resPath.init(url,'/');
    response.mtype = "answer";    
    m.erase("url");
	
    m.mtype = mtype = "cmd";
    m.msubtype = msubtype = "putConf";	

    if (resPath[0] == "" and resPath.size() > 1) {
    	resPath.erase(resPath.begin());
    }

    fullPath = getRootPath(url)+"/"+url;        
    if (checkPermission(resPath, WRITE) and !FileManager::isDir(fullPath)) {
        ofstream f;
	std::string body;
        if (!addResource(url)) {
	    response.add("body", "Forbidden");		
	}

        mutexLock.lock();       //critical section -- start
	if (FileManager::isFile(fullPath)) {
            f.open(fullPath.c_str(), ofstream::app);
        } else {
            f.open(fullPath.c_str(), ofstream::out);
        }

	if (!f.is_open()) {
            mutexLock.unlock();
	    response.add("body", "Forbidden");	
	    return response;
	}
	
        body = m.getString("body");
        f << body << std::endl;
	f.close();
        mutexLock.unlock();     //critical section -- end

	if(SubjectSet::check_subject(mtype, msubtype) || SubjectSet::check_subject(mtype, dummy)){
	    m.add("method", "POST"); 
	    m.add("url", url); 
	    SubjectSet::notify(m);        
	}
	
        response.add("body", "OK");
    
    } else {
        response.add("body", "Forbidden");
    }
    
    return response;
}


bool ArchiveManager::addResource(std::string resPath, std::string src, bool overwrite) {
    boost::unique_lock<boost::mutex> lock(ArchiveManager::mutexLock);              

    NameList nlist;
    string absPath=getRootPath(resPath);
    string fullPath = archiveHome+"/"+resPath;
    
    nlist.init(resPath, '/');
    if (FileManager::isFile(fullPath) and src=="") {
        return true;
    }
    
    for (int i=0; i<(int)nlist.size()-1; ++i) {
        absPath += "/" + nlist[i];	
        if ( !FileManager::isDir(absPath) ) {
            FileManager::makeDir(absPath, true);
        } 
    }
    
    hsrv::strReplace(fullPath,"//","/");
    if (src!="") {
       if (!FileManager::isFile(fullPath) or overwrite) {
         if (FileManager::copyFile(src, fullPath)) {
          return true;
       	 } else {
          return false;
       	 }  
       } else {
          return true;
       }
       
    } else {
       return true;
    }
}


bool ArchiveManager::deleteResource(std::string resPath) {
    boost::unique_lock<boost::mutex> lock(ArchiveManager::mutexLock);          
  
    NameList nlist;
    string absPath=getRootPath(resPath);
    string fullPath = absPath+"/"+resPath;
    RootDescriptor d;

    nlist.init(resPath, '/');
    if (nlist.front() == "") {
	nlist.erase(nlist.begin());
    }

    if (accessInfo.count(nlist.front()) == 0) {
	return false;
    }

    d = accessInfo.at(nlist.front());
    if (d.prefix != hsrv::configdir) {
	fullPath += "/data";
  	nlist.push_back("data");
    }
    
    if (!FileManager::isFile(fullPath) and resPath!="") {
        return false;
    
    } else if (!checkPermission(nlist, WRITE)) {
	return false;
    
    } else {
	std::string parent = getParentDirectory(fullPath);
	std::string resname = nlist.back();
	return FileManager::deleteFile(parent, resname);
    }

}



bool ArchiveManager::updateResource(std::string resPath, std::string s) {
    boost::unique_lock<boost::mutex> lock(ArchiveManager::mutexLock);          
    
    string absPath=archiveHome;
    string fullPath = archiveHome+"/"+resPath;
    ofstream f;    

    hsrv::strReplace(fullPath,"//","/");
    if (!FileManager::isFile(fullPath)) {
	f.open(fullPath.c_str(), ofstream::out);
    
    } else {
	f.open(fullPath.c_str(), ofstream::app);
    }
    
    f << s << std::endl;   
    
    f.close();
    return true;
}




void ArchiveManager::exploreArchive(string src, vector<string>& list, bool noDir) {
    std::vector<string> subdir;
    FileManager::dirList(src, subdir);

    if (subdir.size() == 0) {
        std::vector<string> flist;
        FileManager::fileList(src, flist);
        for (int i=0; i < (int)flist.size(); ++i) {
            string fullPath = src+"/"+flist[i];
            if (FileManager::isFile(fullPath)) {
                list.push_back(fullPath);
            } else if (FileManager::isDir(fullPath) and !noDir) {
                list.push_back(fullPath);
            }
        }
        if (noDir == false) {
            list.push_back(src);
        }
        return;
        
    } else {
        for (int i=0; i<(int)subdir.size(); ++i) {
            string child = src+"/"+subdir[i];
            exploreArchive(child, list, noDir);
        }
    }
}


void ArchiveManager::getSubPaths(string src, vector<string>& list) {
    std::string rootP;
    std::string homeD;

    if (*src.rbegin() == '/') {
        int n = src.size();
        src = src.substr(0,n-1);
    }
    
    rootP = getRootPath(src);
    src = rootP + "/" + src;
    hsrv::strReplace(src, "//", "/");
    exploreArchive(src, list, true);

    for (int i=0; i<(int)list.size();++i) {
        int pos = list[i].find(rootP);
        if (pos != string::npos) {
            list[i] = list[i].substr(rootP.size(), string::npos);
            if (list[i][0] == '/') {
                list[i] = list[i].substr(1, string::npos);
            }
        }
    }
    
    
    std::vector<string> instanceMethodList;
    std::vector<string>::iterator it = list.begin();
    while(it != list.end()) {
        string fullPath = archiveHome+"/"+*it;
	NameList nl;
        nl.init(*it, '/');
        if (!checkPermission(nl, READ) or (fullPath==src)) {
	    it = list.erase(it);
        } else if (nl.back() == "data") {
            string parent = getParentDirectory(*it);
            it = list.erase(it);
            for (int i=0; i<(int)methods.size(); ++i) {
                instanceMethodList.push_back(parent+"/"+methods[i]);
            }
            
        } else {
            it++;
        }
    }
    
    list.insert(list.end(), instanceMethodList.begin(), instanceMethodList.end());
}


string ArchiveManager::getRelativePath(string path) {
    string relPath;
    int pos;

    hsrv::strReplace(relPath, "//", "/");
    hsrv::strReplace(archiveHome, "//", "/");
    
    pos = path.find(archiveHome);
    if (pos != string::npos) {
        relPath = path.substr(archiveHome.size(), string::npos);
    }
    
    if (*relPath.begin() == '/') {
        relPath = relPath.substr(1, string::npos);
    }
    
    return relPath;
}


string ArchiveManager::extractURL(string reqURL) {
    string pattern = "/"+archiveName+"/";
    size_t nchar = pattern.size();
    size_t pos = reqURL.find(pattern);
    
    if (pos < 0) {
        return "";
    } else { 
        return reqURL.substr(pos+nchar, string::npos);
    }
}


bool ArchiveManager::getCategoriesJson(std::string body, NameList& category) {
    boost::property_tree::ptree pt, rpt;
    std::stringstream ss;
    ss<<body;
    read_json(ss,rpt);
    if(body.find("select") == string::npos) return false;
    pt = rpt.get_child("select");
    BOOST_FOREACH(boost::property_tree::ptree::value_type &v, pt) {
      string cat = v.second.get<string>("name");
      if(cat != "") category.push_back(cat);
    }
    return true;
}

bool ArchiveManager::getTimeRangeFromJson(std::string body, std::string& start, std::string& end) {
    std::stringstream ss;
    std::string start_date;
    std::string start_time;
    std::string end_date;
    std::string end_time;
    boost::property_tree::ptree rpt;
    boost::property_tree::ptree pt;

    if(body.find("window") != string::npos) {
       ss << body;
       read_json(ss, rpt);
       pt = rpt.get_child("window");
    }
    else {
       ss << body;
       read_json(ss, pt);
    }
    start_date = pt.get<string>("start_date");
    start_time = pt.get<string>("start_time");
    end_date = pt.get<string>("end_date");    
    end_time = pt.get<string>("end_time");

	
    std::string d1, d2, m1, m2, y1, y2, h1, h2, min1, min2;
    d1 = start_date.substr(0, 2);
    m1 = start_date.substr(2, 2);
    y1 = start_date.substr(4, 4);
    d2 = end_date.substr(0, 2);
    m2 = end_date.substr(2, 2);
    y2 = end_date.substr(4, 4);
	
    h1 = start_time.substr(0, 2);
    min1 = start_time.substr(3, 5);
    h2 = end_time.substr(0, 2);
    min2 = end_time.substr(3, 5);

    start = y1+m1+d1+h1+min1;
    end = y2+m2+d2+h2+min2;

    return true;
}


std::string ArchiveManager::normalizeDateTime(std::string tt) {
    std::string t;
    std::string y, m, d, h, min;
    if(tt==""||tt.size()<10) return tt;
    if(tt.substr(0,2)=="20") t = tt.substr(2);
    else t = tt;
    y = "20"+t.substr(0,2);
    m = t.substr(2,2);
    d = t.substr(4,2);
    h = t.substr(6,2);
    min = t.substr(8,2);

    return (y+m+d+h+min);
}


void ArchiveManager::clear(std::string filename) {
    ifstream f;
    std::string line;

    f.open(filename.c_str());
    if ( !f.is_open() ) {
      return;
    }

    while( !f.eof() ) {
      getline(f, line);
      std::string xmlstring = "<?xml version=\"1.0\" ?>"+line;
      std::string url;	

      TiXmlDocument doc;
      TiXmlNode* desc;
      TiXmlNode* pChild;
      TiXmlElement* pElement;
      doc.Parse(xmlstring.c_str(), 0 , TIXML_ENCODING_UTF8);
      desc = doc.FirstChild();
      desc = desc->NextSibling();
 
      if (desc != NULL) {
	url = hsrv::getAttrbyName(desc->ToElement(), "url");
	url = archiveHome + "/" + url;		
	string parentDir = getParentDirectory(url);
	FileManager::deleteFile(parentDir, FileManager::getTail(url));
      }
    }
    	
    f.close();
}


void ArchiveManager::purge(bool video) {
    boost::unique_lock<boost::mutex> lock(ArchiveManager::mutexLock);          

    std::vector<string> archiveFiles;
    string datestr = hsrv::getday(-60);
    
    exploreArchive(archiveHome, archiveFiles, true);
    for (int i=0; i<(int)archiveFiles.size(); ++i) {
	string filename = archiveFiles[i];
        string ext = FileManager::getExt(filename);
	string tail = FileManager::getTail(filename);
	string prefix;	
	size_t pos;

// aggiunto da Attilio per consentire di saltare la purge su root con timeWin negativ
        int timeWin;
        NameList fRelPath;
        fRelPath.init(getRelativePath(filename), '/');
        timeWin = getWindow(fRelPath.front());
        if(timeWin < 0) {
           continue;
        }
//
	pos = tail.find('.');
	if (pos != std::string::npos) {
	    prefix = tail.substr(0,pos);
	}
	
	if (tail != "data" and prefix != "data") {
	  continue;
	}	

        //current data, rename it with time extension
        if ( !isDate(ext, '_')) {
            string newName = archiveFiles[i] + "." + datestr;
            FileManager::moveFile(archiveFiles[i], newName);
            
        
        //old data, check if expired, eventually delete it
        } else {
        //    int timeWin;
        //    NameList fRelPath;
        //    fRelPath.init(getRelativePath(filename), '/');
        //    timeWin = getWindow(fRelPath.front());
            
            if ( getDateDifference(datestr, ext) > timeWin) {
           	clear(filename);     
		string parentDir = getParentDirectory(filename);
                FileManager::deleteFile(parentDir, FileManager::getTail(filename));
            }
        }
    }
    
    return;
}



void ArchiveManager::restify(MMessage& m, bool conf) {
    NameList url;
    string urlstr;
    
    //extract an url relative to the archive root
    //urlstr = extractURL(m.getString("url"));
    urlstr = m.getString("url");    
    
    //no url => old style horus message
    if (urlstr == "") {
    	urlstr = "/message"; 
    }

    //remove an eventual empty space
    url.init(urlstr, '/');
    if (url[0] == "" and url.size() > 1) {
      url.erase(url.begin());
    }
    
    bool msgSub = false;
    std::vector<string> msgSubDir = getSubDir("message");
    

    if (url.empty()) {
      for (int i=0; i<msgSubDir.size(); ++i) {
    	  if (msgSubDir[i] == m.mtype){
	     msgSub = true;        
	     urlstr += "/message/";	
	  }
      }
    }

    
    if (url.front() == "message" or msgSub) {
        //case 1: old-style request -> convert url in a restful url
        if (m.mtype != "*" and m.msubtype != "*") {
            urlstr += "/" + m.mtype;
            urlstr += "/" + m.msubtype;
            
        } else {
            //case 2: restful request -> normalize message for retro-compatibility
            if (m.mtype == "*" and (url.size() > 1)) {
                m.mtype = url[1];
            }
            
            if (m.msubtype == "*" and (url.size() > 2)) {
                m.msubtype = url[2];
            }
        }
    }
    //url is now relative and corrected
    if (url.back() != "data" and !conf) {
    	urlstr += "/data";
    }
    m.add("url", urlstr);
     
    return;
}


bool ArchiveManager::isValidMethod(string method) {
    int nmeth = (int)ArchiveManager::methods.size();
    for (int i=0; i<nmeth; ++i) {
        if (ArchiveManager::methods[i] == method) {
            return true;
        }
    }
     
    return false;
}


bool ArchiveManager::isInstance(string pathname) {
     int pos = pathname.rfind("/");
     string dirPath = pathname.substr(0, pos);
       
     if (FileManager::isDir(dirPath) and !FileManager::dirCount(dirPath)) {
         return true;
     } else {
         return false;
     }
}


string ArchiveManager::getAll(MMessage req) {
    boost::unique_lock<boost::mutex> lock(ArchiveManager::mutexLock);          
    ifstream f;
    string allData, line;
    string resPath = getExec(req);
    string filePath = resPath+"/data";   
    if (resPath == "Not Found" or resPath == "Forbidden") {
	return resPath;
    }

    f.open(filePath.c_str());
    if ( !f.is_open() ) {
	return "Not Found";
    }    

    while( !f.eof() ) {
        getline(f, line);
        allData += line +"\n";
    }
    f.close();
    
    return allData;
}


string ArchiveManager::getSubSet(MMessage req) {
    boost::unique_lock<boost::mutex> lock(ArchiveManager::mutexLock);          

    size_t pos;    
    ifstream f;    
    std::string resPath;
    std::string setData, line, body;
    std::string rootPath = archiveHome+"/archive/";
    std::vector<std::string> cameras;
    std::string start, end;
    
    body = req.getString("body");
    if ( !getTimeRangeFromJson(body, start, end) ) {
      return "Bad Request";
    }
    FileManager::dirList(rootPath, cameras);
    for ( int i=0; i<(int)cameras.size(); ++i) {
	std::string camdir = rootPath+cameras[i];
	std::vector<std::string> descriptors;
	FileManager::fileList(camdir, descriptors);
    	
	for (int j=0; j<(int)descriptors.size(); ++j) {
		string tail = FileManager::getTail(descriptors[j]);
		string prefix;	
		size_t pos;
	
		pos = tail.find('.');
		if (pos != std::string::npos) {
		    prefix = tail.substr(0,pos);
		}
			
		if (prefix != "data") {
		  continue;
		}	
		
		if (tail != "data" and !evalDescriptor(start, end, descriptors[j])) {
		  continue;
      		}
	 	
	        string filePath = camdir+"/"+descriptors[j];   
		f.open(filePath.c_str());
		if ( !f.is_open() ) {
		  continue;
		}

		while( !f.eof() ) {
		  getline(f, line);
		  std::string xmlstring = "<?xml version=\"1.0\" ?>"+line;
		  std::string timestamp;	
	
		  TiXmlDocument doc;
		  TiXmlNode* desc;
		  TiXmlNode* pChild;
		  TiXmlElement* pElement;
		  doc.Parse(xmlstring.c_str(), 0 , TIXML_ENCODING_UTF8);
		  desc = doc.FirstChild();
		  desc = desc->NextSibling();
 
		  if (desc != NULL) {
		    timestamp = hsrv::getAttrbyName(desc->ToElement(), "time");
		    timestamp = ArchiveManager::normalizeDateTime(timestamp);	
		    if (timestamp >= start and timestamp <= end) {
			setData += line;
		     }	
		  }
	    	}
	    	f.close();
	    }
	}

    return setData;
}

string ArchiveManager::getSubLog(MMessage req) {
    boost::unique_lock<boost::mutex> lock(ArchiveManager::mutexLock);          
    ifstream f;    
    std::string resPath;
    std::string line, body;
    std::string rootPath;
    std::string start, end, prefix, tail;
    bool firstitem = true;
    std::stringstream setData;
    NameList category;

    rootPath = archiveHome+req.getString("url");
    body = req.getString("body");
    if ( !getTimeRangeFromJson(body, start, end) ) {
      return "Bad Request";
    }
    getCategoriesJson(body, category);
    std::vector<std::string> descriptors;
    FileManager::fileList(rootPath, descriptors);
    setData << "[";
    for (size_t j=0; j<descriptors.size(); ++j) {
	tail = FileManager::getTail(descriptors[j]);
	prefix = FileManager::getStem(tail);	
	
	if (prefix != "data") {
	  continue;
	}	

	if (tail != "data" and !evalDescriptor(start, end, descriptors[j])) {
	  continue;
     	}

        string filePath = rootPath+"/"+descriptors[j];   
	f.open(filePath.c_str());
	if ( !f.is_open() ) {
	  continue;
	}

	while( !f.eof() ) {
	  getline(f, line);
	  if(line.size()<10) {
	    continue;
	  }
          boost::property_tree::ptree pt;
          std::stringstream ss;

	  ss<<line;
	  read_json(ss, pt);
	  std::string timestamp = pt.get<string>("time");
	  timestamp = ArchiveManager::normalizeDateTime(timestamp);	
 	  std::string cat = pt.get<string>("type");
	  if ((timestamp=="" or (timestamp > start and timestamp <= end)) and
					 (category.size()==0 or category.member(cat))) {
	     if(firstitem) {
		setData << line;
	        firstitem = false;
	     }
	     else setData <<","<<line;
	  }	

	}
        f.close();
     }
     setData<<"]";
     return setData.str();
}


bool ArchiveManager::evalDescriptor(std::string start, std::string end, std::string desc) {
    size_t pos;
    NameList date;	
    std::string t, dday, sday, eday;
    if((pos = desc.find('.')) == string::npos) return true;
    t = desc.substr(pos+1, std::string::npos);
    date.init(t, '_');
    dday += date[0]+date[1]+date[2];
    sday = start.substr(0,8);
    eday = end.substr(0,8);
 
    return (dday >= sday and dday <= eday);
}



std::string ArchiveManager::getEnergyData(MMessage req) {
    boost::unique_lock<boost::mutex> lock(ArchiveManager::mutexLock);       

    std::string resPath = getExec(req);
    std::string url = req.getString("url"); 
    std::string fname = hsrv::tailof(url);
    std::string fullpath = resPath+"/"+fname;
    std::string start = req.getString("start");
    std::string end = req.getString("end");
    std::string setdata;

    if (resPath == "Not Found" or resPath == "Forbidden") {
   	return resPath;
    }

    std::ifstream input(fullpath.c_str(), std::ios::binary);
    if (!input.is_open()) {
    	return "Not Found";
    }
 
    std::string line;
    while (std::getline(input, line)) {	
        float apower, ipower, energy;
        string time;
        stringstream ts,ss;
	std::string timestamp;   
        timeval t1, t2;

        scanLine(line, apower, ipower, energy, time);
 
        timestamp = ArchiveManager::normalizeDateTime(time);
        if (timestamp >= start and timestamp <= end) {
          ss<<"{\"Apower\":\""<< apower<<"\",\"Ipower\":\""<<ipower<<"\",\"energy\":\""<<energy<<"\",\"time\":\""<<time<<"\"}";
	  setdata += ss.str() +",";
    	}
        gettimeofday(&t2, NULL);
    }
    
    size_t pos = setdata.find_last_of(",");
    if (pos != std::string::npos) {
       setdata[pos] = ' ';
    }
    
    return setdata;
}


string ArchiveManager::getPMLog(MMessage req) {
    boost::unique_lock<boost::mutex> lock(ArchiveManager::mutexLock);       

    std::string devname, url, setdata, body, mode;
    boost::property_tree::ptree pt;
    stringstream ss;
    NameList nl;
    size_t pos;    

    url = req.getString("url");
    body = req.getString("body");
    ss << body;
    read_json(ss, pt);
    mode = pt.get<string>("report");
    if (mode == "DAILY") {
      setdata = getPMDaily(req);
    
    } else if (mode == "MONTHLY") {
      setdata = getPMMonthly(req);
    
    } else {
      setdata = getPMAnnual(req);
    } 
    
    nl.init(url, '/');
    devname = nl.back();
    pos = devname.find('.');
    devname = devname.substr(0, pos);
    
    std::string response = "{\"response\":{\"message\":\"ok\",";
    response += "\"device\":{\"name\":\""+devname+"\",";
    response += "\"energy_parameters\":[\n";
    response += setdata + "]}}}";   

    return response;
}


string ArchiveManager::getPMDaily(MMessage req) {
    std::string devname, url, body, devdir, response;
    std::string date, today, d, m, y, respath, fullpath;
    boost::property_tree::ptree pt;
    stringstream ss;
    NameList nl;
    size_t pos;    
    ifstream f;   
    
    body = req.getString("body");
    url = req.getString("url");
    nl.init(url, '/');
    devname = nl.back();
    pos = devname.find('.');
    devname = devname.substr(0, pos);
    
    today = ArchiveManager::getCurrentDay(d,m,y);
    ss << body;
    read_json(ss, pt);
    date = pt.get<string>("date");
    url = "/event/"+devname;
    req.add("url", url);
    respath = getExec(req);    
    
    if (date == today) {
      fullpath = respath+"/"+devname+"/data";
    
    } else {
      fullpath = respath+"/"+devname+"/data."+
                date.substr(0,4)+"_"+date.substr(4,2)+"_"+date.substr(6,2);
    }

    std::ifstream input(fullpath.c_str(), std::ios::binary);
    if (!input.is_open()) {
    	return "";
    }
 
    std::string line;
    std::vector<PMStat> stat;

    while (std::getline(input, line)) {	
        float apower, ipower, energy;
        string time;
        stringstream ts,ss;
        string hs;
        int h; 

        scanLine(line, apower, ipower, energy, time);
    
        hs = time;
//        hs = hs.substr(6,2);
//        h = atoi(hs.c_str());
        stat.push_back(PMStat(energy, hs));
    }

    for (size_t i=0; i<stat.size(); i++) {
        response += stat[i].getJsonString(DAILY);
        if (i < stat.size()-1) {
          response += ",\n";
        }
    }

    return response;
}





string ArchiveManager::getPMMonthly(MMessage req) {
    std::string date, today, d, m, y, respath, fullpath, start, end;
    std::string devname, url, body, devdir, response, b, setdata;
    std::vector<std::string> descriptors;
    boost::property_tree::ptree pt;
    stringstream ss;
    NameList nl;
    size_t pos;    
    ifstream f;  
    int month; 
    std::vector<std::string> dayarray;
    today = getCurrentDay(d, m, y);
    body = req.getString("body");
    url = req.getString("url");
    ss << body;
    read_json(ss, pt);
    b = pt.get<string>("date");
    start = b.substr(0,4)+b.substr(4,2)+"01";
    end = b.substr(0,4)+b.substr(4,2)+"31";
    month = atoi(b.substr(4,2).c_str());

    nl.init(url, '/');
    devname = nl.back();
    pos = devname.find('.');
    devname = devname.substr(0, pos);
    
    devdir = getRootPath("event")+"/event/"+devname;
    FileManager::fileList(devdir, descriptors);
    for (int i=0; i<(int)descriptors.size(); ++i) {
      int idx;
      NameList tmp;
      if (descriptors[i] == "data") {
        idx = atoi(d.c_str())-1;
      } else {
        tmp.init(descriptors[i], '_');
        idx = atoi(tmp[2].c_str())-1;
      }

      if ((descriptors[i] != "data") and 
	!evalDescriptor(start, end, descriptors[i])) {
	continue;
      }

      if ((descriptors[i] == "data") and 
        ((m != b.substr(4,2)) or (y != b.substr(0,4)))) {
        continue;
      }

      url = "/event/"+devname;
      req.add("url", url);
      respath = getExec(req);
      fullpath = respath+"/"+devname+"/"+descriptors[i];
      dayarray.push_back(getPMDaySummary(fullpath));
    }
    sort(dayarray.begin(), dayarray.end());  
    for(size_t j=0; j<dayarray.size(); j++)
      if(j<dayarray.size()-1) setdata += dayarray[j] + ",\n";
      else setdata += dayarray[j] + "\n";

    return setdata;
}    

string ArchiveManager::getPMMonthlySummary(MMessage req) {
    std::string date, today, d, m, y, respath, fullpath, start, end;
    std::string devname, url, body, devdir, response, b, setdata;
    std::vector<std::string> descriptors;
    boost::property_tree::ptree pt;
    stringstream ss;
    NameList nl;
    size_t pos;    
    ifstream f;  
    int month; 
    std::vector<std::string> dayarray;
    today = getCurrentDay(d, m, y);
    body = req.getString("body");
    url = req.getString("url");
    ss << body;
    read_json(ss, pt);
    b = pt.get<string>("time");
    start = b.substr(0,4)+b.substr(4,2)+"01";
    end = b.substr(0,4)+b.substr(4,2)+"31";
    month = atoi(b.substr(4,2).c_str());
    nl.init(url, '/');
    devname = nl.back();
    pos = devname.find('.');
    devname = devname.substr(0, pos);
    
    devdir = getRootPath("event")+"/event/"+devname;
    FileManager::fileList(devdir, descriptors);
    for (int i=0; i<(int)descriptors.size(); ++i) {
      int idx;
      NameList tmp;
      if (descriptors[i] == "data") {
        idx = atoi(d.c_str())-1;
      } else {
        tmp.init(descriptors[i], '_');
        idx = atoi(tmp[2].c_str())-1;
      }

      if ((descriptors[i] != "data") and 
	!evalDescriptor(start, end, descriptors[i])) {
	continue;
      }

      if ((descriptors[i] == "data") and 
        ((m != b.substr(4,2)) or (y != b.substr(0,4)))) {
        continue;
      }

      url = "/event/"+devname;
      req.add("url", url);
      respath = getExec(req);
      fullpath = respath+"/"+devname+"/"+descriptors[i];
      dayarray.push_back(getPMDaySummary(fullpath));
    }
    float energy, tenergy;
    int tt;
    energy = 0;
    for(size_t j=0; j<dayarray.size(); j++) {
       sscanf(dayarray[j].c_str(), "{\"time\":\"%d\",\"energy\":\"%f\"",&tt, &tenergy);
       energy += tenergy;
    }
    string mm = b.substr(4,2).c_str();
    if(energy >0 ) setdata = "{\"time\":\"" + mm + "\",\"energy\":\""+ hsrv::double2a((double)energy)+"\"}";
	else setdata = "";
    return setdata;
}



string ArchiveManager::getPMAnnual(MMessage req) {
    std::string date, today, d, m, y, respath, fullpath, start, end;
    std::string devname, url, body, devdir, response, b, setdata;
    std::vector<std::string> descriptors;
    std::vector<PMStat> stats;
    boost::property_tree::ptree pt;
    stringstream ss;
    NameList nl;
    size_t pos;    
    ifstream f;   
    
    today = getCurrentDay(d, m, y);
    body = req.getString("body");
    url = req.getString("url");
    ss << body;
    read_json(ss, pt);
    b = pt.get<string>("date");
    bool notfirst = false;

    //For each month
    for (int i=0; i<12; ++i) {
      MMessage mreq;
      stringstream ym;
      std::string mbody;   
      NameList tuples;
      PMStat mstat();

      ym << b; //year
      if (i+1 < 10) ym << 0;
      ym << i+1; //year+month (yyyymm)
      mbody = "{\"time\":\""+ym.str()+"\"}";
      mreq.add("url", url);
      mreq.add("body", mbody);
      string mdata = getPMMonthlySummary(mreq);
      if(mdata != "") {
	 if(notfirst) setdata = setdata + ",\n" + mdata;
	 else setdata = setdata + mdata;
         notfirst = true;
      }
    }
    setdata += "\n";
    return setdata;
}





string ArchiveManager::getPMDaySummary(std::string datafile) {
    PMStat stat(0, "0");;   
    NameList nl;
    ifstream input;
    std::string line, date, day, month, year, fname;

    input.open(datafile.c_str());
    if (!input.is_open()) {
    	return "Not Found";
    }
      
    fname = hsrv::tailof(datafile);
    if (fname == "data") {
      date = getCurrentDay(day, month, year);
    
    } else {
      nl.init(fname, '_');
      day = nl.back();
    }

    while (std::getline(input, line)) {	
        float apower, ipower, energy;
        string time;
        stringstream ts,ss;
        string hs;
        int h; 

	scanLine(line, apower, ipower, energy, time);
    
        stat.update(energy,day.c_str());
    }

    return stat.getJsonString(MONTHLY);
}



string ArchiveManager::getFirst(MMessage req, std::string& filetype) {
    boost::unique_lock<boost::mutex> lock(ArchiveManager::mutexLock);       
    
    ifstream f;
    string firstData;
    string resPath = getExec(req);
    string filePath = resPath + "/data";

    if (resPath == "Not Found" or resPath == "Forbidden") {
	return resPath;
    }


    f.open(filePath.c_str());
    if ( !f.is_open() ) {
	return "Not Found";
    }        
    
    getline(f, firstData);
    f.close();

    std::string xmlstring = "<?xml version=\"1.0\" ?>"+firstData;
    TiXmlDocument doc;
    TiXmlNode* desc;
    TiXmlNode* pChild;
    TiXmlElement* pElement;
    doc.Parse(xmlstring.c_str(), 0 , TIXML_ENCODING_UTF8);
    desc = doc.FirstChild();
    desc = desc->NextSibling();
 
    if (desc != NULL) {
    	string format, url, fullPath;
 	format = hsrv::getAttrbyName(desc->ToElement(), "format");

	if (format == "video") {	
	   url = hsrv::getAttrbyName(desc->ToElement(), "url");
	   filetype = FileManager::getExt(url);
	   fullPath = archiveHome+url;			

	   std::ifstream input(fullPath.c_str(), std::ios::binary);
	   std::vector<char> buffer(std::istreambuf_iterator<char>(input),
	  	(std::istreambuf_iterator<char>()));
	   firstData = string(buffer.begin(), buffer.end());	

	}
    }
    	

    return firstData;
}


string ArchiveManager::getLatestData(string dir) {
   std::vector<string> list;
   std::vector<string>::iterator it;  
   std::string tmp = dir+"/data";
  
   //return current day data
   if (FileManager::isFile(tmp)) {
      return dir+"/data";
   }

   //get past days data
   FileManager::fileList(dir, list);
   it = list.begin();
   while ( it != list.end() ) {
     if (it->find("data.") == string::npos) {
        it = list.erase(it);
     } else {
        it++;
     }
   }

   //no data to return
   if (list.empty()) {
        return "Not Found";
   }

   //return most recent data
   std::sort(list.begin(), list.end());
   return dir+"/"+list.front();
}


string ArchiveManager::getLast(MMessage req, std::string& filetype) {
    boost::unique_lock<boost::mutex> lock(ArchiveManager::mutexLock);       

    ifstream f;
    string temp, lastData;
    string resDir = getExec(req);
    string filePath = getLatestData(resDir);
    bool done = false;
  
    if (filePath == "Not Found" or filePath == "Forbidden") {
	return filePath;
    }


    f.open(filePath.c_str());
    if ( !f.is_open() ) {
	return "Not Found";
    }    

    while ( !f.eof() and !done ) {
        getline(f,temp);
        if (temp.empty()) {
            done = true;
            
        } else {
            lastData = temp;
        }
        
    } 
    f.close();


    std::string xmlstring = "<?xml version=\"1.0\" ?>"+lastData;
    TiXmlDocument doc;
    TiXmlNode* desc;
    TiXmlNode* pChild;
    TiXmlElement* pElement;
    doc.Parse(xmlstring.c_str(), 0 , TIXML_ENCODING_UTF8);
    desc = doc.FirstChild();
    desc = desc->NextSibling();
 
    if (desc != NULL) {
    	string format, url, fullPath;
 	format = hsrv::getAttrbyName(desc->ToElement(), "format");

	if (format == "video") {	
	   url = hsrv::getAttrbyName(desc->ToElement(), "url");
	   filetype = FileManager::getExt(url);
	   fullPath = archiveHome+url;	

	   std::ifstream input(fullPath.c_str(), std::ios::binary);
	   std::vector<char> buffer(std::istreambuf_iterator<char>(input),
	  	(std::istreambuf_iterator<char>()));
	   
	   lastData = string(buffer.begin(), buffer.end());	
	}
    }
   
    return lastData;    
}


std::string ArchiveManager::getDir(MMessage req) {
   boost::unique_lock<boost::mutex> lock(ArchiveManager::mutexLock);
    std::string resPath = getExec(req);
    std::string url = req.getString("url");
    std::string dirname = hsrv::tailof(url);
    std::string fullpath = resPath+"/"+FileManager::getStem(dirname);
    std::string answer;
    if (resPath == "Not Found" or resPath == "Forbidden") {
       return resPath;
    }
    NameList files;
    FileManager::fileList(fullpath, files);
    if(files.size()==0) FileManager::dirList(fullpath, files);
    answer = "{\"response\": {\"message\":\"ok\",\"files\":[";
    //makes the answer 
    for(size_t i=0; i<files.size(); i++) {
	if(i==0) answer += "{\"name\":\""+files[i]+"\"}";
	  else answer += ",{\"name\":\""+files[i]+"\"}";
    }
    answer += "]}}\n";
    return answer;
}



std::string ArchiveManager::getFile(MMessage req) {
    boost::unique_lock<boost::mutex> lock(ArchiveManager::mutexLock);       

    std::string resPath = getExec(req);
    std::string url = req.getString("url"); 
    std::string fname = hsrv::tailof(url);
    std::string fullpath = resPath+"/"+fname;

    if (resPath == "Not Found" or resPath == "Forbidden") {
   	return resPath;
    }


    std::ifstream input(fullpath.c_str(), std::ios::binary);
    if (!input.is_open()) {
    	return "Not Found";
    }

    std::vector<char> buffer(std::istreambuf_iterator<char>(input),
	  	(std::istreambuf_iterator<char>()));
	   
    return string(buffer.begin(), buffer.end());
}

bool ArchiveManager::scanLine(string& line, float& apower, float& ipower, float& energy, string& time) {
  size_t pos;
  string match1 = "Apower\":\"";
  string match2 = "Ipower\":\"";
  string match3 = "energy\":\"";
  string match4 = "timeofday\":\"";
  string number;

  pos = line.find(match1,0);
  if(pos == string::npos) return false;
  pos+= match1.size();
  number = "";
  while(pos <line.size() && line[pos]!='"') number+=line[pos++];
  apower = (float) hsrv::a2double(number);

  pos = line.find(match2,pos);
  if(pos == string::npos) return false;
  pos+= match2.size();
  number = "";
  while(pos <line.size() && line[pos]!='"') number+=line[pos++];
  ipower = (float) hsrv::a2double(number);
 
  pos = line.find(match3,pos);
  if(pos == string::npos) return false;
  pos+= match3.size();
  number = "";
  while(pos <line.size() && line[pos]!='"') number+=line[pos++];
  energy = (float) hsrv::a2double(number);

  pos = line.find(match4,pos);
  if(pos == string::npos) return false;
  pos+= match4.size();
  while(pos <line.size() && line[pos]!='"') time+=line[pos++];

  return true;
}
