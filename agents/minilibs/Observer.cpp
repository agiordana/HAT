/* 
 * File:   Observer.cpp
 * Author: trampfox
 * 
 * Created on November 8, 2012, 3:29 PM
 */

#include "agentlib.h"

using namespace std;

Observer::Observer() {
}

Observer::Observer(const Observer& orig) {
}


bool Observer::match_target(MMessage& m){
    vector<string> tg = m.getList("target");
    if (tg.size()==0) {
        return true;
    }
    for(unsigned i=0; i<tg.size(); i++)
        if(target.member(tg[i])) return true;
    return false;
}

