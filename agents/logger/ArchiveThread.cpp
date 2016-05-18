/* 
 * File:   agent.h
 * Author: Davide Monfrecola<davide.monfrecola@gmail.com>
 *
 * Created on March 1, 2013, 11:09 AM
 */

#include "ArchiveThread.h"

using namespace std;

#define SCALE_FACTOR 1.0
#define MEGAUNIT 500
#define MAXYRANGE 7000

ArchiveThread::ArchiveThread(string name) {
    id = name;
    string threadpath = "threads/"+name;
    MParams params(threadpath);
    params.load();
    
    this->conf = new MParams("archive");
    this->conf->load();
    this->page = new MMessage("horus_status");
    this->install(); 
    trace = new MParams("trace");
    trace->load();
}

ArchiveThread::ArchiveThread(const ArchiveThread& orig) {
}

ArchiveThread::~ArchiveThread() {
}

bool ArchiveThread::install(string wd) {
	this->day = hsrv::getday();
    
	if(wd != "") {
        this->workingdir = wd;
    }
    else {
        this->workingdir = hsrv::homedir + "/";
    }
	horus_status = hsrv::homedir + "/horus_status";
    
	if(!hsrv::isdir(horus_status)) FileManager::makeDir(horus_status, true);
    
	return true;
}

bool ArchiveThread::installday() {
	string current_daily_dir;
	current_daily_dir = this->workingdir + "/" + this->day;
    
	if(!hsrv::isdir(current_daily_dir)) {
        
        FileManager::makeDir(current_daily_dir, true);
	}
    
	return true;	
}

void ArchiveThread::start_service_threads() {
    this->service_thread.push_back(new Thread(&ArchiveThread::do_work, this));
}

void ArchiveThread::do_work(ArchiveThread* obj) {
    MMessage out_mess;
    MMessage msg;
    obj->purge();
    while(1) {
        string file;
        string lfile;
        string feature;
        msg = obj->receive_message();
        obj->update();
        if(msg.mtype == "logcmd" && msg.msubtype == "archive") {
            feature = msg.getString("feature");
            if (isToTrace(feature)) {
            lfile = hsrv::homedir + "/";
                lfile += msg.getString("class");
                if (!FileManager::isDir(lfile)) {
                    FileManager::makeDir(lfile, false);
                }
                lfile += "/";
                lfile += feature;
                if (!FileManager::isDir(lfile)) {
                    FileManager::makeDir(lfile, false);
                }
                lfile += "/data";
                writelog(lfile, msg);
                if(hsrv::behaviour=="active") {
                    MMessage rstmsg;
                    string url = day+"/"+msg.getString("class")+"/"+feature;
                    rstmsg.mtype = "rscmd";
                    rstmsg.msubtype = "serversend";
                    rstmsg.add("url", url);
                    rstmsg.add("item", mkItem(msg));
                    SubjectSet::notify(rstmsg);
                    
                }
            }
        }
    }
}

bool ArchiveThread::writelog(string lfile, MMessage msg) {
	ofstream out;
    bool first;        
    out.open(lfile.c_str(), ofstream::app);
    if(out!=NULL) {
        out <<"{";
        MMessage::iterator it;
        first = true;
        for(it=msg.begin(); it!=msg.end(); it++) 
            if (it->first != "class" && 
                it->first != "feature"&&
                it->first != "method"&&
                it->first != "language"&&
                it->first != "url") {
                    if(first) first = false;
                    else out<<",";
                    out<< "\""<<it->first << "\"";
                    out<<":";
                    out<<"\""<<msg.getString(it->first)<<"\"";
                }
              out<<"}"<<endl;
              out.close();
            }
	    else {
		string info = "FILE "+lfile+ "  cannot be open!";
		hsrv::logger->info(info);
	    }
	return true;
}

string ArchiveThread::mkItem(MMessage& msg) {
    stringstream out;
    bool first = true;
    MMessage::iterator it;
    out <<"{";
    for(it=msg.begin(); it!=msg.end(); it++) 
        if (it->first != "class" && 
            it->first != "feature"&&
            it->first != "method"&&
            it->first != "language"&&
            it->first != "url") {
            if(first) first = false;
            else out<<",";
            out<<it->first;
            out<<":";
            out<<msg.getString(it->first);
        }
    out<<"}";
    return out.str();
}


bool ArchiveThread::purge() {
	string path = workingdir;
        string pathfile;
        string cmd;
	string tmp;
	unsigned m;
	unsigned max_day_number;
	string fname;
	string pathd;
	string pathdc;
	vector<string> dir_list;
    
	tmp = this->conf->get("maxdaynumber");
	
	if(tmp == "") return 0;
	max_day_number = hsrv::a2unsigned(tmp);

	if(max_day_number == 0) return 0;
        FileManager::dirList(path, dir_list);
    
	if(dir_list.size() <= max_day_number) return false;

	sort(dir_list.begin(), dir_list.end());
	m = dir_list.size() - max_day_number;
	
	for(unsigned i = 0; i < m; i++) {
        pathfile = path + "/" + dir_list[i];
        if((cmd = this->conf->get("compresscmd"))!= "") {
            hsrv::strReplace(cmd, "$PATH", pathfile);
            hsrv::strReplace(cmd, "$PATH", pathfile);
            hsrv::cmdExec(cmd);
        }
        if((cmd = this->conf->get("uploadcmd"))!= "") {
            hsrv::strReplace(cmd, "$PATH", pathfile);
            hsrv::cmdExec(cmd);
        }
		cmd = "rm -r " + pathfile;     
        hsrv::cmdExec(cmd);
        cmd = "rm "+ pathfile + ".*";
        hsrv::cmdExec(cmd);
	}	
    
	return true;
}

bool ArchiveThread::update() {
	string currentday = hsrv::getday();
    
	if(currentday == day) {
            return 1;
        }
        
        // operazioni da effettuare se e' cambiata la data (prima operazione del giorno)
	day = currentday;
	hsrv::archive->purge();
	
    return true;
}

double ArchiveThread::getTimeInSecond(string& time) {
    string h = time.substr(6,2);
    string m = time.substr(8,2);
    string s = time.substr(10,2);
    double tmp = hsrv::a2double(h) * 3600;
    tmp += hsrv::a2double(m) * 60;
    tmp += hsrv::a2double(s);
    return tmp;
}

string ArchiveThread::mkDayTime(double time) {
    int itime = (int) time;
    char buff[MAXB];
    int hour = itime/3600;
    int min = itime % 3600;
    min = min / 60;
    int sec = itime % 60 ;
    sprintf(buff, "%02d:%02d:%02d", hour, min, sec);
    return buff;
}



string ArchiveThread::mkDate(string day) {
    NameList myday;
    myday.init(day,'_');
    if(myday.size()<3) return "";
    string mydate = myday[2] + " - " + myday[1] + " - " + myday[0];
    return mydate;
}

bool ArchiveThread::mkHorusStatusPage(string& name, string& value, string& time) {
    string item = value+"; time="+time+")";
    page->add(name,item );
    page->add("language", "xml");
    AgentConf::pageRegister("/horus_status.xml", *page);
    page->add("language", "html");
    AgentConf::pageRegister("/horus_status", *page);
    return true;
}

bool ArchiveThread::isToTrace(string feature) {
    if(trace == NULL) return false;
    if(trace->get("all") == "yes") return true;
    if (trace->get(feature) == "yes") return true;
    return false;
}
