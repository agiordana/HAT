//
//  FileManager.h
//  FileManager
//
//  Created by Davide Monfrecola on 12/7/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifndef mdagent_FileManager_h
#define mdagent_FileManager_h

class FileManager {
public:
    
    /**
     Crea la directory specificata
     @param dirpath path relativo dove creare la directory
     @param perror flag che indica se stampare l'errore o meno richiamando perror e far terminare il 
                   programma richiamando la exit()
     @return 1 se la directory viene creata correttamente
     0 in caso di errore
     @see isDir
     */
    static bool makeDir(std::string&, bool); 
    /**
     Cancella la directory specificata
     @param dirpath path relativo della directory da cancellare 
     @return 1 se la directory viene cancellata correttamente
     0 in caso di errore
     @see deleteFile()
     */
    static bool deleteDir(std::string&);
    /**
     Cancella il file specificato
     @param filepath path relativo dove si trova il file
     @param filename nome del file
     @return 1 se il file è stato eliminato correttamente 
     0 in caso di errore
     @see isFile()
     */
    static bool deleteFile(std::string&, const std::string& = "");
    /**
     Controlla se il file esiste
     @param filepath path relativo del file da controllare
     @return 1 se il file esiste ed è possibile accederci 
     0 altrimenti
     */
    static bool isFile(std::string&);
    /**
     Controlla se il file con l'estensione specificata esiste
     @param filepath path relativo del file da controllare
     @param fileext estensione del file da controllare
     @return 1 se il file con l'estensione specificata esiste
     0 altrimenti
     @see 
     */
    static bool isFile(std::string&, const std::string&);
    /**
     Controlla se la directory esiste
     @param dirpath path relativo della directory da controllare
     @return 1 se il file esiste ed è possibile accederci 
     0 altrimenti
     */
    static bool isDir(std::string&);
    /**
     Conta quanti file sono presenti all'interno della cartella specificata
     @param dirpath path relativo della directory 
     @return numero di file presenti nella directory -1 altrimenti
     */
    static int fileCount(std::string&);
    /**
     Recupera tutti i file presenti nella directory specificata e ritorna una lista
     @param dirpath path relativo della directory 
     @return numero di file presenti nella directory, -1 altrimenti
     */
    static int fileList(std::string&, std::vector<std::string>&);
    /**
     Conta quante directory sono presenti all'interno della cartella specificata
     @param dirpath path relativo della directory 
     @return numero di file presenti nella directory -1 altrimenti
     */
    static int dirCount(std::string&);
    /**
     Recupera tutti i file presenti nella directory specificata e ritorna una lista
     @param dirpath path relativo della directory 
     @return numero di file presenti nella directory, -1 altrimenti
     */
    static int dirList(std::string&, std::vector<std::string>&);
    /**
     Ritorna la dimensione in byte del file passato come parametro
     @param filepath path relativo del file
     @param filename nome del file 
     @return dimensione in byte del file
     @see isFile(std::string&)
     */
    static long filesize(std::string&, const std::string& = "");
    /**
     Copia il file specificato nella directory specificata
     @param filepath path corrente del file da copiare
     @param new_filepath path dove copiare il file
     @param new_filename nome del nuovo file creato. Se lasciato vuoto utilizza il nome del 
     file originale 
     */
    static bool copyFile(std::string&, std::string&, const std::string& = "");
    /**
     Sposta il file specificato nella directory specificata
     @param filepath path relativo corrente del file da copiare
     @param new_filepath path relativo dove copiare il file
     @param new_filename nome del nuovo file creato. Se lasciato vuoto utilizza il nome del 
     file originale
     */
    static bool moveFile(std::string&, std::string&, const std::string& = "");
    /**
     Copia il file specificato nella directory specificata
     @param filepath path relativo corrente del file da copiare
     @param filename nome del file da copiare
     @param new_filepath path relativo dove copiare il file
     @param new_filename nome del nuovo file creato. Se lasciato vuoto utilizza il nome del 
     file originale 
     */
    static bool copyDir(std::string&, std::string&, std::string&, const std::string& = "");
    /**
     Sposta il file specificato nella directory specificata
     @param filepath path relativo corrente del file da copiare
     @param filename nome del file da copiare
     @param new_filepath path relativo dove copiare il file
     @param new_filename nome del nuovo file creato. Se lasciato vuoto utilizza il nome del 
     file originale
     */
    static bool moveDir(std::string&, std::string&, std::string&, const std::string& = "");
    /**
     Copia tutti i file presenti nella directory specificata. È anche possibile specificare 
     un'estensione. In quel caso copia solo i file con l'estensione specificata
     @param dirpath path relativo corrente della directory dalla quale prendere i file
     @param new_dirpath path relativo della directory nella quale copiare i file
     @param fileext estensione dei file da copiare. Se vuota (default) copia tutti i file
     presenti nella directory senza 
     */
    static bool copyAllFile(std::string&, std::string&, const std::string& = "");
    /**
     Sposta tutti i file presenti nella directory specificata. È anche possibile specificare 
     un'estensione. In quel caso sposta solo i file con l'estensione specificata
     @param dirpath path relativo corrente della directory dalla quale spostare i file
     @param new_dirpath path relativo della directory nella quale spostare i file
     @param fileext estensione dei file da spostare. Se vuota (default) sposta tutti i file
     presenti nella directory senza 
     */
    static bool moveAllFile(std::string&, std::string&, const std::string& = "");
	/**
	 Salva una string su file memorizzato su disco
	 @param name nome del file
	 @param content il contenuto del file
	 @return true se l'operazione è andata a buon fine, false altrimenti
	 */
	static bool saveFile(std::string&, std::string&);
	/**
	Legge un file memorizzato su disco
	@param name nome del file
	@return stringa il contenuto del file se l'operazione è andata a buon fine, una vuota 
	altrimenti
	*/
	static std::string readFile(std::string&);
	
    /** 
	 L'argomento di ingresso è un pathname.
	 Ritorna la "tail", cioè il nome del file
	 @ param pathhname
	 @ return il nome local del file
	 */
	static std::string getTail(std::string);
    /** 
	 L'argomento di ingresso è un pathname.
	 Ritorna la "root", cioè ilpathname precedente il nome del file
	 @ param pathhname
	 @ return la radice
	 */
	static std::string getRoot(std::string);
	/** 
	 L'argomento di ingresso è un file name o un pathname.
	 Ritorna la l'estensione del file corrispondente
	 @param pathhname
	 @return l'estensione del file se esiste, altrimenti una stringa vuota
	 */
	static std::string getExt(std::string path);
	/** 
	 L'argomento di ingresso è un file name o un pathname.
	 Ritorna il nome senza l'estensione (stem)
	 @param pathhname
	 @return la radice del file senza l'estensione
	 */
	static std::string getStem(std::string path);
	/**
	 Controlla se il file corrispondente a path esiste ed e in un formato
	 video valido
	 @param path
	 @return true se a path corrisponde un file video valido, false altrimenti
	 */
	static bool isVideoFile(std::string path);
	/**
	 Controlla se il file corrispondente a path esiste ed e in un formato
	 immagine valido
	 @param path
	 @return true se a path corrisponde un file immagine valido, false altrimenti
	 */
	static bool isImageFile(std::string path);
    /**
     Elimina il carattere char dalla stringa
     @param string char
     @return la stringa purgata
     **/
    
    static std::string stripCharacter(std::string, char);
    /**
     @var std::string
     @brief path assoluto della base dir da utilizzare per le operazioni eseguite dal 
            FileManager
     */
    static std::string basedir;
    
};

#endif
