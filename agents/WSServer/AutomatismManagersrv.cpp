using namespace std;

#include "AutomatismManager.h"

bool AutomatismManager::isDigit(char c) {
  return (c=='0' || c=='1' || c=='2' || c=='3' || c=='4' || c=='5' || c=='6' || c=='7' || c=='8' || c=='9');
}

string AutomatismManager::mkExternalName(int mode, string sname, string ecname) {
   string ext_name;
   NameList sensor_name;
   string rnumber = "";
   sensor_name.init(sname, '_');
   if(mode == 0&&sensor_name.size()>1) {
	ext_name = sensor_name[0]+"_000"+sensor_name[1];
	return ext_name;
   }
   if(mode >0 && sensor_name.size()>1) {
	ext_name = sensor_name[0]+ "_";
        for(size_t i=ecname.size(); i>0; i--) if(isDigit(ecname[i])) rnumber += ecname[i];
	for(size_t i=0; i<3-rnumber.size(); i++) ext_name += '0';
	ext_name += rnumber;
	return ext_name;
   }
   return "";
}

string AutomatismManager::getBody(string message) {
   string body = message.substr(message.find("\r\n\r\n")+4);
   return body;
}

string AutomatismManager::getFile(string url) {
   string answer;
   MMessage mreq;
   mreq.add("url", url);
   answer = hsrv::archive->getFile(mreq);
   return answer;
}

bool AutomatismManager::checkAndMakePath(string& path) {
   NameList plist;
   plist.init(path, '/');
   if(plist.size() < 1 || plist[0] == "") return false;
   string fullpath = hsrv::homedir +"/" + plist[0];
   if(!FileManager::isDir(fullpath)) return false;
   for(size_t i=1; i<plist.size(); i++) {
     fullpath += ("/"+plist[i]);
     if(!FileManager::isDir(fullpath)) FileManager::makeDir(fullpath,false);
   }
   return true;
}

string AutomatismManager::makeGetAnswer(string& answer) {
   string res = par.get("get_answer");
   string tmp = answer;
   size_t i;
   if(tmp.size() == 0) return "";
   for(i=0; i<tmp.size()&&tmp[i] != '{'; i++);
   if(tmp[i] == '{') tmp.erase(0, i+1);
   for(i=tmp.size()-1; i>0 && tmp[i] != '}'; i--);
   if(i>0) tmp.erase(i, string::npos);
   hsrv::strReplace(res,"$BODY", tmp);
   return res;
}

string AutomatismManager::loadTemplate(string file) {
   string res;
   ifstream in(file.c_str());
   if(!in.is_open()) return "";
   char ch;
   while((ch=in.get())&&!in.eof()) {
      res += ch;
   }
   in.close();
   return res;
}

string AutomatismManager::mkResource(string& file) {
   ofstream out;
   string tmp = hsrv::homedir + "/_tmp.Automatism";
   out.open(tmp.c_str());
   out<<file<<endl;
   out.close();
   return tmp;
}

string AutomatismManager::extractString(string& src, char first, char last) {
   string tmp = src;
   size_t i;
   if(tmp.size() == 0) return "";
   for(i=0; i<tmp.size()&&tmp[i] != first; i++);
   if(tmp[i] == first) tmp.erase(0, i+1);
   for(i=tmp.size()-1; i>0 && tmp[i] != last; i--);
   if(i>0) tmp.erase(i, string::npos);
   return tmp;
}

string AutomatismManager::sendCmd(string file) {
   string request = "POST / HTTP/1.1\r\nContent-Length: "+hsrv::unsigned2a(file.size())+"\r\n\r\n"+file+"\r\n\r\n";
   string answer = NameTable::forward(request, par.get("automatismag"));
   return answer;
}

bool AutomatismManager::sendFile(string url, string file) {
   string request = "POST "+url+" HTTP/1.1\r\nContent-Length: "+hsrv::unsigned2a(file.size())+"\r\n\r\n"+file+"\r\n\r\n";
   string answer = NameTable::forward(request, par.get("automatismag"));
   if(answer.find("Not Found") == string::npos &&
	(answer.find("list") != string::npos || answer.find("done") != string::npos || answer.find("OK") != string::npos)) return true;
   return false;
}

string AutomatismManager::fileDownload(string url) {
   string request = "GET " + url + " HTTP/1.1\r\n\r\n\r\n";
   string answer = NameTable::forward(request, par.get("automatismag"));
   return getBody(answer);
}

string AutomatismManager::extractValue(string& body, string keyword) {
   size_t pos = body.find(keyword);
   if(pos == string::npos) return "";
   while(pos<body.size() && body[pos]!='=') pos++;
   while(pos<body.size() && body[pos]!='"') pos++;
   pos++;
   string res="";
   while(pos<body.size() && body[pos] != '"' && body[pos] != ',') res+=body[pos++];
   return res;
}

string AutomatismManager::mkAnswerBody(string& src, string category, char mark) {
   string body = "";
   string alias;
   string name;
   string status;
   string from;
   string to;
   string itemstatus;
   string tcategory;
   size_t pos = 0;
   size_t newpos = 0;
   while((newpos = src.find("item", pos)) != string::npos) {
      pos = newpos+4;
      string lsrc = src.substr(pos);
      name = extractValue(lsrc,"name");
      if(name!="" && name[0] == mark) {
        alias = extractValue(lsrc,"alias");
        if(alias.size()>3) tcategory = alias.substr(0,2);
        else tcategory = "";
        if(tcategory == category) {
           status = extractValue(lsrc,"status");
           from = cvtimex2j(extractValue(lsrc,"from="));
           to = cvtimex2j(extractValue(lsrc,"to="));
           itemstatus = par.get("item_status");
           hsrv::strReplace(itemstatus,"$ALIAS", alias);
           hsrv::strReplace(itemstatus,"$START", from);
           hsrv::strReplace(itemstatus,"$END", to);
           hsrv::strReplace(itemstatus, "$TRUEFALSE", (status == "ON" ? "true" : "false"));
           if(body == "") body += itemstatus;
           else body += ("," + itemstatus);
        }
      }
   }
   return body;
}

string  AutomatismManager::cvtimej2x(string src) {
  return src+":00";
}

string  AutomatismManager::cvtimex2j(string src) {
  if(src.size() <= 5) return src;
  return src.substr(0,5);
}
