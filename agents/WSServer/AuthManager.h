/* 
 * File:   AuthManager.h
 * Author: attilio
 *
 * Created on July 6, 2015, 12:37 PM
 */

#ifndef AUTHMANAGER_H
#define	AUTHMANAGER_H
#include "agentlib.h"
#include "NameTable.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>


class User: public MParams {
public:
  User(std::string, std::string);
  User(){};
  std::string getField(std::string);
  bool setField(std::string, std::string);
  bool print();
};
   
class AuthManager {
public:
    static bool init(std::string dir);
    static MMessage execSet(std::vector<std::string>& params, std::string& method, std::string body="");
    static MMessage execGet(std::vector<std::string>& params, std::string& method, std::string body="");
    static MMessage execCheck(std::vector<std::string>& params, std::string& method, std::string body="");
    static boost::mutex mutexclusion;
    static bool printUsers();
protected:
    static MParams par;
    static MParams user_template;
    static std::map<std::string, User> users;
    bool static deleteUser(std::string);
    bool static addUser(std::string, std::string, MMessage&);
    static bool isUniquePasswd(std::string, std::string); 
    static std::string getUserOf(std::string, std::string); 
};

#endif	/* RPCTABLE_H */
