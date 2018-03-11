//
//  DirWatch.cpp
//  soapagent
//
//  Created by Attilio Giordana on 2/4/13.
//  Copyright (c) 2013 Università del Piemonte Orientale. All rights reserved.
//

using namespace std;

#include "DirWatch.h"

#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define BUF_LEN     ( 16000 * ( EVENT_SIZE + 16 ) )


/*------------------- class DirToWatch --------------------------*/

DirToWatch::DirToWatch(string dir, string nm): MParams::MParams(dir, nm) {
   load();
   name = nm;
   wd = 0;
   pathname = get("pathname");
   status = get("status");
   if(status == "") status = "ON";
}

bool DirToWatch::subjects(MParams& subj) {
   subj.add(name, "subject", "local_event");
   return true;
}


bool DirToWatch::watch(int filedesc) {
   wd = inotify_add_watch( filedesc, pathname.c_str(), IN_MODIFY | IN_CREATE | IN_DELETE );
   return true;
}


bool DirToWatch::setCamera(MMessage& m) {
   if(m.getString(name) != "") {
	status = m.getString(name);
	add("status", status);
	save();
	return true;
   }
   return false;
}

bool DirToWatch::resetCamera() {
   status = "ON";
   add("status", "ON");
   save();
   return true;
}


/*------------------- class FileNotify --------------------------*/
FileNotify::FileNotify(string dir) {
   vector<string> filelist;
   string config_dir = hsrv::configdir+"/"+dir;

   FileManager::fileList(config_dir, filelist);

   for(size_t i = 0; i < filelist.size(); i++) {
	insert(end(),DirToWatch(dir, FileManager::getStem(filelist[i])));
   }
}

bool FileNotify::init() {
   filedesc = inotify_init();
   if(filedesc>=0) for(size_t i=0; i<size(); i++) (*this)[i].watch(filedesc);
   return (filedesc>=0);
}

string FileNotify::getPathname(int wd) {
   for(unsigned i=0; i<size(); i++) if((*this)[i].wd == wd) return (*this)[i].pathname;
   return "";
}

string FileNotify::getName(int wd) {
   for(unsigned i=0; i<size(); i++) if((*this)[i].wd == wd) return (*this)[i].name;
   return "";
}

string FileNotify::getStatus(int wd) {
   for(unsigned i=0; i<size(); i++) if((*this)[i].wd == wd) return (*this)[i].status;
   return "";
}

vector<FDesc> FileNotify::fnotify() {
   char buffer[BUF_LEN+1];
   vector<FDesc> tmp;
   int length;
   int i=0;
   string evdesc;
   string path;
   int bl = BUF_LEN;
   length = read( filedesc, buffer, bl );
   if(length < 0) return tmp;
   while( i < length ) {
	struct inotify_event *event = ( struct inotify_event * ) &buffer[ i ];
	if(event->len) {
		evdesc = "";
		if((event->mask & IN_CREATE) && (event->mask & IN_ISDIR)) evdesc = "dcreate";
		else if((event->mask & IN_CREATE) && !(event->mask & IN_ISDIR)) evdesc = "fcreate";
		else if((event->mask & IN_DELETE) && (event->mask & IN_ISDIR)) evdesc = "ddelete";
		else if((event->mask & IN_DELETE) && !(event->mask & IN_ISDIR)) evdesc = "fdelete";
		else if((event->mask & IN_MODIFY) && (event->mask & IN_ISDIR)) evdesc = "dmodified";
		else if((event->mask & IN_MODIFY) && !(event->mask & IN_ISDIR)) evdesc = "fmodified";
		path = getPathname(event->wd);
		path = path + "/" + string(event->name);
		tmp.push_back(FDesc(path, getName(event->wd), evdesc, getStatus(event->wd)));
	}
	i += EVENT_SIZE + event->len;
   }
   return tmp;
}
   
 
bool FileNotify::subjects(MParams& subj) {
   for(size_t i=0; i<size(); i++) (*this)[i].subjects(subj);
   return true;
}
   
bool FileNotify::getEventList(NameList& events) {
   for(size_t i=0; i<size(); i++) events.push_back((*this)[i].name+"="+(*this)[i].status);
   return events.size()>0;
}

bool FileNotify::setCamera(MMessage& m) {
   for(size_t i=0; i<size(); i++) (*this)[i].setCamera(m);
   return true;
}


bool FileNotify::resetCamera() {
   for(size_t i=0; i<size(); i++) (*this)[i].resetCamera();
   return true;
}

/*------------------- class DirWatch --------------------------*/

DirWatch::DirWatch(string n) {
    id = n;
    cameraSetup("cameras");
    file_watcher = new FileNotify("cameras");
    i = 0;
    makeEventDescriptor();
}

DirWatch::~DirWatch() {

}

void DirWatch::start_service_threads() {
    this->service_thread.push_back(new Thread(&DirWatch::do_work, this));
//    this->service_thread.push_back(new Thread(&DirWatch::do_service, this));
}

void DirWatch::do_work(DirWatch* obj) {
    MMessage msg;
    unsigned i;
    vector<FDesc> events;
    file_watcher->init();
    while(1) {
	events = file_watcher->fnotify();
	for(i=0; i<events.size(); i++) {
	  if(events[i].event == "fcreate") {
    	    MMessage out_mess;
	    out_mess.mtype = "local_event";
	    out_mess.msubtype = "motion";
	    out_mess.add("event_source", "SE_"+events[i].source);
	    out_mess.add("pathname", events[i].pathname);
	    out_mess.add("value", "1");
	    out_mess.add("agent", hsrv::agentname);
	    out_mess.add("status", events[i].status);
            SubjectSet::notify(out_mess);
	  }
	}
    }
}

void DirWatch::do_service(DirWatch* obj) {
   MMessage msg;
   while(1) {
	msg = receive_message();
	if(msg.mtype == "cmd" && msg.msubtype == "setcamera") {
	   file_watcher->setCamera(msg);
	   makeEventDescriptor();
	}
	else if(msg.mtype == "cmd" && msg.msubtype == "resetcamera") {
	   file_watcher->resetCamera();
	   makeEventDescriptor();
	}
   }
}

bool DirWatch::getSubjects(MParams &subj) {
    ObserverThread::getSubjects(subj);
    file_watcher->subjects(subj);
    return true;
}


bool DirWatch::makeCameraDescriptor(string dir, string root, string camera ) {
   string descriptor = dir+"/"+camera+".xml";
   if(!FileManager::isFile(descriptor)) {
     ofstream out(descriptor.c_str());
     out<<"<\?xml version=\"1.0\" \?><set type=\"camera\"><attribute name=\"pathname\" value=\""<<root<<"/"<<camera<<"\"/><attribute name=\"event\" value=\"SE_"<<camera<<"\"/> </set>"<<endl;
     out.close();
   }
   hsrv::publish("SE_"+camera, "type=\"source\" value=\"OFF\" status=\"ON\"");
   return true;
}


bool DirWatch::cameraSetup(string cameras) {
   string cameradir = hsrv::configdir + "/" +cameras;
//   if(FileManager::isDir(cameradir)) {
//	if(FileManager::fileCount(cameradir) >0) return true;
//   }
//   else FileManager::makeDir(cameradir, true);
   FileManager::makeDir(cameradir, true);
   string root = hsrv::homedir+"/cameras";
   if(!FileManager::isDir(root)) { cout<<"videoserver area: "<<root<<" not found!"<<endl; exit(-1); }
   vector<string> cameralist;
   FileManager::dirList(root, cameralist);
   for(size_t i=0; i<cameralist.size(); i++) makeCameraDescriptor(cameradir, root, cameralist[i]);
   return true;
}

bool DirWatch::makeEventDescriptor() {
   NameList events;
   ofstream description;
   NameList tmp;
   file_watcher->getEventList(events);
   description.open("eventlist.xml");
   description << "<set type=\"events\">";
   for(size_t i=0; i<events.size(); i++) {
	tmp.clear();
        tmp.init(events[i], '=');
	if(tmp.size()==2) description << "<attribute name=\"SE_" <<tmp[0]<<"\" value=\""<<tmp[1]<<"\"/>";
	if(tmp.size()==1) description << "<attribute name=\"" <<tmp[0]<<"\" value=\"ON\"/>";
   }
   description << "</set>"<<endl;
   description.close();
   string localurl = "events/data";
   string pathname = hsrv::homedir+"/eventlist.xml";
   hsrv::archive->addResource(localurl, pathname, true);
   FileManager::deleteFile(hsrv::homedir, "eventlist.xml");
   return true;
}
