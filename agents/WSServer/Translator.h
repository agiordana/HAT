#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include "agentlib.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>


class Translator: public MParams {
public:
   Translator(std::string);
   std::string log2text(boost::property_tree::ptree::value_type&);
   std::string dummy();
private:
   std::string remap(std::string&, std::string&);
   std::string getKey(std::string&);
   std::string clean(std::string&);
   std::string timeFormat(std::string&);
};

#endif /* TRANSLATOR_H */
