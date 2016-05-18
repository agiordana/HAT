/* 
 * File:   WebSocket.cpp
 * Author: Davide Monfrecola<davide.monfrecola@gmail.com>
 * 
 * Created on February 8, 2013, 10:54 AM
 */

#include "agentlib.h"

#include <boost/algorithm/string/replace.hpp>
#include <boost/asio.hpp>

using namespace std;

WebSocket::WebSocket() {
}

WebSocket::WebSocket(const WebSocket& orig) {
}

WebSocket::~WebSocket() {
}

void WebSocket::on_open(websocketpp::server_tls::connection_ptr conn) {
    if(_DEBUG_) hsrv::logger->info("-- A Client "+ this->get_conn_id(conn) +" has joined. --");
    
    this->connections.insert(pair<string, websocketpp::server_tls::connection_ptr>(this->get_conn_id(conn), conn));
}

void WebSocket::on_close(websocketpp::server_tls::connection_ptr conn) {
    map<string, connection_ptr>::iterator it;
    map<string, vector<websocketpp::server_tls::connection_ptr> >::iterator subscribers_it;
    string connection_id = this->get_conn_id(conn);
    
    // rimuove sottoscrizioni
    for(subscribers_it = this->connections_subscriptions.begin();
        subscribers_it != this->connections_subscriptions.end();
        subscribers_it++) 
    {
        // scorre tutte le connessioni presenti
        for(unsigned int i = 0;i < subscribers_it->second.size();i++) {
            // se trova la connessione nel vettore delle sottoscrizioni la elimina
            if(this->get_conn_id(subscribers_it->second[i]) == connection_id) {
                subscribers_it->second.erase(subscribers_it->second.begin() + i);
                if(_DEBUG_) {
                    hsrv::logger->info("[WebSocket::on_close] Client "+ connection_id +" removed from subscription " + subscribers_it->first);
                    stringstream tmp;
                    tmp << "[WebSocket::on_close] Vector size: " << subscribers_it->second.size();
                    hsrv::logger->info(tmp.str());
                }
            }
            
        }
    }
    
    it = this->connections.find(connection_id);
    if(it != this->connections.end()) {
        // il client si √® disconnesso ed √® gi√† stato eliminato dalla lista dei
        // client attivi. Si pu√≤ ignorare.
        this->connections.erase(it);
        if(_DEBUG_) hsrv::logger->info("-- Client "+ connection_id +" has left.");
    }
}

void WebSocket::on_message(websocketpp::server_tls::connection_ptr conn, message_ptr msg) {
    
}

boost::shared_ptr<boost::asio::ssl::context> WebSocket::on_tls_init() {
    // create a tls context, init, and return.
    boost::shared_ptr<boost::asio::ssl::context> context(new boost::asio::ssl::context(boost::asio::ssl::context::tlsv1));
    try {
        context->set_options(boost::asio::ssl::context::default_workarounds |
                             boost::asio::ssl::context::no_sslv2 |
                             boost::asio::ssl::context::single_dh_use | 
                             boost::asio::ssl::context::verify_none);
        context->set_password_callback(boost::bind(&WebSocket::get_password, this));
        context->use_certificate_chain_file(hsrv::configdir + "/certs/server.pem");
//        context->use_certificate_chain_file(certs.get("cert_chain_file"));
        context->use_private_key_file(hsrv::configdir + "/certs/server.pem", boost::asio::ssl::context::pem);
//        context->use_private_key_file(certs.get("private_key_file"), boost::asio::ssl::context::pem);
        context->use_tmp_dh_file(hsrv::configdir + "/certs/dh512.pem");
//        context->use_tmp_dh_file(certs.get("tmp_dh_file"));

    } catch (std::exception& e) {
        hsrv::logger->error(e.what());
    }
    return context;
}

string WebSocket::get_conn_id(websocketpp::server_tls::connection_ptr conn) {
    std::stringstream endpoint;
    endpoint << conn;
    return endpoint.str();
}

void WebSocket::send_to_all(std::string data) {
    map<string, connection_ptr>::iterator it;
    
    for (it = this->connections.begin(); it != this->connections.end(); it++) {
        (*it).second->send(data);
    }
}

void WebSocket::send_to_subscribers(string event_type, string data) {
    map<string, vector<websocketpp::server_tls::connection_ptr> >::iterator it;
    vector<websocketpp::server_tls::connection_ptr>::iterator conn_it;
    
    it = this->connections_subscriptions.find(event_type);
    
    // √® presente un vettore di connessioni alle quali inviare il messaggio
    if(it != this->connections_subscriptions.end()) {
        
        if(it->second.size() > 0) {
            
            for(conn_it = it->second.begin();conn_it != it->second.end();conn_it++) {
                if(_DEBUG_) hsrv::logger->info("[WebSocket::send_to_subscribers] Send message (event_type:" + event_type + ")");
                (*conn_it)->send(data);
            }
//            for(unsigned i = 0;i < it->second.size();i++) {
//                websocketpp::server_tls::connection_ptr* conn = it->second[i];
//                (*conn)->send(data);
//            }
        }
        
    }
}

bool WebSocket::subscribe(string conn_id, string event_type) {
    map<string, vector<websocketpp::server_tls::connection_ptr> >::iterator it;
    map<string, websocketpp::server_tls::connection_ptr>::iterator conn_it;
    
    // recupera puntatore alla connessione 
    conn_it = this->connections.find(conn_id);
    
    if(conn_it == this->connections.end()) {
        if(_DEBUG_) hsrv::logger->info("[WebSocket::subscribe] Connection ID not found: " + conn_id);
        return false;
    }
    
    it = this->connections_subscriptions.find(event_type);
    
    // evento non presente, creo vettore
    if(it == this->connections_subscriptions.end()) {
        vector<websocketpp::server_tls::connection_ptr> tmp;
        tmp.push_back(conn_it->second);
        
        pair<string, vector<websocketpp::server_tls::connection_ptr> > pair_tmp(event_type, tmp);
        this->connections_subscriptions.insert(pair_tmp);
        
        if(_DEBUG_) hsrv::logger->info("[WebSocket::subscribe] Connection for event " + event_type + " added (new vector created)");
    }
    // evento presente, aggiungo connessione al vettore
    else {
        it->second.push_back(conn_it->second);
        
        if(_DEBUG_) hsrv::logger->info("[WebSocket::subscribe] Connection for event " + event_type + " added (push back)");
    }
    
    return true;
}

bool WebSocket::unsubscribe(string conn_id, string event_type) {
    map<string, vector<websocketpp::server_tls::connection_ptr> >::iterator subscribers_it;
    
    subscribers_it = this->connections_subscriptions.find(event_type);
    
    if(subscribers_it != this->connections_subscriptions.end()) {
        
        // scorre tutte le connessioni presenti
        for(unsigned int i = 0;i < subscribers_it->second.size();i++) {
            // se trova la connessione nel vettore delle sottoscrizioni la elimina
            if(this->get_conn_id(subscribers_it->second[i]) == conn_id) {
                subscribers_it->second.erase(subscribers_it->second.begin() + i);
                if(_DEBUG_) {
                    hsrv::logger->info("[WebSocket::unsubscribe] Client "+ conn_id +" removed from subscription " + subscribers_it->first);
                }
            }
            
        }
        
    }
    else {
        // connessione non presente nelle liste delle subscription
        return false;
    }
    
    return true;
}
