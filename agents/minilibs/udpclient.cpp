#include "agentlib.h"
//#include "cipher.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define MAXLENGTH 65000
#define EXTRACHARS 16
#define BGAP 12

using namespace std;


/*************************************************************
*********************** METODI PRIVATI ***********************
**************************************************************/
		
//Imposta i parametri di connessione in base ai valori presenti negli attributi ip e nel parametro port.
//ritorna TRUE se va a buon fine, FALSE altrimenti
bool UDPClient::setaddr(int port) {
	// se non è stata impostata la porta o l'indirizzo ip, torna errore
	if (port<=0 || ip=="") {
		err=-1;
		return false;
	}
	memset(&sendaddr,0,sizeof(sendaddr));
	sendaddr.sin_family=AF_INET;
    	sendaddr.sin_port=htons(port);
    	inet_aton(ip.c_str(),&sendaddr.sin_addr);
    	err=0;	
	return true;
}



/*************************************************************
********************** METODI PUBBLICI ***********************
**************************************************************/

bool UDPClient::start(string crypt) {
	//se è stata già avviata una connessione, torna errore
    if(crypt=="ON") {
        type = ENCRYPT;
    }
	if (sockfd>0) {
		err=-1;
		return false;
	}
	
	//apri connessione
    if ((sockfd=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))<0) {
        //se non è stato possibile aprire il socket, torna errore
        err=errno;
        return false;
    }
    
    err=0;
    return true;
	
}


//Invia il messaggio contenuto in msg alla porta port, cifrandolo se è stata impostata una chiave di cifratura
//ritorna il numero di caratteri inviati o -1 in caso di errore
int UDPClient::sendMsg(string m) {
	string msg=m;
	//dimensione in caratteri del messaggio
	size_t size=msg.size(); //dimensione in caratteri del messaggio
	size_t chars=0; //caratteri inviati
    string dgst;
    string ascitime;
	
	//imposto dati indirizzo
	if (!setaddr(hsrv::a2int(mport))) return -1;
	
	//cifra il messaggio se si richiede criptatura, invia
	if (type==ENCRYPT) {
        ascitime = '"'+ hsrv::double2a(hsrv::gettime())+'"';
        msg = msg+"<>"+ ascitime;
        size = msg.size();
        dgst = Cipher::digest(msg,Cipher::getKeySize(PUBLIC));
        size_t n = Cipher::getKeySize(PUBLIC)+msg.length()+EXTRACHARS;
		char cifrato[n];
        for (unsigned i=0; i<n; i++) {
            cifrato[i] = 0;
        }
        sprintf(cifrato, "%03d,%03d,%3d,", BGAP, (int)Cipher::getKeySize(PUBLIC)+BGAP,(int)size+BGAP+Cipher::getKeySize(PUBLIC));
 //       ll = strlen(cifrato);
		if (!Cipher::public_encrypt(dgst,&cifrato[BGAP])) {
			err=Cipher::getErrno();
			return -1;
		}
        size = Cipher::getKeySize(PUBLIC)+BGAP;
        strcpy(&cifrato[size],msg.c_str());
        size = n;
        chars=sendto(sockfd, cifrato, size, 0, (struct sockaddr*) &sendaddr, sizeof(sendaddr));
	}
	
	//altrimenti,invio messaggio semplice
	else chars=sendto(sockfd, msg.c_str(), msg.size(), 0, (struct sockaddr*) &sendaddr, sizeof(sendaddr));
	
	//se non sono stati inviati caratteri, torna errore
	if (chars==0 || chars!=size) {		
    		err=(chars>0) ? -1 : errno;
    		return -1;
    	}
    	
    	//altrimenti, torna caratteri spediti
    	err=0;
	return (int)chars;
}


//chiude il socket descriptor, ritorna sempre TRUE
bool UDPClient::stop() {
	if (sockfd>0) close(sockfd);
	err=0;
	return true;
}


//ritorna una stringa contenente un messaggio di errore, in base al codice di errore presente in err
string UDPClient::getErr() {
	string str("");
	if (err>0) str = strerror(err);
	else if (err==-1) str="something goes wrong on class UDPClient";
	else if (err==WRONG_KEY) str="wrong key used on Cipher";
	err=0;
	return str;
}

//restituisce il codice di errore
int UDPClient::getErrno() {
	return err;
}		
	
