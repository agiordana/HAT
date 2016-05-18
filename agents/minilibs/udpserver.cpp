#include "agentlib.h"
//#include "cipher.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXLENGTH 65000
#define MAXDELAY 5

using namespace std;


/*************************************************************
*********************** METODI PRIVATI ***********************
**************************************************************/

//Imposta i parametri di connessione in base ai valori presenti negli attributi mip e port
//ritorna sempre TRUE
bool UDPServer::setaddr() {
    int mport = hsrv::a2int(port);
    memset(&addr,0,sizeof(struct sockaddr_in));
    addr.sin_family=AF_INET;
    addr.sin_port=htons(mport);
    inet_aton(mip.c_str(),&addr.sin_addr);    
    return true;			
}


//apre una nuova socket. Restituisce TRUE in caso di successo, FALSE altrimenti
bool UDPServer::open_socket() {
	err=0;
	if (sockfd>0) {
		err=-1;
                stringstream tmp;
                tmp << "[UDPServer] open_socket() failed (sockefd > 0) | sockfd: " << sockfd;
                hsrv::logger->error(tmp.str(),__FILE__, __FUNCTION__, __LINE__);
		return false;
	}
	if ((sockfd=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))<0) {
		err=errno;
		stringstream tmp;
		tmp << "[UDPServer] Socket() failed with errno: " << this->getErr();
		hsrv::logger->error(tmp.str(),__FILE__, __FUNCTION__, __LINE__);
		return false;
	}
	return true;			
}


//Esegue il bind()
//ritorna TRUE se va a buon fine, FALSE in caso di errore
bool UDPServer::do_bind() {
	err=0;
	setaddr();
	if( bind(sockfd, (struct sockaddr *) &addr, sizeof(addr)) < 0 ) {
    		err=errno;
		stringstream tmp;
                tmp << "[UDPServer] bind() failed with errno (port: " << this->port << "): " << this->getErr();
                hsrv::logger->error(tmp.str(),__FILE__, __FUNCTION__, __LINE__);
    		return false;
    	}
    return true;
}


//Imposta i parametri di multicast
//ritorna TRUE se va a buon fine, FALSE in caso di errore
bool UDPServer::setMulticast() {
	err=0;
	struct ip_mreq mcast;
	memset(&mcast,0,sizeof(mcast));
	mcast.imr_multiaddr.s_addr = inet_addr(mip.c_str());
	mcast.imr_interface.s_addr = htonl(INADDR_ANY);
	if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mcast, sizeof (mcast)) < 0) { //creo gruppo multicast
		err=errno;
		stringstream tmp;
                tmp << "[UDPServer] setsockopt() failed with errno: " << this->getErr();
                hsrv::logger->error(tmp.str(),__FILE__, __FUNCTION__, __LINE__);
		return false;
	}
	return true;
}

bool UDPServer::setPortReuse() {
    int reuse=1;

   #ifndef SO_REUSEPORT
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0)
   #else
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, (char *)&reuse, sizeof(reuse)) < 0)
   #endif
   {
	err=errno;
	stringstream tmp;
	tmp << "[UDPServer] setsockopt() failed with errno: " << this->getErr();
	return false;
    }
    return true;
}


/*************************************************************
********************** METODI PUBBLICI ***********************
**************************************************************/
		
//Inizio della connessione: apre la socket, esegue il bind e setta le impostazioni di multicast 
//ritorna TRUE in caso di successo, FALSE altrimenti
bool UDPServer::start(string crypt) {
    if(crypt=="ON") {
        type = ENCRYPT;
    }
	err=0;
	if (!open_socket()) return false;
	if (!setPortReuse()) return false;
	if (!do_bind()) return false;
	if (!setMulticast()) return false;
	return true;
}


//Memorizza nel parametro msg il messaggio ricevuto
//ritorna il numero di caratteri letti, -1 in caso di errore
int UDPServer::recvMsg(string& msg, int w) {
	memset(&last_reply, 0 , sizeof(last_reply));
	socklen_t replysize=sizeof(struct sockaddr_in);
	size_t chars=0; //caratteri ricevuti
    string ascitime;
    size_t pos;
    int rv;
    int n, m, k;
    double start_time;
    double arrival_time;
    string dgst;
    string tmpmsg="";
	
	char buff[MAXLENGTH];
	
    struct pollfd ufds[1];
    ufds[0].fd = sockfd;
    ufds[0].events = POLLIN;
    
    if((rv = poll(ufds, 1, w*1000)) == -1) {
        err= -1;
        return -1;
    }
    else {
        if(rv == 0) return 0;
    }
    
    //se non sono stati ricevuti caratteri, torna errore
	if ((chars=recvfrom(sockfd, buff, sizeof(buff), 0, (struct sockaddr*) &last_reply, &replysize))==0) {
		err= (chars==0) ? -1 : errno;
		return -1;
	}
	
	if (type==ENCRYPT) {
//		Cipher::init(key);
        sscanf(buff, "%d,%d,%d,",&k, &n, &m);
		string decifrato;
		//se non è possibile decifrarlo, torna errore
		if (!Cipher::private_decrypt(&buff[k], decifrato)) {
			err=Cipher::getErrno();
			return -1;
		}
		//copia messaggio nella stringa di destinazione
		for(unsigned i=n; i<m&&i<chars; i++) {
            if(buff[i]!='\0')tmpmsg+=buff[i];
        }
        dgst = Cipher::digest(tmpmsg, Cipher::getKeySize(PRIVATE));
        if (dgst != decifrato) {
            err = -1;
            return -1;
        }
        pos=tmpmsg.find("<>");
        ascitime = tmpmsg.substr(pos+3,tmpmsg.length()-pos-4);
        start_time = hsrv::a2double(ascitime);
        arrival_time = hsrv::gettime();
        if (arrival_time-start_time > MAXDELAY||start_time-arrival_time > MAXDELAY) {
            err = -1;
            return -1;
        }
        msg=tmpmsg.substr(0,pos-1);
	}
	//altrimenti, copia messaggio nella stringa di destinazione
	else msg.assign(buff);

	//ritorno i caratteri letti
	err=0;
	return (int)chars;
}


//Ritorna i parametri dell'agente che ha inviato il messaggio, presenti nell'attributo last_reply
struct sockaddr_in UDPServer::getReplyAddress() {
	return last_reply;
}


//Chiude la connessione, ritorna sempre TRUE
bool UDPServer::stop() {
	if (sockfd>0) close(sockfd);
	err=0;
	return true;
}


//ritorna una stringa contenente un messaggio di errore, in base al codice di errore presente in err
string UDPServer::getErr() {
	string str("");
	if (err>0) str = strerror(err);
	else if (err==-1) str="something goes wrong on class UDPServer";
	else if (err==WRONG_KEY) str="wrong key used on cipher";
	err=0;
	return str;
}


//restituisce il codice di errore
int UDPServer::getErrno() {
	return err;
}		
	
