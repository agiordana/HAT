#include "agentlib.h"
#include "Route.h"
#include "ArchiveRoute.h"
#include "Configurator.h"

class ConfRoutingMap: public RoutingMap {
public:
   ConfRoutingMap(std::string file);
protected:
   void loadClasses();
};
