#ifndef new_soap_Router_h
#define new_soap_Router_h

#include <iostream>
#include <vector>
#include <string>
#include <regex.h>
#include "../tinyxml/tinyxml.h"
#include "../libjson/libjson.h"

using namespace std;


// superclasse che prefissa tutte le classi che dovranno ricevere un uri da eseguire
class RouterExec {
public:
    std::string name;
    RouterExec(std::string n) { name = n; };
    //* metodo che process gli uri
    // 1) parametro: uri, 2) parametro: vettore di parametri contenete le variabili estratte dall'uri,
    // 3) parametro: il metodo interno che deve essere utilizzato
    virtual MMessage exec(std::string uri, std::vector<std::string> params, std::string method, string body="") { return name; };

};

//contiene le stringhe estratte dalla descrizione xml che specificano i vincoli sui parametri
class Condition {
public:
    std::string param;
    std::string regexp;
};

// descrive uno delle possibili vie che può accogliere un uri
class Route {
public:
    NameList _template;
    std::string format;
    std::string method; // POST, GET, PUT, DELETE
    std::vector<Condition> conditions; //vettore delle condizioni che deve soddisfare l'uri
    std::string called_method; // metodo che deve essere chiamato nella class prefissato con RouterExec
    RouterExec* target; // classe che dovra` elaborare l'uri (chiamata target->exec(...) )

    static bool match_regexp(std::string reg, std::string s);
};

class RoutingMap : public std::vector<Route> {
public:
    MMessage route(std::string uri, std::string method, std::string body="");
    RoutingMap(std::string file);
    
protected:
    virtual void loadClasses();
    bool load(std::string);
    std::vector<RouterExec*> instances; 
};


#endif
