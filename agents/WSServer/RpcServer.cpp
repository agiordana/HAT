using namespace std;

#include "RpcServer.h"

MMessage RpcServer::exec(std::string uri, vector<string> params, string method, string body) {
   MMessage answer;
   if(method == "rpcExecGet") {
      answer = DeviceManager::execGet(params, method, body);
   }
   if(method == "rpcExecGetSet") {
      answer = DeviceManager::execGetSet(params, method, body);
   }
   if(method == "rpcExecPut") {
      answer = DeviceManager::execPut(params, method, body);
   }
   if(method == "rpcExecPost") {
      answer = DeviceManager::execPost(params, method, body);
   }
   else if(method == "rpcExecPostSet") {
      answer = DeviceManager::execPostSet(params, method, body);
   }
   else if(method == "rpcAuthMngGet") {
      answer = AuthManager::execGet(params, method, body);
   }
   else if(method == "rpcAuthMngSet") {
      answer = AuthManager::execSet(params, method, body);
   }
   else if(method == "rpcAuthMngCheck") {
      answer = AuthManager::execCheck(params, method, body);
   }
   else if(method == "rpcAlarmMngGet") {
      answer = AlarmManager::execGet(params, method, body);
   }
   else if(method == "rpcAlarmMngPut") {
      answer = AlarmManager::execPut(params, method, body);
   }
   else if(method == "rpcAlarmMngPost") {
      answer = AlarmManager::execPost(params, method, body);
   }
   else if(method == "rpcAutomatismMngGet") {
      answer = AutomatismManager::execGet(params, method, body);
   }
   else if(method == "rpcAutomatismMngPut") {
      answer = AutomatismManager::execPut(params, method, body);
   }
   else if(method == "rpcAutomatismMngPost") {
      answer = AutomatismManager::execPost(params, method, body);
   }
   else if(method == "rpcLogMngPost") {
      answer = LogManager::execPost(params, method, body);
   }
   else if(method == "rpcPwrMngGet") {
      answer = PwrManager::execGet(params, method, body);
   }
   else if(method == "rpcPwrMngPost") {
      answer = PwrManager::execPost(params, method, body);
   }
   else if(method == "rpcSrvMngGet") {
      answer = SrvManager::execGet(params, method, body);
   }
   else if(method == "rpcSrvMngPut") {
      answer = SrvManager::execPut(params, method, body);
   }
   return answer;
}
