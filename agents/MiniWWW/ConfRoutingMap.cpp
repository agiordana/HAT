using namespace std;
#include "ConfRoutingMap.h"


ConfRoutingMap::ConfRoutingMap(string file): RoutingMap(file) {
    this->loadClasses();
    clear();
    RoutingMap::load(file);
}

void ConfRoutingMap::loadClasses() {
    ArchiveRoute *i1;
    CmdRoute *i2;
    LoggerRoute *i3;
    Configurator *i4;
    RouterExec *def;
    i1 = new ArchiveRoute();
    i2 = new CmdRoute();
    i3 = new LoggerRoute();
    i4 = new Configurator();
    def = new RouterExec("Default");
    instances.clear();
    
    instances.push_back(i1);
    instances.push_back(i2);
    instances.push_back(i3);    
    instances.push_back(i4);    
    instances.push_back(def);
    
    return;
}

