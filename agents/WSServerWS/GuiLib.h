
/* 
 * File:   GuiLib.h
 * Author: attilio
 *
 * Created on September 6, 2016, 22:00 PM
 */

#ifndef GUILIB_H
#define	GUILIB_H
#include "agentlib.h"

class DeviceHolder {
public:
  std::string device;
  DeviceHolder(std::string name);
  std::string describeHTML();
private:
  std::string description;
  std::string getProto(std::string);
};

class BarHolder: public std::vector<DeviceHolder> {
public:
   BarHolder(std::string);
   bool add(std::string);
   std::string describeHTML();
protected:
   std::string description;
   std::string getProto(std::string);
};

#endif	/* GUILIB_H */
