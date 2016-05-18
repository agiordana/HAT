#include "UserLog.h"

using namespace std;

UserLog::UserLog(MParams& p) {
   NameList evtype;
   setof<MAttribute> xmlmap = p.extractCategory("log");
   for(size_t i=0; i<xmlmap.size(); i++) {
     evtype.clear();
     evtype.init(xmlmap[i].value,'+');
     for(size_t j=0; j<evtype.size(); j++) 
       if(logmap.count(evtype[j]) == 0) {
 	  logmap[evtype[j]] = new NameList();
          logmap[evtype[j]]->push_back(xmlmap[i].name);
       }
       else logmap[evtype[j]]->push_back(xmlmap[i].name);
     string path = hsrv::homedir+"/logs/"+xmlmap[i].name;
     FileManager::makeDir(path,true);
     path += "/data";
     pathname[xmlmap[i].name] = path;
   }
   map<string, NameList*>::iterator it;
   for(it = logmap.begin(); it!=logmap.end(); it++) {
      proto[it->first] = p.get(it->first,"proto");
   }
   setof<MAttribute> xmlsource = p.extractCategory("source");
   for(size_t i=0; i<xmlsource.size(); i++) {
      source[xmlsource[i].name] = new NameList();
      (*source[xmlsource[i].name]).init(xmlsource[i].value, '+');
   }
}

bool UserLog::log(MMessage& m) {
   string mtype = m.getString("class");
   size_t pos;
   string prototype;
   if(logmap.count(mtype) == 0) return false;
   if(proto.count(mtype)>0) prototype=proto[mtype];
   else {
      hsrv::logger->info("ERROR: no prototype for "+mtype);
      return false;
   }
   string key;
   string msubtype = m.getString("feature");
   if((pos = msubtype.find("_state")) != string::npos) msubtype = msubtype.substr(0, pos);
   m.add("mtype", mtype);
   m.add("msubtype", msubtype);
   if(m.getString("agent") != "") m.add("agentname", m.getString("agent"));
   if(m.getString("Ipower") != "") m.add("value", m.getString("Ipower"));
   string time = "20" + hsrv::getasciitimecompact();
   m.add("time", time);
   while((key=getKey(prototype))!="") {
      string wd = "$"+key;
      if(key=="program")hsrv::strReplace(prototype, wd, m.getString(key));
         else hsrv::strReplace(prototype, wd, remap(m.getString(key)));
   }
   for(size_t i=0; i<(*logmap[mtype]).size(); i++) 
     if(checkSource((*logmap[mtype])[i],m.getString("agentname"))){
       ofstream out;
       out.open(pathname[(*logmap[mtype])[i]].c_str(), ofstream::app);
       out<<prototype<<endl;
       out.close();
     }
   return true;
} 


string UserLog::remap(string v) {
   if(v=="ON") return "true";
   else if(v=="OFF") return "false";
   return v;
}

string UserLog::getKey(string& proto) {
   size_t pos;
   string key = "";
   pos = proto.find("$");
   if(pos == string::npos) return "";
   while(proto[++pos]!='"') key+=proto[pos];
   return key;
}

bool UserLog::checkSource(string target, string agent) {
   if(source.count(target)>0) return (*source[target]).member(agent);
   return true;
}
