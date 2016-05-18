using namespace std;

#include "Translator.h"

Translator::Translator(string file): MParams::MParams(file) {
   load();
}

string Translator::dummy() {
   stringstream ss;
   ss<<"ciao ciao";
   return ss.str();
}

string Translator::log2text(boost::property_tree::ptree::value_type& v) {
   string evtype = v.second.get<string>("type");
   string res = get(evtype,"proto");
   if(res=="") return "";
   string key;
   while((key = getKey(res)) != "") {
      string currentval = v.second.get<string>(key);
      if(key=="time") currentval = timeFormat(currentval);
      string wd = "$"+key;
      string nwd = remap(evtype, currentval);
      hsrv::strReplace(res,wd,nwd); 
   }
   return res;
}

string Translator::getKey(string& proto) {
   size_t pos;
   string key = "";
   char delim;
   pos = proto.find("$");
   if(pos == string::npos) return "";
   while((pos+1) < proto.size() && (delim = proto[++pos])!=' '&&delim != ','&&delim != ';'&&delim!='"'&&delim!='.') key+=delim;
   return key;
}

string Translator::remap(string& category, string& v) {
   string value = get(v, category);
   if(value == "") value = clean(v);
   return value;  
}

string Translator::clean(string& v) {
   string res;
   size_t pos;
   if(v[2]=='_') res=v.substr(3);
      else res=v;
   if((pos=res.find("_"))!=string::npos) res = res.substr(0,pos);
   return res;
}


string Translator::timeFormat(string& t) {
  string res = "";
  if(t.size()<14) return t;
  res = t.substr(6,2)+"-";
  res += (t.substr(4,2)+"-");
  res += (t.substr(0,4)+" - ");
  res += (t.substr(8,2)+":");
  res += (t.substr(10,2)+":");
  res += (t.substr(12,2));
  return res;
}
