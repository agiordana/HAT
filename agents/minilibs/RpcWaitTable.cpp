#include "RpcWaitTable.h"
#define RPC_TIMEOUT 20

#define MAXTAG 1000000

using namespace std;

RpcEntry::RpcEntry(string tg) {
   tag = tg;
}

RpcWaitTable::RpcWaitTable() {
    tagSequence = 0;
    tag = "RPC_" +hsrv::int2a(tagSequence);
}

string RpcWaitTable::getTag() {
    string answer = tag;
    tagSequence++;
    if(tagSequence > MAXTAG) tagSequence = 0;
    tag = "RPC_" +hsrv::int2a(tagSequence);
    return answer;
}

string RpcWaitTable::callRegister(MMessage& m) {
    boost::unique_lock<boost::mutex> lock(mutexLock);
    pending.push_back(new RpcEntry(getTag()));
    pending.back()->rtime = hsrv::gettime();
    m.add("tag", pending.back()->tag);
    return pending.back()->tag;
}

bool RpcWaitTable::signal(MMessage& m) {
    boost::unique_lock<boost::mutex> lock(RpcWaitTable::mutexLock);
    string mtag = m.getString("answer_tag");
    for(size_t i=0; i<pending.size(); i++) 
       if(pending[i]->tag == mtag) {
	  pending[i]->waiting_queue.send(m);
	  return true;
       }
    return false;
}

bool RpcWaitTable::checkTimeOut() {
    MMessage res;
    double ctime = hsrv::gettime();
    boost::unique_lock<boost::mutex> lock(mutexLock);
    for(size_t i=0; i<pending.size(); i++) {
        if(pending[i]->rtime + RPC_TIMEOUT < ctime) {
	   res = mkComplain(pending[i]->tag);
	   pending[i]->waiting_queue.send(res);
	   hsrv::logger->info("RPC: timed out!!!!");
	}
    }
    return true;
}

MMessage RpcWaitTable::wait(string tag) {
   size_t pos;
   RpcEntry* entry = getEntry(tag);
   if(entry == NULL) return MMessage("error", "no_entry");
   MMessage answer = entry->waiting_queue.receive();
   clearEntry(entry);
   return answer;
}

MMessage RpcWaitTable::mkComplain(string tag) {
   MMessage answer("rpc", "timeout");
   answer.add("answer_tag", tag);
   answer.add("value", "timed_out");
   return answer;
}

RpcEntry* RpcWaitTable::getEntry(string tag) {
   vector<RpcEntry*>::iterator it;
   boost::unique_lock<boost::mutex> lock(mutexLock);
   for(it=pending.begin(); it!=pending.end()&&(*it)->tag != tag; it++);
   if(it == pending.end()) return NULL;
   return *it;
}

bool RpcWaitTable::clearEntry(RpcEntry* entry) {
   vector<RpcEntry*>::iterator it;
   boost::unique_lock<boost::mutex> lock(mutexLock);
   for(it = pending.begin(); it!= pending.end()&&*it!=entry; it++);
   if(*it != entry) return false;
   if(*it == entry) pending.erase(it);
   return true;
}
     
    
