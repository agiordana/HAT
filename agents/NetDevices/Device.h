/*
 * Device.h
 *
 *  Created on: Jun 23, 2017
 *      Author: Attilio
 */

#ifndef DEVICE_H_
#define DEVICE_H_

#include "agentlib.h"
#include "ObserverThread.h"
#include "Component.h"
#include "Automaton.h"

class Device: public ObserverThread, public MParams {
public:
	string name;
	Device(std::string dir, std::string nm);
	~Device();
    void start_service_threads();
    bool getObservations(MParams& obs);
    bool getSubjects(MParams& subj);
    bool setSync(unsigned N, unsigned i) {cycle = N; turn = i; return true;};

protected:
	unsigned cycle;
	unsigned turn;
	unsigned scount;
	std::vector<Component> component;
	Automaton* control;
	void do_work(Device* obj);
	bool load(std::string& dir, std::string& nm);
	std::string status();
	std::string status(std::string);
        bool sync();
	bool publish();
	std::string aggregateValue(std::string);
	bool eventNotify();
	std::string valueNormalize(std::string&, std::string&, std::string);
};

#endif /* DEVICE_H_ */
