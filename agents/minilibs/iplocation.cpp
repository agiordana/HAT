//
//  iplocation.cpp
//  phoneagent
//
//  Created by Davide Monfrecola on 11/15/11.
//  Copyright 2011 Università del Piemonte Orientale. All rights reserved.
//

using namespace std;

#include "agentlib.h"

//------------------------- Class IPlocation ------------------------------
int IPlocation::setaddr() {
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(getportaddr(targetport));
	addr.sin_addr.s_addr = htonl(getipaddress(targetip));
	
    return 1;
}

int IPlocation::issource(string& s) {
	string tofind='_'+s+'_';
	
    if(sources == "_") 
        return 1;
	
    if(sources.find(tofind) != string::npos) 
        return 1;
    else 
        return 0;
}

int IPlocation::connectSetup() {
	if(sd >=0) 
        connectClose();
	
    setaddr();
	
    if((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        myerr--; 
        return myerr;
    }
	
    if (connect(sd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        myerr--; 
        return myerr;}
	
    myerr = 0;
	
    return sd;
}

int IPlocation::connectON() {
	
    if(myerr >= 0 && sd > 0) 
        return 1;
	else 
        return 0;
}

int IPlocation::connectClose() {
	if(sd >= 0) 
        close(sd);
	sd = -1;
	myerr = 0;
	return 1;
}

int IPlocation::dataAvailable(int fd) {
	int rc;
	fd_set fds;
	struct timeval tv;
	
	tv.tv_sec = TOUT;
	tv.tv_usec = 0;
	
	FD_ZERO(&fds);
	FD_SET(fd,&fds);
	rc = select(fd+1, &fds, NULL, NULL, &tv);
	
    if (rc < 0) 
        return -1;
    
    return FD_ISSET(fd,&fds) ? 1 : 0;
}

int IPlocation::sendCmd(unsigned char buffer[]) {
	int res;
	
    if(sd < 0 || myerr < 0) 
        return -1;
	
    res = write(sd, buffer, 6);
	
    if(res < 0) {
		myerr--;
		return myerr;
	}
    
	res = read(sd, buffer, 6);
	
    if(res < 0) {
		myerr--;
		return myerr;
	}
    
	myerr = 0;
	return 1;
}

int IPlocation::sendCmd(char buffer[], unsigned bsize) {
	int res;
	int bl;
	buffer[bsize-1] = 0;
	bl = strlen(buffer);
	
    if(sd < 0 || myerr < 0) 
        return -1;
	
    res = write(sd, buffer, bl);
	
    if(res < 0) {
		myerr--;
		return myerr;
	}
    
	bl = 0;
	
    while((res = read(sd, &buffer[bl], bsize)) > 0) 
        bl += res;
	
    buffer[bl] = 0;
	
    if(res < 0) {
		myerr--;
		return myerr;
	}
    
	myerr = 0;
	
    return 1;
}

int IPlocation::sendCmd(string& msg) {
    char buffer[MAXB];
    char buffin[MAXB+2];
    int bl, count;
    int i;
    int res;
    
    if((msg.size()+1) > MAXB) return 0;
    sprintf(buffer, "%s", msg.c_str());
    buffer[strlen(msg.c_str())] = '\0';
    bl = strlen(buffer);
    res = write(sd, buffer, bl);
    msg = "";
    if((count = read(sd, buffin, MAXB)) <=0) return 0;
    buffin[count] = '\0';
    for (i=0; i<count&&buffin[i]!='{'; i++);
    msg = string(&buffin[i]);
    while((count = read(sd, buffin, MAXB))>0){
        buffin[count] = '\0';
        msg += string(buffin);
    }
    return 1;
}

int IPlocation::rvportSetup() {
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(getportaddr(targetport));
    
	if(sd > 0) 
        close(sd);
	//setaddr();
	if((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        myerr = -1; 
        return myerr;
    }
    
	if(bind(sd, (struct sockaddr *)&addr, sizeof(addr)) <0) {
        myerr = -2; 
        return myerr;
    }
	
    listen(sd, MAXCONN);
	
    return 1;
}
