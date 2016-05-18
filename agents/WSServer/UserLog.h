#ifndef USERLOG_H
#define USERLOG_H

#include "agentlib.h"

class UserLog {
public:
   UserLog(MParams&);
   bool log(MMessage&);
private:
   std::string remap(std::string);
   std::string getKey(std::string&);
   std::map<std::string, NameList*> logmap;
   std::map<std::string, std::string> pathname;
   std::map<std::string, std::string> proto;
   std::map<std::string, NameList*> source;
   bool checkSource(std::string, std::string);
};

#endif /* USERLOG_H */
