//
//  GPIOThread.cpp
//
//  Created by Attilio Giordana on 7/5/15.
//  Copyright (c) 2015 Penta Dynamic Solutions. All rights reserved.
//

using namespace std;

#include "GPIOThread.h"

GPIOThread::GPIOThread(string n) {
    id = n;
    Gpio* tmp;
    vector<string> filelist;
    MParams par("threads",n);
    par.load();
    MkOnewire(par.get("onewiredir"));
    string gpioin = hsrv::configdir+"/gpioin";
    FileManager::fileList(gpioin, filelist);
    for(size_t i=0; i<filelist.size(); i++) {
	tmp = new Gpio("gpioin", FileManager::getStem(filelist[i]));
        gpio.push_back(tmp);
	input.push_back(tmp);
    }
    string gpioout = hsrv::configdir+"/gpioout";
    filelist.clear();
    FileManager::fileList(gpioout, filelist);
    for(size_t i=0; i<filelist.size(); i++) {
	tmp = new Gpio("gpioout", FileManager::getStem(filelist[i]));
        gpio.push_back(tmp);
	output.push_back(tmp);
    }
    filelist.clear();
    string pwmdir = hsrv::configdir+"/pwmout";
    FileManager::fileList(pwmdir, filelist);
    for(size_t i=0; i<filelist.size(); i++) {
	tmp = new Gpio("pwmout", FileManager::getStem(filelist[i]));
        gpio.push_back(tmp);
	pwmout.push_back(tmp);
    }
    MkExport();
    gpioRegister();
    pfd = (struct pollfd*)malloc(sizeof(struct pollfd)*input.size());
    probe_polling = new MMessageQueue<MMessage>(3);
}

GPIOThread::~GPIOThread() {

}

void GPIOThread::start_service_threads() {
    if(hsrv::debug<2) this->service_thread.push_back(new Thread(&GPIOThread::do_work, this));
    this->service_thread.push_back(new Thread(&GPIOThread::do_work1, this));
    this->service_thread.push_back(new Thread(&GPIOThread::do_work2, this));
}

void GPIOThread::do_work(GPIOThread* obj) {
    int value;
    MMessage out_mess;
    for(size_t i=0; i<input.size(); i++) {
        pfd[i].fd = open(input[i]->gpiovalue.c_str(), O_RDONLY);
        pfd[i].events = POLLPRI;
        pfd[i].revents = 0;
    }
    for(size_t i=0; i<input.size(); i++) {
      value = get_current_value(pfd[i].fd);
      input[i]->currentValue = hsrv::int2a(value);
      hsrv::publish(input[i]->name, input[i]->currentValue);
    }
    while(1) {
        int ready = poll(pfd, (int)input.size(), -1);
        for(size_t i=0; i<input.size(); i++)
            if(pfd[i].revents != 0) {
                value = get_current_value(pfd[i].fd);
                string asciivalue = hsrv::int2a(value);
                double tt = hsrv::gettime();
		if(input[i]->wait_until <= tt && asciivalue != input[i]->currentValue) {
                   out_mess.mtype = "local_event";
                   out_mess.msubtype = "update";
                   out_mess.add("gpio", input[i]->gpioname);
                   out_mess.add("name", input[i]->name);
                   out_mess.add("value", asciivalue);
                   string asciitime = hsrv::double2a(tt);
                   out_mess.add("time", asciitime);
		   input[i]->currentValue = asciivalue;
                   if(input[i]->wait_until>0)SubjectSet::notify(out_mess);
		   hsrv::publish(input[i]->name, asciivalue);
		   input[i]->wait_until = tt + input[i]->w;
		   out_mess.clear();
	 	}
            }
    }
}
    
void GPIOThread::do_work1(GPIOThread* obj) {
    MMessage msg;
    for(size_t i=0; i<output.size(); i++) output[i]->setInitialValue();
    for(size_t i=0; i<pwmout.size(); i++) pwmout[i]->setInitialValue();
    while(true) {
//	sleep(1);
	msg = obj->receive_message();
	if(msg.mtype=="devcmd") {
	   string dev=msg.msubtype;
	   string value = msg.getString("value");
	   for(size_t i=0; i<output.size(); i++) 
	      if(msg.msubtype == output[i]->name) {
		 output[i]->setCurrentValue(value);
//		 hsrv::publish(output[i]->name, value);
		 break;
	      }
	   for(size_t i=0; i<pwmout.size(); i++) {
	      if(msg.msubtype == pwmout[i]->name) {
		 pwmout[i]->setCurrentValue(value);
//		 hsrv::publish(pwmout[i]->name, value);
		 break;
	      }
	   }
         }
         else if (msg.mtype=="trigger" && msg.msubtype=="pollw1") {
	    probe_polling->send(msg);
	 }
    }
}

void GPIOThread::do_work2(GPIOThread* obj) {
    MMessage msg;
    MMessage out_msg;
    string cmd;
    string value;
    size_t i = 0;
    for(size_t j=0; j<onewire.size(); j++) {
	onewire[j]->currentValue = "0";
	hsrv::publish(onewire[j]->name, onewire[j]->currentValue);
    }
    while(true) {
        msg = probe_polling->receive();
        if(onewire.size() > 0 && msg.mtype=="trigger" && msg.msubtype == "pollw1") {
           if(i>=onewire.size()) i=0;
           out_msg.mtype="local_event";
           out_msg.msubtype="update";
           cmd = onewire[i]->get("poll","script");
           value = hsrv::cmdExec(cmd);
	   if(value != "") {
	      onewire[i]->currentValue = getAsciiValue(value, onewire[i]->get("valuekw","script"));
	      hsrv::publish(onewire[i]->name, onewire[i]->currentValue);
              out_msg.add("gpio", onewire[i]->gpioname);
              out_msg.add("name", onewire[i]->name);
              out_msg.add("value", onewire[i]->currentValue);
              SubjectSet::notify(out_msg);
	   }
           out_msg.clear();
           i++;
        }
    }
}

string GPIOThread::getAsciiValue(string& s, string kw) {
   string value = "";
   size_t pos = s.find(kw);
   if(pos == string::npos) return "";
   else {
     for(size_t i=pos+kw.size(); i<s.size()&&s[i]!='\r'&&s[i]!='\n'&&s[i]!=' '; i++) {
	value+=s[i];
     }
   }
   return value;
}

bool GPIOThread::getObservations(MParams &obs) {
    ObserverThread::getObservations(obs);
    return true;
}

bool GPIOThread::getSubjects(MParams &subj) {
    ObserverThread::getSubjects(subj);
    return true;
}

bool GPIOThread::MkExport() {
    for(size_t i=0; i<gpio.size(); i++)
        gpio[i]->initialise();
    for(size_t i=0; i<gpio.size(); i++)
        gpio[i]->program();
    return true;
}

int GPIOThread::get_current_value(int fd) {
    int value;
    lseek(fd, 0, 0);

    char buffer[1024];
    int size = read(fd, buffer, sizeof(buffer)-1);
    if (size != -1) {
        buffer[size] = '\0';
        value = atoi(buffer);
    }
    else {
        value = -1;
    }
    return value;
}

/*
bool GPIOThread::set_current_value(string out, string v) {
   FILE *chout;
   if(hsrv::debug >= 1) cout<<"WRITING: "<<out<<"="<<v<<endl;
   chout = fopen(out.c_str(), "w");
   if(chout == NULL) return false;
   fprintf(chout, "%s", v.c_str());
   fclose(chout);
   return true;
}
*/

bool GPIOThread::gpioRegister() {
   ofstream gpioch;
   gpioch.open("gpioch.xml");
   gpioch<< "<?xml version=\"1.0\"?><set type=\"gpioch\">";
   for(size_t i=0; i<gpio.size(); i++) {
	gpioch<<"<attribute name=\""<<gpio[i]->name<<"\" value=\""<<gpio[i]->gpiotype<<"\"/>";
        string defv = gpio[i]->get("default_value");
	if(defv == "") defv = "0";
	hsrv::publish(gpio[i]->name, defv);
   }
   for(size_t i=0; i<onewire.size(); i++) {
	gpioch<<"<attribute name=\""<<onewire[i]->name<<"\" value=\""<<onewire[i]->gpiotype<<"\"/>";
        string defv = onewire[i]->get("default_value");
	if(defv == "") defv = "0";
	hsrv::publish(onewire[i]->name, defv);
   }
   gpioch << "</set>"<<endl;
   gpioch.close();
   string localurl = "gpioactive/gpioch.xml";
   string pathname = hsrv::homedir+"/gpioch.xml";
   hsrv::archive->addResource(localurl, pathname, true);
   FileManager::deleteFile(hsrv::homedir, "gpioch.xml");
   return true;
}

bool GPIOThread::MkOnewire(string dir) {
   vector<string> owdev;
   vector<string> sysowdev;
   map<string, bool> consistent;
   string owdir = hsrv::configdir + "/onewire";
   string tmpname;
   Gpio* tmp;
   if(dir == "") return false;
   FileManager::fileList(owdir, owdev);
   if(hsrv::debug<2) FileManager::fileList(dir, sysowdev);
   for(size_t i=0; i<owdev.size(); i++) {
	tmp = new Gpio("onewire", FileManager::getStem(owdev[i]));
	onewire.push_back(tmp);
   }
   if(hsrv::debug > 1) return true;
   for(size_t i=0; i<sysowdev.size(); i++) {
      if(sysowdev[i] != "w1_bus_master1" && (tmpname = isDefined(sysowdev[i])) =="") {
	tmpname = mkName();
	owDefine(dir,tmpname,sysowdev[i]);
	tmp = new Gpio("onewire",tmpname);
	onewire.push_back(tmp);
        consistent[tmpname] = true;
      }
      consistent[tmpname] = true;
   }
   return true;
}

string GPIOThread::isDefined(string sysname) {
   for(size_t i=0; i<onewire.size(); i++) {
	if(onewire[i]->gpioname == sysname) return onewire[i]->name;
   }
   return "";
}

bool GPIOThread::owDefine(string dir, string name, string gpioname) {
   string templ="";
   char ch;
   string templpath = hsrv::configdir + "/templates/OneWire.xml";
   string devpath = hsrv::configdir + "/onewire/" + name + ".xml";
   ifstream in(templpath.c_str());
   if(!in.is_open()) return false;
   while((ch=in.get())&&!in.eof()) {
      templ+=ch;
   }
   in.close();
   while(hsrv::strReplace(templ,"$OWDIR",dir));
   while(hsrv::strReplace(templ,"$NAME",name));
   while(hsrv::strReplace(templ,"$GPIONAME",gpioname));
   ofstream out(devpath.c_str());
   if(!out.is_open()) return false;
   for(size_t i=0; i<templ.size(); i++) out.put(templ[i]);
   out.close();
   return true;
}

bool GPIOThread::owPurge(vector<string>& defined, map<string, bool>& consistent) {
   string cmd;
   string devfile;
   for(size_t i=0; i<defined.size(); i++)
     if(consistent.count(defined[i]) <=0) {
        devfile = hsrv::configdir + "/onewire/" + defined[i] + ".xml";
        cmd = "rm -f "+devfile;
        hsrv::cmdExec(cmd);
        for(size_t j=0; j<onewire.size(); j++)
	   if(onewire[j]->gpioname == defined[i]) {
		onewire.erase(onewire.begin()+j);
		break;
           }
      }
   return true;
}

string GPIOThread::mkName() {
   string tname = "";
   for(size_t i=0; i<onewire.size() && tname == ""; i++) {
     tname = "PR" + hsrv::unsigned2a(i+1);
     for(size_t j=0; j<onewire.size(); j++) {
	if(tname == onewire[j]->name) {
	   tname = "";
        }
     }
   }
   if(tname == "") tname = "PR" + hsrv::unsigned2a(onewire.size() + 1);
   return tname;
}
