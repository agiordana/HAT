//
//  net.cpp
//  phoneagent
//
//  Created by Davide Monfrecola on 11/15/11.
//  Copyright 2011 Università del Piemonte Orientale. All rights reserved.
//

using namespace std;

#include "agentlib.h"

// ----------- Class Net ------------------------------

int Net::checkip(string& adr) {
	unsigned i, n = 0;
    
	for(i = 0; i < adr.size(); i++) {
		if((adr[i] < '0' || adr[i] > '9') && adr[i] != '.') 
            return 0;
		if(adr[i] == '.') 
            n++;
	}
    
	if(n != 3) 
        return 0;
	
    return 1;
}

string Net::getip(string& s) {
	string addr = "";
	unsigned i = 0;
    
	while(s[i] != ':' && i < s.size()) 
        addr += s[i++];
	
    return addr;
}

string Net::getIp(string& adr) {
	struct hostent *host;
	ostringstream outs;
	
    if(checkip(adr)) 
        return adr;
	
    host = gethostbyname(adr.c_str());
	
    if(host == NULL) 
        return "";
	
    outs << (host->h_addr_list[0][0]&255) << "." << (host->h_addr_list[0][1]&255) << "." << (host->h_addr_list[0][2]&255) << "." << (host->h_addr_list[0][3]&255);
	
    return  string(outs.str());
}

unsigned int Net::getipaddress(string& ip) {
	unsigned ipval = 0;
	unsigned b[4], i;
    
	string tip = getIp(ip);
	sscanf(tip.c_str(),"%u.%u.%u.%u", &b[0], &b[1], &b[2], &b[3]);
	
    for(i=0; i<4; i++) 
        ipval = (ipval*256) + b[i];
	
    return ipval;
}

unsigned short Net::getportaddr(string& port) {
	unsigned p;
    
	sscanf(port.c_str(),"%us", &p);
	
    return (unsigned short)p;
}

string Net::getsources(string& s) {
	string src = "";
	unsigned i = 0;
    
	while(s[i] != '_'&&i<s.size()) 
        i++;
	
    if(i==s.size()) {
		src = '_';
		return src;
	}
    
	src += s[i++];
	while(i<s.size()) 
        src += s[i++];
	src+='_';
	
    return src;
}

string Net::getport(string& s) {
	string addr="";
	unsigned i=0;
    
	while(s[i] != ':' && i < s.size()) 
        i++;
	i++;
	
    while(i<s.size() && s[i] != '_') 
        addr+=s[i++];
	
    return addr;
}
