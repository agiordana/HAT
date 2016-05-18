#ifndef agentnet_Crypto_h
#define agentnet_Crypto_h


#include <openssl/rsa.h>
#include <openssl/sha.h>
#include <boost/uuid/sha1.hpp>

/**
@def WRONG_KEY
@brief codice di errore nel caso in cui la chiave scelta non sia valida
*/
#define WRONG_KEY -2

/**
@brief Cipher
metodi per la cifratura/decifratura di stringhe
@author Valeria Mussa
@date Maggio 2012
*/
class Cipher {

	private:
	
		/*************************************************************
		******** parametri per il funzionamento della classe *********
		**************************************************************/
	
		/**
		@enum cryptType
		@brief definizione del tipo di chiave (PUBLIC, PRIVATE)
		*/
		
		/**
		@var RSA*
		@brief chiave usata
		*/
		static RSA *key;
        static RSA *publickey;
        static RSA *privatekey;
		
		/**
		@var int
		@brief codice di errore
		*/
		static int err;
		
		
		
		/*************************************************************
		******************** metodi privati generici *****************
		**************************************************************/
		
		/**
		Aggiunge casualità alla cifratura
		*/
		static void SeedRNG();
		
		
		/*************************************************************
		********* metodi privati per il recupero della chiave ********
		**************************************************************/
		
		/**
		Recupera la chiave pubblica da un file .pem
		@param p path del file .pem contenente la chiave
		@return chiave pubblica
		*/		
		static RSA * getPublicKey (std::string p);
		
		/**
		Recupera la chiave privata da un file .pem
		@param p path del file .pem contenente la chiave
		@return chiave privata
		*/
		static RSA * getPrivateKey (std::string p);
		
		/**
		Recupera la chiave da un file .pem e imposta il tipo di chiave (PUBLIC, PRIVATE)
		@param p path del file .pem contenente la chiave
		@return chiave
		@see getPublicKey(), getPrivateKey()
		*/
		static RSA * getKey(std::string p);
		
        static std::string convert(char* h);
    
	public:
	
		/**
		Definizione delle impostazioni di cifratura: preleva la chiave, stabilisce se è pubblica o privata
		@param file path del file .pem contenente la chiave
		@return TRUE se la chiave è valida, FALSE altrimenti
		*/
        static bool init(std::string privkey, std::string pubkey);
		
        /**
         Cifra una stringa
         @param m stringa da cifrare
         @param c array di caratteri in cui finirà il messaggio cifrato
         @return TRUE se la cifratura va a buon fine, FALSE altrimenti
         */
        static bool public_encrypt(std::string m, char* c);
    
        /**
         Decifra un array di caratteri
         @param c array di caratteri da decifrare
         @param d stringa in cui finirà il messaggio decifrato
         @return TRUE se la decifratura va a buon fine, FALSE altrimenti
         */
        static bool private_decrypt(char* c, std::string& d);

        /**
         estrae un digest dalla string passata come argomento
         @param d la stringa sorgente
         @param n massimo numero di caratteri nel digest
         @return il digest calcolato
         */
        static std::string digest(std::string& d, size_t n=256);
		
		/**
		Ritorna la dimensione della chiave utilizzata
		@return dimensione della chiave
		*/
		static int getKeySize(int ty);
		
		/**
		Ritorna un messaggio di errore
		@return stringa contenente il messaggio di errore
		*/
		static std::string getErr();
		
		/**
		Ritorna il codice di errore
		@return codice di errore (errno per errore di sistema, -1 errore generico, WRONG_KEY errore sulla chiave
		*/
		static int getErrno();
		
		/**
		Pulizia della struttura dati
		*/
		~Cipher() { RSA_free(key); }; //distruttore (chiude *key)


};

#endif


