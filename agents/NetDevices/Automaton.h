/*
 * Atomaton.h
 *
 *  Created on: Jun 25, 2017
 *      Author: Attilio
 */

#ifndef ATOMATON_H_
#define ATOMATON_H_
#include "agentlib.h"

class Automaton: public MParams {
public:
	Automaton(std::string dir, std::string nm);
	bool getObservations(MParams& obs);
	std::string nextState(std::string);
	NameList actions(std::string ty, std::string subty, std::string v, std::string status);
};


#endif /* ATOMATON_H_ */
