//
//  DummySource.cpp
//  soapagent
//
//  Created by Attilio Giordana on 7/5/2015.
//  Copyright (c) 2015 Penta Dynamic Solutions. All rights reserved.
//

using namespace std;

#include "VirtualDevices.h"

VrtDevices::VrtDevices(string n) {
    id = n;
    loadDevices("devices");
    day = hsrv::getday();
}

VrtDevices::~VrtDevices() {

}

void VrtDevices::start_service_threads() {
    this->service_thread.push_back(new Thread(&VrtDevices::do_work, this));
}

void VrtDevices::do_work(VrtDevices* obj) {
    MMessage out_mess;
    MMessage::iterator it;
    MMessage msg;
    string current_time;
    string fname;
    freegpioRegister();
    makeDevicePage(dev_by_name);
    while(1) {
        msg = obj->receive_message();
//	cout <<"VrtDevices-----: "<< msg.xmlEncode(0)<<endl;
	if(msg.mtype == "cmd" && msg.msubtype == "reset") doReset(msg);
	else if(msg.mtype == "local_event" && msg.msubtype == "update") update(msg);
	else if(msg.mtype == "trigger" && msg.msubtype == "publish") publish();
	else if(msg.mtype == "trigger" && msg.msubtype == "measure") doMeasure();
	else if(msg.mtype == "trigger" && msg.msubtype == "archiveupd") doArchiveUpdate();
	else if(msg.mtype == "cmd" && msg.msubtype == "putConf" && hsrv::checkConfigureEnabled()) doReboot(msg);
	else if(msg.mtype == "cmd" && msg.msubtype == "devConfigure" && hsrv::checkConfigureEnabled()) devConfigure(msg);
	else if(msg.mtype == "cmd" && msg.msubtype == "devDelete" && hsrv::checkConfigureEnabled()) devDelete(msg);
        else if(msg.mtype == "cmd" && dev_by_name.count(msg.msubtype)>0) execcmd(msg);
        else if(msg.mtype == "set" && dev_by_name.count(msg.msubtype)>0) {
		setstatus(msg);
		makeDevicePage(dev_by_name);
	}
        else if(msg.mtype == "local_event" && dev_by_io.count(msg.msubtype)>0) execLocalEvent(msg);
        else if(msg.mtype == "wait" && msg.msubtype == "end") execwait(msg);
    }
}


bool VrtDevices::getObservations(MParams &obs) {
    ObserverThread::getObservations(obs);
    map<string,Device*>::iterator it;
    for(it=dev_by_name.begin(); it!=dev_by_name.end(); it++) it->second->getObservations(obs);
    return true;
}

bool VrtDevices::getSubjects(MParams &subj) {
    ObserverThread::getSubjects(subj);
    map<string,Device*>::iterator it;
    for(it=dev_by_name.begin(); it!=dev_by_name.end(); it++) it->second->getSubjects(subj);
    return true;
}

bool VrtDevices::loadDevices(string dir) {
    vector<string> devtype;
    string config_dir = hsrv::configdir+"/"+dir;
    string rawdevice_dir = hsrv::configdir+"/rawdevices";
    string rwadevices = "rawdevices";
    mkRawOutput(rawdevice_dir);
    mkRawInput(rawdevice_dir);
    loadRawOutput("rawdevices");
    loadRawInput("rawdevices");
    FileManager::dirList(config_dir, devtype);
    for(size_t i = 0; i < devtype.size(); i++) {
        loadDevType(dir, devtype[i]);
    }
}

bool VrtDevices::loadDevType(string dir1, string dir2) {
    vector<string> devlist;
    setof<MAttribute> input;
    setof<MAttribute> output;
    Device* tmp;
    string config_dir = hsrv::configdir+"/"+dir1+"/"+dir2;
    string dev_dir = dir1+"/"+dir2;
    FileManager::fileList(config_dir, devlist);
    for(size_t i = 0; i < devlist.size(); i++) {
        if(dir2 == "ElectricMeter") {
            tmp = new ElectricMeter(dev_dir, FileManager::getStem(devlist[i]));
            dev_by_name[tmp->name] = tmp;
            input = tmp->input();
            for(size_t j=0; j<input.size(); j++) {
	       dev_by_io[input[j].name] = tmp;
	    }
        }
        else if(dir2 == "FSMDevice") {
            tmp = new FSMDevice(dev_dir, FileManager::getStem(devlist[i]));
            dev_by_name[tmp->name] = tmp;
            input = tmp->input();
            for(size_t j=0; j<input.size(); j++) dev_by_io[input[j].name] = tmp;
        }
        else if(dir2 == "BinaryInput") {
            tmp = new BinaryInput(dev_dir, FileManager::getStem(devlist[i]));
            dev_by_name[tmp->name] = tmp;
            input = tmp->input();
            for(size_t j=0; j<input.size(); j++) dev_by_io[input[j].name] = tmp;
        }
        else if(dir2 == "AnalogInput") {
            tmp = new AnalogInput(dev_dir, FileManager::getStem(devlist[i]));
            dev_by_name[tmp->name] = tmp;
            input = tmp->input();
            for(size_t j=0; j<input.size(); j++) dev_by_io[input[j].name] = tmp;
        }
        else if(dir2 == "BinaryOutput") {
            tmp = new BinaryOutput(dev_dir, FileManager::getStem(devlist[i]));
            dev_by_name[tmp->name] = tmp;
            input = tmp->input();
            for(size_t j=0; j<input.size(); j++) dev_by_io[input[j].name] = tmp;
        }
        else if(dir2 == "Pwm") {
            tmp = new Pwm(dev_dir, FileManager::getStem(devlist[i]));
            dev_by_name[tmp->name] = tmp;
            input = tmp->input();
            for(size_t j=0; j<input.size(); j++) dev_by_io[input[j].name] = tmp;
        }
        else if(dir2 == "RGBPwm") {
            tmp = new RGBPwm(dev_dir, FileManager::getStem(devlist[i]));
            dev_by_name[tmp->name] = tmp;
            input = tmp->input();
            for(size_t j=0; j<input.size(); j++) dev_by_io[input[j].name] = tmp;
        }
        else if(dir2 == "TriStateOutput") {
            tmp = new TriStateOutput(dev_dir, FileManager::getStem(devlist[i]));
            dev_by_name[tmp->name] = tmp;
            input = tmp->input();
            for(size_t j=0; j<input.size(); j++) dev_by_io[input[j].name] = tmp;
        }
    }
    tmp = NULL;
    return true;
}

bool VrtDevices::loadRawOutput(string dir) {
    string rawoutput_dir = hsrv::configdir + "/" +dir+"/RawOutput";
    string dev_dir = dir +"/RawOutput";
    vector<string> devlist;
    setof<MAttribute> input;
    setof<MAttribute> output;
    Device* tmp;
    FileManager::fileList(rawoutput_dir, devlist);
    for(size_t i = 0; i < devlist.size(); i++) {
        tmp = new RawOutput(dev_dir, FileManager::getStem(devlist[i]));
        dev_by_name[tmp->name] = tmp;
        input = tmp->input();
        for(size_t j=0; j<input.size(); j++) dev_by_io[input[j].name] = tmp;
    }
    return true;
}

bool VrtDevices::loadRawInput(string dir) {
    string rawoutput_dir = hsrv::configdir + "/" +dir+"/RawInput";
    string dev_dir = dir +"/RawInput";
    vector<string> devlist;
    setof<MAttribute> input;
    setof<MAttribute> output;
    Device* tmp;
    FileManager::fileList(rawoutput_dir, devlist);
    for(size_t i = 0; i < devlist.size(); i++) {
        tmp = new RawInput(dev_dir, FileManager::getStem(devlist[i]));
        dev_by_name[tmp->name] = tmp;
	hsrv::publish(tmp->name, "0");
        input = tmp->input();
        for(size_t j=0; j<input.size(); j++) dev_by_io[input[j].name] = tmp;
    }
    return true;
}


bool VrtDevices::update(MMessage& msg) {
    if(dev_by_io.count(msg.getString("name")) > 0 ) {
	msg.msubtype = msg.getString("name");
	execLocalEvent(msg);
	return true;
    }
    return false;
}

bool VrtDevices::publish() {
   map<string,Device*>::iterator it;
   for(it=dev_by_name.begin(); it != dev_by_name.end(); it++) it->second->publish();
   return true;
}

bool VrtDevices::doMeasure() {
   map<string,Device*>::iterator it;
   for(it=dev_by_name.begin(); it != dev_by_name.end(); it++) it->second->doMeasure();
   return true;
}

bool VrtDevices::doReset(MMessage& m) {
   string dev = m.getString("device");
   string field = m.getString("field");
   if(dev_by_name.count(dev) > 0) return dev_by_name[dev]->reset(field); 
   else return false;
}

bool VrtDevices::doReset(string field) {
   map<string,Device*>::iterator it;
   for(it=dev_by_name.begin(); it != dev_by_name.end(); it++) it->second->reset(field);
   return true;
}

void VrtDevices::doReboot(MMessage& m) {
   NameList names;
   names.init(m.getString("url"), '/');
   if(names[0] == "devices"||names[1]=="devices") {
	exit(1);
   }
}
  
bool VrtDevices::execcmd(MMessage& msg) {
    if(dev_by_name.count(msg.msubtype)==0) return false;
    return dev_by_name[msg.msubtype]->execcmd(msg); 
}
  
bool VrtDevices::setstatus(MMessage& msg) {
    if(dev_by_name.count(msg.msubtype)==0) return false;
    return dev_by_name[msg.msubtype]->setstatus(msg); 
}
  
bool VrtDevices::execLocalEvent(MMessage& msg) {
    if(dev_by_io.count(msg.msubtype)==0) return false;
    return dev_by_io[msg.msubtype]->execcmd(msg); 
}
  
bool VrtDevices::execwait(MMessage& msg) {
    size_t i;
    string dev = msg.getString("value");
    for(i=0; i<dev.length()&&dev[i]!='_'; i++);
    if(i<dev.length()-2) dev.erase(0,i+1);
    for(i=0; i<dev.length()&&dev[i]!=SEPARATOR; i++);
    if(dev[i] == SEPARATOR) dev.erase(i);
    if(dev_by_name.count(dev)==0) return false;
    return dev_by_name[dev]->execcmd(msg); 
}

bool VrtDevices::freegpioRegister() {
   setof<MAttribute> used;
   MParams gpioactive("gpioactive", "gpioch");
   gpioactive.load();
   MParams gpioused("gpioactive", "gpioch");
   MParams gpiofree("gpioafree", "gpioch");
   map<string, Device*>::iterator it;
   for(it=dev_by_name.begin(); it!=dev_by_name.end(); it++) {
	used = it->second->input();	
        for(size_t i=0; i<used.size(); i++) {
		gpioused.add(used[i].name, used[i].value);
	}
	used = it->second->output();
        for(size_t i=0; i<used.size(); i++) gpioused.add(used[i].name, used[i].value);
   }
   for(size_t i=0; i<gpioactive.size(); i++) if(gpioused.get(gpioactive[i].name)=="") gpiofree.add(gpioactive[i].name, gpioactive[i].value);
   ofstream gpioch;
   gpioch.open("gpiochfree.xml");
   gpioch<< "<?xml version=\"1.0\"?><set type=\"gpioch\">";
   for(size_t i=0; i<gpiofree.size(); i++)
	gpioch<<"<attribute name=\""<<gpiofree[i].name<<"\" value=\""<<gpiofree[i].value<<"\"/>";
   gpioch << "</set>"<<endl;
   gpioch.close();
   string localurl = "gpiofree/gpioch.xml";
   string pathname = hsrv::homedir+"/gpiochfree.xml";
   hsrv::archive->addResource(localurl, pathname, true);
   FileManager::deleteFile(hsrv::homedir, "gpiochfree.xml");
   return true;
}

bool VrtDevices::devDelete(MMessage& m) {
   string devicepath = hsrv::configdir+"/devices/";
   string devicehome = hsrv::homedir+"/devices/";
   vector<string> devicegroups;
   bool done = false;
   FileManager::dirList(devicepath, devicegroups);
   for(size_t i=0; i<devicegroups.size() && !done; i++) {
      string cdir = devicepath+"/"+devicegroups[i];
      string hdir = devicehome+"/"+devicegroups[i];
      done = devRemove(cdir, hdir, m);
   }
   if(done) exit(1);
   return true;
}

bool VrtDevices::devRemove(string cpath, string hpath, MMessage& m) {
   NameList devset;
   FileManager::fileList(cpath, devset);
   string name = m.getString("name");
   if(name=="") name = m.getString("value");
   deleteManifest(name);
   name += ".xml";
   if(devset.member(name)) {
      FileManager::deleteFile(cpath, name);
      FileManager::deleteFile(hpath, name);
      return true;
   }
   return false;
}

bool VrtDevices::devConfigure(MMessage& m) {
   bool iscomment = false;
   ifstream devtemplate;
   ofstream device;
   string devname = m.getString("devtype");
   string templatepath = hsrv::configdir+"/templates/"+m.getString("devtype")+".xml";
   string devicepath = hsrv::configdir+"/devices/";
   string prefix = "";
   string oldvalue;
   string newvalue;
   size_t i = 0;
   if(devname == "") {
	devname = m.getString("value");
        templatepath = hsrv::configdir+"/templates/"+m.getString("value")+".xml";
   }
   char ch;
   mkManifest(m);
   devtemplate.open(templatepath.c_str());
   if(!devtemplate.is_open()) return false;

   string description;
   while((ch=devtemplate.get())&&!devtemplate.eof()) {
      description+=ch;
   }
   devtemplate.close();
   size_t pointer = description.find("semantic_class");
   if(pointer==string::npos) {
	hsrv::logger->error("Wrong template: doesn't contain the semantic category");
	return false;
   }
   while(pointer<description.size()&&description[pointer++]!='=');
   if(pointer >= description.size()) {
        hsrv::logger->error("Wrong template: semantic category non correctly specified");
        return false;
   }
   while(pointer<description.size()&&description[pointer]!=']'&&description[pointer]!='-'&&description[pointer]!='\r'&&description[pointer]!='\n') {
	if(description[pointer]!=' ') devicepath+=description[pointer];
	pointer++;
   }
   if(pointer >= description.size()||(description[pointer]!=']'&&description[pointer]!='-')) {
        hsrv::logger->error("Wrong template: semantic category non correctly specified");
        return false;
   }
   if(description[pointer]=='-') iscomment = true;
   while(description[pointer]!='<'&&pointer<description.size()) pointer++;
   if(pointer >= description.size()) {
        hsrv::logger->error("Wrong template: semantic category non correctly specified"); 
        return false;
   }
   if(!iscomment) description.erase(0,pointer);
   while(removeComment(description));
   MMessage::iterator it;
   string duration = "0";
   string pduration = "0";
   for(it=m.begin(); it!=m.end(); it++) {
      oldvalue = it->first;
      newvalue = m.getString(it->first);
      if(oldvalue[0]=='$') while(hsrv::strReplace(description,oldvalue,newvalue));
      if(oldvalue=="$NAME") devname=newvalue;
      if(oldvalue=="$PREFIX") prefix=newvalue;
      if(oldvalue=="$DURATION") duration = newvalue;
      if(oldvalue=="$PDURATION") pduration = newvalue;
   }
   if(duration == "0") while(hsrv::strReplace(description,"$DURATION",duration));
   if(pduration == "0") while(hsrv::strReplace(description,"$PDURATION",pduration));
   devicepath = devicepath +"/"+prefix+"_"+devname+".xml";
   device.open(devicepath.c_str());
   if(!device.is_open()) {
      hsrv::logger->info("Impossible to save: "+devicepath);
      return false;
   }
   pointer = 0;
   while(pointer<description.size()) device.put(description[pointer++]);
   device.close();
   exit(1);
   return true;
}


bool VrtDevices::doArchiveUpdate() {
   string currentday = hsrv::getday();
   string yesterday = hsrv::getday(-900);

   hsrv::logger->info("ARCHIVEUPD: "+day+" "+currentday+" "+yesterday);
   if(currentday == day) {
      return true;
   }
   hsrv::logger->info("ARCHIVEPURGE: "+day+" "+currentday+" "+yesterday);
   if(yesterday != day) {
      day = currentday;
      return true;
   }

   // operazioni da effettuare se e' cambiata la data (prima operazione del giorno)
   hsrv::archive->purge();
   hsrv::logger->info("Archive PURGE!!!! "+day + " " +currentday);
   day = currentday;
   doReset("energy");
   return true;
}

bool VrtDevices::removeComment(string& description) {
   size_t endpos;
   size_t pos = description.find("<!--");
   if(pos == string::npos) return false;
   endpos = description.find("-->");
   if(endpos < pos || endpos == string::npos) return false;
   while(description[endpos] != '<') endpos++;
   description.erase(pos, endpos - pos);
   return true;
}

bool VrtDevices::mkManifest(MMessage& m) {
   ofstream manifest;
   ostringstream  tmanifest;
   string name = m.getString("$PREFIX") + "_" + m.getString("$NAME");
   string manifestpath = hsrv::configdir+"/devmanifest/";
   if(!FileManager::isDir(manifestpath)) FileManager::makeDir(manifestpath, true);
   manifestpath += name;
   manifestpath += ".xml";
   manifest.open(manifestpath.c_str());
   m.xmlPrint(tmanifest,1);
   manifest<<tmanifest.str();
   manifest.close();
   return true;
}

bool VrtDevices::deleteManifest(string name) {
  string manifestpath = hsrv::configdir+"/devmanifest/";
  FileManager::deleteFile(manifestpath, name+".xml");
  return true;
}

bool VrtDevices::mkRawOutput(string dir) {
  string path = dir + "/RawOutput";
  if(!FileManager::isDir(path)) FileManager::makeDir(path, true);
  string rmcmd = "rm -f "+path + "/*.xml";
  hsrv::cmdExec(rmcmd);
  string gpio = hsrv::configdir + "/gpioout";
  vector<string> filelist;
  FileManager::fileList(gpio, filelist);
  for(size_t i=0; i<filelist.size(); i++) {
    if(filelist[i].find("out") != string::npos) {
      string source = gpio+"/"+filelist[i];
      FileManager::copyFile(source, path, filelist[i]);
    }
  }
  filelist.clear();
  gpio = hsrv::configdir + "/pwmout";
  FileManager::fileList(gpio, filelist);
  for(size_t i=0; i<filelist.size(); i++) {
    if(filelist[i].find("PWM") != string::npos) {
      string source = gpio+"/"+filelist[i];
      FileManager::copyFile(source, path, filelist[i]);
    }
  }
  return true;
}

bool VrtDevices::mkRawInput(string dir) {
  string path = dir + "/RawInput";
  if(!FileManager::isDir(path)) FileManager::makeDir(path, true);
  string rmcmd = "rm -f "+path + "/*.xml";
  hsrv::cmdExec(rmcmd);
  string gpio = hsrv::configdir + "/gpioin";
  vector<string> filelist;
  FileManager::fileList(gpio, filelist);
  for(size_t i=0; i<filelist.size(); i++) {
    if(filelist[i].find("in") != string::npos) {
      string source = gpio+"/"+filelist[i];
      FileManager::copyFile(source, path, filelist[i]);
    }
  }
  return true;
}

bool VrtDevices::makeDevicePage(map<string, Device*>& dev) {
    vector< pair<string,string> >  page;
    map<string, Device*>::iterator it;
    string status;

    for(it = dev.begin(); it != dev.end(); it++) {
        status = it->second->status;
        if(status == "") status = "ON";
        pair<string, string> p(it->first, status);
        page.push_back(p);
    }
    hsrv::saveItemList("devices", "status", page);
    return true;
}
