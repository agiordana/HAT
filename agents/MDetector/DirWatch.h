//
//  DummySource.h
//  soapagent
//
//  Created by Attilio Giordana on 2/4/13.
//  Copyright (c) 2013 Università del Piemonte Orientale. All rights reserved.
//

#ifndef _WATCH_DIR
#define _WATCH_DIR

#include "agentlib.h"
#include "ObserverThread.h"
#include <sys/inotify.h>
#include <boost/thread.hpp>


class FDesc {
public:
   std::string pathname;
   std::string event;
   std::string source;
   std::string status;
   FDesc() { pathname = ""; event = "";};
   FDesc(std::string p, std::string e) { pathname = p; event = e;};
   FDesc(std::string p, std::string s, std::string e, std::string f) { pathname = p; source = s; event = e; status = f; };
};

class DirToWatch: public MParams {
public:
   DirToWatch(std::string, std::string);
   std::string pathname;
   std::string name;
   std::string status;
   bool watch(int fd);
   int wd;
   bool setCamera(MMessage&);
   bool resetCamera();
   bool subjects(MParams&);
};


class FileNotify: setof<DirToWatch> {
public:
   std::string watchFile();
   bool init();
   FileNotify(std::string dir);
   std::vector<FDesc> fnotify();
   bool subjects(MParams&);
   bool getEventList(NameList&);
   bool setCamera(MMessage&);
   bool resetCamera();
private:
   int filedesc;
   std::string getPathname(int wd);
   std::string getName(int wd);
   std::string getStatus(int wd);
};

class DirWatch: public ObserverThread {
public:
    DirWatch(std::string n);
    ~DirWatch();
    void start_service_threads();
private:
    FileNotify* file_watcher;
    void do_work(DirWatch* obj);
    void do_service(DirWatch* obj);
    unsigned i;
    bool getSubjects(MParams& subj);
    bool cameraSetup(std::string);
    bool makeCameraDescriptor(std::string, std::string, std::string);
    bool makeEventDescriptor();
};

#endif
