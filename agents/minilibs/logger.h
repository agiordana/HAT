//
//  logger.h
//  phoneagent
//
//  Created by Davide Monfrecola on 11/14/11.
//  Copyright 2011 Università del Piemonte Orientale. All rights reserved.
//

#ifndef phoneagent_logger_h
#define phoneagent_logger_h

class MyException;
class Message;

/** @brief Horus Logger - Componente per i log 
 
 Viene utilizzata un'istanza di questa classe per il log di eventi su file e remoto.
 Ogni agente/componente che necessita di una gestione dei log può utilizzare un'istanza
 di questa classe. Sono disponibili vari livelli di log (debug < info < warning < alert < error < fatal) 
 Al suo interno è presente una coda di messaggi e la gestione di quest'ultima è affidata ad una
 thread che, attraverso il meccanismo della mutua esclusione, garantisce che i messaggi presenti 
 nella coda vengano scritti in modo corretto sul file di log.
 @author Davide Monfrecola
 @date Novembre 2011
 */
class HorusLogger: public MsgQueue<Message> {
public:
	/**
     Costruttore della classe. Crea una nuova istanza del componente di log. Le operazioni
     che compie sono:
     	- inizializzazione dei vari layout di output
     	- impostazione del tipo di output (stdout se in fase di debug, file altrimenti)
     	- scelta del layout di default da utilizzare
     @param logpath path del file di log
     @see initLayouts(), setLayout(), setOutputType()
     */
	HorusLogger(const std::string&) throw();
	/**
     Distruttore della classe
     */
	~HorusLogger() throw() {};
	/**
     Messaggio di log di livello debug
     @param message messaggio di log
     @param file file dal quale è stato richiamato il metodo 
     @param function funzione dalla quale è stato richiamato il metodo
     @param line linea del file dal quale è stato richiamato il metodo
     @param level stringa che indica il livello del messaggio di log che verrà utilizzata per 
     		generare il messaggio da scrivere su file
     @see send()
     */
	virtual void debug(const std::string&, const std::string& = "", const std::string& = "", int = 0, const std::string& = "DEBUG");
	/**
     Messaggio di log di livello info
     @param message messaggio di log
     @param file file dal quale è stato richiamato il metodo 
     @param function funzione dalla quale è stato richiamato il metodo
     @param line linea del file dal quale è stato richiamato il metodo
     @param level stringa che indica il livello del messaggio di log che verrà utilizzata per 
     		generare il messaggio da scrivere su file
     @see send()
     */
	virtual void info(const std::string&, const std::string& = "", const std::string& = "", int = 0, const std::string& = "INFO");
	/**
     Messaggio di log di livello error
     @param message messaggio di log
     @param file file dal quale è stato richiamato il metodo 
     @param function funzione dalla quale è stato richiamato il metodo
     @param line linea del file dal quale è stato richiamato il metodo
     @param level stringa che indica il livello del messaggio di log che verrà utilizzata per 
     		generare il messaggio da scrivere su file
     @see send()
     */
	virtual void error(const std::string&, const std::string& = "", const std::string& = "", int = 0, const std::string& = "ERROR");
	/**
     Messaggio di log di livello alert
     @param message messaggio di log
     @param file file dal quale è stato richiamato il metodo 
     @param function funzione dalla quale è stato richiamato il metodo
     @param line linea del file dal quale è stato richiamato il metodo
     @param level stringa che indica il livello del messaggio di log che verrà utilizzata per 
     		generare il messaggio da scrivere su file
     @see send()
     */
	virtual void alert(const std::string&, const std::string& = "", const std::string& = "", int = 0, const std::string& = "ALERT");
	/**
     Messaggio di log di livello warning
     @param message messaggio di log
     @param file file dal quale è stato richiamato il metodo 
     @param function funzione dalla quale è stato richiamato il metodo
     @param line linea del file dal quale è stato richiamato il metodo
     @param level stringa che indica il livello del messaggio di log che verrà utilizzata per 
     		generare il messaggio da scrivere su file
     @see send()
     */
	virtual void warning(const std::string&, const std::string& = "", const std::string& = "", int = 0, const std::string& = "WARNING");
	/**
     Messaggio di log di livello fatal
     @param message messaggio di log
     @param file file dal quale è stato richiamato il metodo 
     @param function funzione dalla quale è stato richiamato il metodo
     @param line linea del file dal quale è stato richiamato il metodo
     @param level stringa che indica il livello del messaggio di log che verrà utilizzata per 
     		generare il messaggio da scrivere su file
     @see send()
     */
	virtual void fatal(const std::string&, const std::string& = "", const std::string& = "", int = 0, const std::string& = "FATAL");
    /**
     Crea un'istanza della classe Message, lo inizializza con le informazioni ricevute e lo 
     invia alla coda dei messaggi di log
     @code
         Message log_message;
         
         log_message.add("message", message);
         log_message.add("level", level);
         log_message.add("file", file);
         log_message.add("function", function);
         log_message.add("line", line);
     @endcode
     @param message  messaggio di log
     @param file file dal quale è stato richiamato il metodo 
     @param function funzione dalla quale è stato richiamato il metodo
     @param line linea del file dal quale è stato richiamato il metodo
     @param level stringa che indica il livello del messaggio di log che verrà utilizzata per 
     		generare il messaggio da scrivere su file
     @see send()
     */
    virtual int send(const std::string&, const std::string& = "", const std::string& = "", int = 0, const std::string& = "LOG");
    /**
     Riceve le informazioni loggare in un'istanza di Message e prepara la stringa da scrivere
     su file/stdout/stderr strutturata in base al layout scelto
     @param log_message informazioni da loggare 
     @see hsrv::strReplace()
     */
    virtual void writeLog(Message& log_message);
    /**
     Inizializza i layout disponibili inserendo delle coppie di valori nella mappa dei layout.
     Ogni coppia di valori è formata da una stringa che indica il nome del layout e dalla 
     stringa che rappresenta la struttura del messaggio da utilizzare per i messaggi di log
     */
	virtual void initLayouts();
    /**
     Imposta il layout da utilizzare per i messaggi di log
     @param selLayout nome del layout
     */
	virtual void setLayout(std::string);
    /**
     Imposta il tipo di output desiderato
     @param type tipo di output (stdout/stderr/file)
     */
    virtual void setOutputType(const std::string&);
    /**
     Restituisce il layout impostato
     @return stringa con il modello di layout impostato
     */
	virtual std::string getLayout();
	
private:
	
    /**
     @var static std::string
     @brief layout del messaggio di log 
     */
    std::string layout;
    /**
     @var int
     @brief tipo di output da utilizzare
            0 => standard output
            1 => standard error
            2 => file
     */
    int output_type;
    /**
     @var static std::map<std::string, std::string>
     @brief mappa che contiene i layout disponibili per i messaggi di log
     */
    std::map<std::string, std::string> layouts;
    
};

#endif
