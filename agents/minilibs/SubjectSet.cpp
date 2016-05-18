/* 
 * File:   SubjectSet.cpp
 * Author: trampfox
 * 
 * Created on November 13, 2012, 9:58 AM
 */

#include "agentlib.h"

using namespace std;

std::map<std::pair<std::string,std::string>, Subject> SubjectSet::subjects;
boost::mutex SubjectSet::mutexclusion;

void SubjectSet::add_subject(string type, string subtype) {
    boost::unique_lock<boost::mutex> lock(mutexclusion);
    if(isDummy(subtype)) {
        string dummy = "*";
        Subject alarm(type, dummy);
        pair<string, string> key(type, dummy);
        SubjectSet::subjects.insert(pair<pair<string, string>, Subject>(key, alarm));
    }
    else {
        Subject alarm(type, subtype);
        pair<string, string> key(type, subtype);
        SubjectSet::subjects.insert(pair<pair<string, string>, Subject>(key, alarm));
    }
}

bool SubjectSet::check_subject(string& type, string& subtype) {
    boost::unique_lock<boost::mutex> lock(mutexclusion);
    pair<string, string> key(type, subtype);
    if(SubjectSet::subjects.count(key)>0) return true;
    else return false;
}

bool SubjectSet::add_observer(string type, string subtype, Observer* observer) {
    boost::unique_lock<boost::mutex> lock(mutexclusion);
    map<pair<string, string>, Subject>::iterator it;
    string dummy = "*";
    bool flag = false;
    
    it = SubjectSet::subjects.find(pair<string, string>(type, subtype));
    if(it == SubjectSet::subjects.end()) it = SubjectSet::subjects.find(pair<string, string>(type, dummy));
    
    if(it != SubjectSet::subjects.end()) {
        it->second.register_observer(observer);
        return true;
    }
    if(isDummy(subtype)) {
        for(it=SubjectSet::subjects.begin(); it != SubjectSet::subjects.end(); it++)
            if(it->first.first == type) {
                it->second.register_observer(observer);
                flag = true;
            }
    }
   
    if(_DEBUG_&&!flag) hsrv::logger->debug("[add_observer] Subject "+type+" - "+subtype+" not found");
    return flag;
}

bool SubjectSet::remove_observer(Observer* observer) {
    boost::unique_lock<boost::mutex> lock(mutexclusion);
    map<pair<string, string>, Subject>::iterator it;
    for (it= subjects.begin(); it!=subjects.end(); it++) {
        it->second.remove_observer(observer);
    }
    return true;
}

bool SubjectSet::remove_observer(string id) {
    boost::unique_lock<boost::mutex> lock(mutexclusion);
    map<pair<string, string>, Subject>::iterator it;
    for (it= subjects.begin(); it!=subjects.end(); it++) {
        it->second.remove_observer(id);
    }
    return true;
}

bool SubjectSet::notify(std::string type, std::string subtype, MMessage& message) {
    boost::unique_lock<boost::mutex> lock(mutexclusion);
    map<pair<string, string>, Subject>::iterator it;
    string dummy = "*";
    bool flag = false;
    
    it = SubjectSet::subjects.find(pair<string, string>(type, subtype));

    /* ritorna false se lo stato dell'agente è OFF */
    if(AgentConf::status == OFF) return false;

    if (it==SubjectSet::subjects.end() && !isDummy(subtype)) {
       it = SubjectSet::subjects.find(pair<string, string>(type, dummy));
    }
    
    if(it != SubjectSet::subjects.end()) {
        it->second.notify_observers(message);
        return true;
        }
    if(isDummy(subtype)) {
        for(it=SubjectSet::subjects.begin(); it != SubjectSet::subjects.end(); it++)
            if(it->first.first == type) {
                    it->second.notify_observers(message);
                    flag = true;
            }
    }
    
    if(_DEBUG_ && !flag) hsrv::logger->debug("[notify_observer] Observer "+type+" - "+subtype+" not found");
    return flag;

}

bool SubjectSet::notify(MMessage& message) {
    boost::unique_lock<boost::mutex> lock(mutexclusion);
    map<pair<string, string>, Subject>::iterator it;
    string type = message.mtype;
    string subtype = message.msubtype;
    
    string dummy = "*";
    bool flag = false;
    
    it = SubjectSet::subjects.find(pair<string, string>(type, subtype));

    /* ritorna false se lo stato dell'agente è OFF */
    if(AgentConf::status == OFF) return false;

    if (it==SubjectSet::subjects.end() && !isDummy(subtype)) {
        it = SubjectSet::subjects.find(pair<string, string>(type, dummy));
    }

    if(it != SubjectSet::subjects.end()) {
        it->second.notify_observers(message);
        return true;
    }
    if(isDummy(subtype)) {
        for(it=SubjectSet::subjects.begin(); it != SubjectSet::subjects.end(); it++)
            if(it->first.first == type) {
                    it->second.notify_observers(message);
                    flag = true;
            }
    }
    
    if(_DEBUG_ && !flag) hsrv::logger->debug("[notify_observer] Observer "+type+" - "+subtype+" not found");
    return flag;
}

void SubjectSet::printSet() {
    boost::unique_lock<boost::mutex> lock(mutexclusion);
    map<pair<string, string>, Subject>::iterator it;
    
    cout << "===== SubjectSet map =====" << endl;
    cout << "| type - subtype | Subject address" << endl;        
    for(it = SubjectSet::subjects.begin();it != SubjectSet::subjects.end();it++) {
        cout << "| " << (it->first).first << " - " << (it->first).second << " | " << endl;
    }
}

bool SubjectSet::isDummy(string& subj) {
    size_t found = subj.find("*");
    return (found != string::npos);
}

