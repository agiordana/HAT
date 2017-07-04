using namespace std;
 
#include "PwrManager.h"


bool PwrManager::setDeviceStatus(string name, string status, string priority) {
   string res;
   string message = par.get("set_device_cmd");
   hsrv::strReplace(message,"$NAME",name);
   hsrv::strReplace(message,"$PRIORITY",priority);
   hsrv::strReplace(message,"$STATUS", (status=="true"?"ON":"OFF"));
   res = sendCmd(message);
   if(res.find("done")!=string::npos) return true;
   else return false;
}

string PwrManager::getBody(string message) {
   string body = message.substr(message.find("\r\n\r\n")+4);
   return body;
}

string PwrManager::mkAnswer(vector<DeviceDescriptor>& devices) {
   string answer;
   stringstream ss;
   for(size_t i=0; i<devices.size(); i++) {
	if(i>0) ss<<",";
	ss<<"{\"name\":\""<<devices[i].name<<"\",\"enable_status\":\"";
	ss<<(devices[i].status=="ON"?"true":"false");
	ss<<"\"}";
   }
   answer = par.get("get_answer_template");
   hsrv::strReplace(answer,"$BODY",ss.str());
   return answer;
}

string PwrManager::sendCmd(string file) {
   string request = "POST / HTTP/1.1\r\nContent-Length: "+hsrv::unsigned2a(file.size())+"\r\n\r\n"+file+"\r\n\r\n";
   string answer = NameTable::forward(request, par.get("pwrcontrol"));
   return answer;
}

bool PwrManager::sendFile(string url, string file) {
   string request = "POST "+url+" HTTP/1.1\r\nContent-Length: "+hsrv::unsigned2a(file.size())+"\r\n\r\n"+file+"\r\n\r\n";
   string answer = NameTable::forward(request, par.get("pwrcontrol"));
   if(answer.find("Not Found") == string::npos &&
        (answer.find("list") != string::npos || answer.find("done") != string::npos || answer.find("OK") != string::npos)) return true;
   return false;
}

string PwrManager::fileDownload(string url) {
   string request = "GET " + url + " HTTP/1.1\r\n\r\n\r\n";
   string answer = NameTable::forward(request, par.get("pwrcontrol"));
   return getBody(answer);
}

string PwrManager::getCategory(string name) {
   if(name.size() >3 && name[2]=='_') return name.substr(0,2);
   else return "";
}

void PwrManager::print(boost::property_tree::ptree const& pt) {
    using boost::property_tree::ptree;
    ptree::const_iterator end = pt.end();
    for (ptree::const_iterator it = pt.begin(); it != end; ++it) {
        std::cout << it->first << ": " << it->second.get_value<std::string>() << std::endl;
        print(it->second);
    }
}

void PwrManager::getDevices(boost::property_tree::ptree const& pt, vector<DeviceDescriptor>&devices, string category) {
    using boost::property_tree::ptree;
    ptree::const_iterator end = pt.end();
    for (ptree::const_iterator it = pt.begin(); it != end; ++it) {
        if(it->first == "type" && it->second.get_value<std::string>()=="load") 
	   mkDevDescription(pt, devices, category);
        else getDevices(it->second,devices,category);
    }
}

void PwrManager::mkDevDescription(boost::property_tree::ptree const& pt, vector<DeviceDescriptor>&devices, string category) {
    string name;
    string priority;
    string status;
    using boost::property_tree::ptree;
    ptree::const_iterator end = pt.end();
    for (ptree::const_iterator it = pt.begin(); it != end; ++it) {
      if(it->first=="device") name = it->second.get_value<std::string>();
      if(it->first=="priority") priority = it->second.get_value<std::string>();
      if(it->first=="status") status = it->second.get_value<std::string>();
    }
    if(getCategory(name)==category) devices.push_back(DeviceDescriptor(name, priority, status));
}
