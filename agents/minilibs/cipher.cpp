using namespace std;

#include "agentlib.h"
#include <string.h>
#include <openssl/rsa.h>
#include <openssl/sha.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#define DGLEN 5

/**************************************************************
************************* ATTRIBUTI ***************************
***************************************************************/

RSA *Cipher::key=NULL;
RSA *Cipher::privatekey=NULL;
RSA *Cipher::publickey=NULL;
int Cipher::err;



/*************************************************************
******************** METODI PRIVATI GENERICI *****************
**************************************************************/

//aggiunge casualità alla cifratura
void Cipher::SeedRNG() {
	err=0;
	if (!RAND_load_file("/dev/urandom", 1024)) {
		char buf[1024];
		RAND_seed(buf, sizeof(buf));
		while (!RAND_status()) {
			int r = rand();
			RAND_seed(&r, sizeof(int));
		}
	}
}


/*************************************************************
************ METODI PRIVATI: RECUPERO DELLA CHIAVE ***********
**************************************************************/

//restituisce la chiave privata prelevandola dal file pem, NULL in caso di errore
RSA * Cipher::getPrivateKey(string pem) {
    FILE* fd = fopen(pem.c_str(), "r");
    if (fd == NULL) return NULL;
    RSA *privKey = PEM_read_RSAPrivateKey(fd, NULL, NULL, NULL);
    fclose(fd);
    return privKey;
}

//restituisce la chiave pubblica prelevandola dal file pem, NULL in caso di errore
RSA * Cipher::getPublicKey(string pem) {
    FILE* fd = fopen(pem.c_str(), "r");
    if (fd == NULL) return NULL;
    RSA *pubKey = PEM_read_RSA_PUBKEY(fd, NULL, NULL, NULL);
    fclose(fd);
    return pubKey;
}

//restituisce la chiave prelevandola dal file pem
//definisce il tipo di chiave inserita (pubblica o privata)
//restituisce NULL in caso di errore
RSA * Cipher::getKey(string pem) {
	RSA *key;
	if (key=getPublicKey(pem)) { return key; }
	else if (key=getPrivateKey(pem)) { return key; }
	else return NULL;
}



/*************************************************************
************* METODI PRIVATI: CIFRATURA/DECIFRATURA **********
**************************************************************/

//decifro il messaggio buff con chiave privata, inserisco il messaggio decifrato in res
bool Cipher::private_decrypt(char *buff, string &res) {
	//setto il valore iniziale di decifrato unsigned	
	unsigned char decifrato[RSA_size(privatekey)];
	memset(decifrato, 0, sizeof(decifrato));					
	//decifro
	if (RSA_private_decrypt(RSA_size(privatekey), reinterpret_cast<unsigned char*>(buff), decifrato, privatekey, RSA_PKCS1_OAEP_PADDING)<0) {
		err=ERR_get_error();
		return false;
	}	
	//preparo risultato
	res.assign(reinterpret_cast<char*>(decifrato));
	return true;	
}

//cifro il messaggio buff con chiave pubblica, inserisco il messaggio cifrato in res
bool Cipher::public_encrypt(string buff, char *res) {
	SeedRNG();
	unsigned char cifrato[RSA_size(publickey)];
	if (RSA_public_encrypt(buff.size(), reinterpret_cast<const unsigned char*>(buff.c_str()), reinterpret_cast<unsigned char*>(res), publickey, RSA_PKCS1_OAEP_PADDING)<0) {
		err=ERR_get_error();
		return false;
	}
	return true;
}



/*************************************************************
*********************** METODI PUBBLICI **********************
**************************************************************/

//prelevo la chiave, stabilisco se è pubblica o privata
//restituisco TRUE se la chiave è valida, FALSE in caso contrario
bool Cipher::init(std::string prkey, std::string pubkey) { 
	err=0;
    if(privatekey!=NULL) delete privatekey;
	privatekey=getKey(prkey);
	if (privatekey==NULL) { err=WRONG_KEY; return false; }
    if(publickey!=NULL) delete publickey;
    publickey=getKey(pubkey);
	if (publickey==NULL) { err=WRONG_KEY; return false; }

	return true;
}


//restituisce la dimensione della chiave
int Cipher::getKeySize(int ty) {
	err=0;
	if(ty==PUBLIC) return RSA_size(publickey);
    else return RSA_size(privatekey);
}

//ritorna una stringa di errore in caso vi sia un errore, stringa vuota in caso contrario
string Cipher::getErr() {
	string str("");
	if (err>0) {
		ERR_load_crypto_strings(); 
		str = ERR_error_string(err, NULL);
		ERR_free_strings(); 
	}
	else if (err==WRONG_KEY) str = "Wrong key used"; 
	else if (err==0 && errno>0) str = strerror(errno);
	return str;
}

//ritorna il codice di errore
int Cipher::getErrno() {
	return err;
}

/*

string Cipher::digest(string &d, size_t n) {
    unsigned i;
    string r="";
    bool skip = true;
    for(i=0; i<d.size(); i++) {
        if(d[i] == '"') {
            if( !skip) skip = true;
            else skip = false;
        }
        if(!skip&&d[i]!='<'&&d[i]!='>'&&d[i]!='?'&&d[i]!='/'&&d[i]!='"') r.push_back(d[i]);
    }
    if(r.size()>n) return r.substr(0,n);
    else return r;
}
*/

string Cipher::convert(char* hash) {
    ostringstream out;
    out << "SHA1: " << std::hex;
    for(int i = 0; i < DGLEN*4; ++i) {
        out << ((hash[i] & 0x000000F0) >> 4)
        <<  (hash[i] & 0x0000000F);
    }
    out << std::endl;
    return out.str();
}


string Cipher::digest(string &a, size_t n) {
    char hash[DGLEN*4];
    unsigned int digest[DGLEN];
    boost::uuids::detail::sha1 sha;
    sha.process_bytes(a.c_str(), a.size());
    sha.get_digest(digest);
    for(int i = 0; i < DGLEN; ++i) {
        const char* tmp = reinterpret_cast<char*>(digest);
        hash[i*4] = tmp[i*4+3];
        hash[i*4+1] = tmp[i*4+2];
        hash[i*4+2] = tmp[i*4+1];
        hash[i*4+3] = tmp[i*4];
    }
    return convert(hash);
}
