#include "agentlib.h"
#include "Route.h"
#include "ArchiveRoute.h"
#include "RpcServer.h"

class RpcRoutingMap: public RoutingMap {
public:
   RpcRoutingMap(std::string file);
protected:
   void loadClasses();
};
