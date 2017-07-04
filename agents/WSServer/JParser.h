#ifndef JPARSER_H
#define JPARSER_H
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

class stringPair {
public:
   std::string first;
   std::string second;
   std::string third;
   std::string fourth;
   stringPair() { first = second = third = fourth = "";};
};

class JParser: public std::vector<stringPair> {
public:
   JParser(std::string k1, boost::property_tree::ptree& pt);
   JParser(std::string k1, std::string k2, boost::property_tree::ptree& pt);
   JParser(std::string k1, std::string k2, std::string k3, boost::property_tree::ptree& pt);
   JParser(std::string k1, std::string k2, std::string k3, std::string k4, boost::property_tree::ptree& pt);
};

#endif
