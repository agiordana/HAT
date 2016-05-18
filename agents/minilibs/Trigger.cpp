/* 
 * File:   Trigger.cpp
 * Author: attilio
 * 
 * Created on November 21, 2012, 11:19 PM
 */

using namespace std;

#include "agentlib.h"

Trigger::Trigger(string file): MParams(file) {
    load();
    initSubject();
}

bool Trigger::initSubject() {
    setof<MAttribute> tr;
    tr = extractCategory("trigger");
    
    for(unsigned i=0; i<tr.size(); i++) {
        string v = tr[i].value;
        tm[tr[i].name] = hsrv::a2double(v);
        // name = tipo | value = sottotipo
        SubjectSet::add_subject("trigger", tr[i].name);
    }
    
    return true;
}

void Trigger::exec(Trigger* obj) {
    //cout << "[Trigger] exec thread started" << endl;
    long MCD = (long)(tm.begin()->second * TPRECISION);
    long m;
    int res;
    MMessage event("trigger");
    
    map<string, double>::iterator it;
    for(it = tm.begin(); it != tm.end(); it++) { 
        m = (long)(it->second * TPRECISION);
        MCD = obj->Euclide(MCD, m);
    }
    
    cycle = MCD / TPRECISION;
    rqtp.tv_sec = (int) cycle;
    rqtp.tv_nsec = (int)(NANOPERSECOND*(cycle - rqtp.tv_sec));
    while( true ) {
        for(it = tm.begin(); it != tm.end(); it++) {
            it->second-= cycle;
            if(it->second <=0) {
	      if(it->first == "check_rpc_timeout" && hsrv::rpctab != NULL) hsrv::rpctab->checkTimeOut();
	      else {
                string v = get(it->first,"trigger");
                it->second = hsrv::a2double(v);
                event.msubtype = it->first;
                event.add("time", hsrv::double2a(hsrv::gettime()));
                event.add("period", v);
                
                if(_DEBUG_) hsrv::logger->info("[Trigger] Sending message: " + event.xmlEncode(1), __FILE__, __FUNCTION__, __LINE__);
                
                SubjectSet::notify(event);
	      }
            }
        }
        res = nanosleep(&rqtp, &rmtp);
        if(res < 0) hsrv::logger->error("nanosleep error");
    }
    return;
}

long Trigger::Euclide(long a, long b)  { // prototipo della funzione Euclide //
    long r;
    r = a % b;             // operazione modulo //
    while(r != 0) {         // ciclo di iterazione //
        a = b;
        b = r;
        r = a % b;
    }
    return b;
}
