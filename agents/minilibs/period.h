/*
 *  period.h
 *  phoneagent
 *
 *  Created by Attilio Giordana on 10/27/11.
 *  Copyright 2011 Università del Piemonte Orientale. All rights reserved.
 *
 */

class Period {
public:
	std::string name;
	std::string t0;
	std::string t1;
	std::vector<std::string> activeEvents(std::string&);
	std::vector<std::string>* isActive(int);	//compatibility with old version
	virtual int printstring(std::ostringstream&);
	int htmlprint(std::ostringstream&);
	int xmlprint(std::ostringstream&);
	Period() { t0 = t1 = "";};
	Period(std::string&);
	Period(std::string&, std::string);
	Period(TiXmlNode*);
private:
	std::vector<std::string> event;
	TiXmlNode* xmldesc;
	int getEvents(TiXmlNode*);
};
