using namespace std;

#include "JParser.h"

JParser::JParser(string k1, boost::property_tree::ptree& pt) {
    stringPair ww;
    BOOST_FOREACH(boost::property_tree::ptree::value_type &v, pt) {
      ww.first = v.second.get<string>(k1);
      push_back(ww);
    }
}

JParser::JParser(string k1, string k2, boost::property_tree::ptree& pt) {
    stringPair ww;
    BOOST_FOREACH(boost::property_tree::ptree::value_type &v, pt) {
      ww.first = v.second.get<string>(k1);
      ww.second = v.second.get<string>(k2);
      push_back(ww);
    }
}

JParser::JParser(string k1, string k2, string k3, boost::property_tree::ptree& pt) {
    stringPair ww;
    BOOST_FOREACH(boost::property_tree::ptree::value_type &v, pt) {
      ww.first = v.second.get<string>(k1);
      ww.second = v.second.get<string>(k2);
      ww.second = v.second.get<string>(k3);
      push_back(ww);
    }
}

JParser::JParser(string k1, string k2, string k3, string k4, boost::property_tree::ptree& pt) {
    stringPair ww;
    BOOST_FOREACH(boost::property_tree::ptree::value_type &v, pt) {
      ww.first = v.second.get<string>(k1);
      ww.second = v.second.get<string>(k2);
      ww.second = v.second.get<string>(k3);
      ww.second = v.second.get<string>(k4);
      push_back(ww);
    }
}
