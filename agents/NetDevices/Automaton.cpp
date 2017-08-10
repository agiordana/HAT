/*
 * Automaton.cpp
 *
 *  Created on: Jun 25, 2017
 *      Author: Attilio
 */

#include "Automaton.h"

using namespace std;

Automaton::Automaton(string dir, string nm): MParams::MParams(dir, nm) {
	load();
}

string Automaton::nextState(string currentState) {
	return get(currentState,"control");
}

bool Automaton::getObservations(MParams &obs) {
	NameList cmds;
	NameList events;
	cmds.init(get("cmd", "trigger"), '+');
	events.init(get("event", "trigger"), '+');
	for(size_t i=0; i<events.size(); i++) obs.add(events[i],"observe","event");
	for(size_t i=0; i<cmds.size(); i++) obs.add(cmds[i],"observe","cmd");
	return true;
}

NameList Automaton::actions(string ty, string subty, string v, string status) {
	NameList al;
	if(get(ty, "trigger")=="") return al;
	NameList values;
	values.init(v,'+');
	if(values.size() >1) al.init(v,'+');
	else if(values.size()==1 && v == "ON") {
	   al.init(get(status, "control"),'+');
	}
	else if(values.size()==1 && v == "OFF") {
	   al.init(status, '+');
	   for(size_t i=0; i< al.size(); i++) al[i] = "OFF";
	}
	return al;
}
