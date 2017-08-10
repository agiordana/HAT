/*
 * Component.h
 *
 *  Created on: Jun 25, 2017
 *      Author: Attilio
 */

#ifndef COMPONENT_H_
#define COMPONENT_H_
#include "agentlib.h"

class Component: public MParams {
public:
	Component(std::string,std::string);
	std::string name();
	bool on();
	bool off();
	std::string setValue(std::string);
	std::string status();
	std::string active();
	std::string setActive(std::string);
	bool sync();
	bool getObservations(MParams& obs);
	bool getSubjects(MParams& subj);
private:
	bool check();
	std::string lstatus;
	std::string lmode;
	bool mkAgentRPC();
	std::string remote_ag_addr;
	std::string getValue(std::string&, std::string);
	RpcCall* rpc;
};

#endif /* COMPONENT_H_ */
