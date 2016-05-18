#include "RpcCall.h"

using namespace std;

bool RpcCall::init(string& n) {
    MParams::load();
    name = n;
    targetip = get("address");
    targetport = get("port");
    return true;
}

RpcCall::RpcCall(string p, string ip) {
    IPlocation();
    name = "*";
    targetip = ip;
    targetport = p;
}

string RpcCall::sendStringMessage(string& message) {
    char *buffer;
    char buffin[MAXB];
    string answer = "";
    unsigned long n, buflen;
    unsigned long bl=0;
    int r;
    int timeout=0;
    string info;

    if(true) {
        info = "Message sent: "+message;
        hsrv::logger->info(info, __FILE__, __FUNCTION__, __LINE__);
    }
    buffer = (char*)malloc(message.size()+2);
  
    sprintf(buffer,"%s",message.c_str());

    buflen = strlen(buffer);
    r = connectSetup();

    if(r > 0 && sd >= 0) {
        n = write(sd, buffer, buflen);
        while((timeout = dataAvailable(sd)) > 0 && (n = read(sd, &buffin, MAXB-1)) > 0)
        if(n > 0) {
            buffin[n] = 0;
            answer += string(buffin);
        }
    }
    connectClose();
    free(buffer);
    if(timeout <= 0) {
        hsrv::logger->alert(name+": tcp connection timed out!!!!");
    }
    return answer;
}
