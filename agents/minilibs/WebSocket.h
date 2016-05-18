/* 
 * File:   WebSocket.h
 * Author: Davide Monfrecola<davide.monfrecola@gmail.com>
 *
 * Created on February 8, 2013, 10:54 AM
 */

#ifndef WEBSOCKET_H
#define	WEBSOCKET_H

#include <websocketpp/sockets/tls.hpp>
#include <websocketpp/websocketpp.hpp>

class WebSocket : public websocketpp::server_tls::handler {
public:
    WebSocket();
    WebSocket(const WebSocket& orig);
    virtual ~WebSocket();
    
    /**
     * Aggiunge la connessione appena aperta alla lista delle connessioni attive
     * 
     * @param conn puntatore alla connessione
     */
    void on_open(websocketpp::server_tls::connection_ptr);
    
    /**
     * Richiamata quando un client chiude la connessione. Il server la elimina
     * dalla lista delle connessioni attive
     * 
     * @param conn puntatore alla connessione
     */
    void on_close(websocketpp::server_tls::connection_ptr);
    
    /**
     * Esegue azioni ogni volta che riceve un messaggio da un client
     * 
     * @param conn puntatore alla connessione
     * @param msg puntare al messaggio ricevuto
     */
    virtual void on_message(websocketpp::server_tls::connection_ptr, message_ptr);
    
    boost::shared_ptr<boost::asio::ssl::context> on_tls_init();
    
    std::string get_password() const { return "test"; }
    
    void http(connection_ptr con) {
        con->set_body("<!DOCTYPE html><html><head><title>WebSocket++ TLS certificate test</title></head><body><h1>WebSocket++ TLS certificate test</h1><p>This is an HTTP(S) page served by a WebSocket++ server for the purposes of confirming that certificates are working since browsers normally silently ignore certificate issues.</p></body></html>");
    }
    
protected:
    /**
     * Invia il messaggio a tutte le connessioni attive
     * 
     * @param data stringa che contiene i dati da inviare ai client
     */
    void send_to_all(std::string data);

    /**
     * Ritorna ID della connessione
     * 
     * @param conn puntatore alla connessione 
     * @return ritorna stringa ID client
     */
    std::string get_conn_id(websocketpp::server_tls::connection_ptr);    
    
    /**
     * @var std::map<connection_ptr,std::string>
     * @brief lista delle connessioni attive (client connessi che devono ricevere
     *        gli update)
     */
    std::map<std::string, websocketpp::server_tls::connection_ptr> connections;
    
    /**
     * @var std::map<std::string, std::vector<*websocketpp::server_tls::connection_ptr> >
     * @brief contiene la lista delle sottoscrizione effettuate dalle varie 
     *        connessioni. Ogni client, una volta instaurata la connessione, invia
     *        messaggi di tipo subscribe dove indica quale tipologia di messaggi
     *        è interessato a ricevere
     */
    std::map<std::string, std::vector<websocketpp::server_tls::connection_ptr> > connections_subscriptions;
    
    /**
     * Invia messaggio a tutte le connessioni attive che hanno richiesto l'invio
     * della tipologia di messaggio passato come parametro
     * 
     * @param event_type nome evento
     * @param data stringa che contiene i dati da inviare ai client
     */
    void send_to_subscribers(std::string event_type, std::string data);
    
    /**
     * Aggiunge alla mappa dei subscribers la connessione passato come parametro.
     * Effettua il subscribe del client per l'evento specificato
     * 
     * @param conn_id ID che identifica la connessione
     * @param event_type nome evento 
     * @return true
     */
    bool subscribe(std::string conn_id, std::string event_type);
    
    /**
     * Elimina dalla mappa dei subscribers la connessione passata come parametro.
     * Effettua il unsubscribe del client per l'evento specificato
     * 
     * @param conn_id ID che identifica la connessione
     * @param event_type nome evento 
     * @return true se l'eliminazione va a buon fine, false altrimenti
     */
    bool unsubscribe(std::string conn_id, std::string event_type);
    
private:
    
    /**
     * Encoding del messaggio da inviare ai client. Crea un messaggio in formato
     * JSON contenente le informazioni da inviare ai client
     * 
     * @param sender mittente
     * @param msg contenuto del messaggio
     * @param escape 
     * @return ritorna una stringa in formato JSON che verrà inviata ai vari client
     */
    std::string encode_message(std::string sender, std::string msg, bool escape = true);
    
    /**
     * Estrae le informazioni dal messaggio JSON ricevuto, crea un'istanza di 
     * MMessage contenente i dati appena ricevuti
     * 
     * @param 
     * @param 
     * @return 
     */
    MMessage decode_message(std::string, message_ptr);
    
};

#endif	/* WEBSOCKET_H */

