/*
 *  dayprogram.h
 *  phoneagent
 *
 *  Created by Attilio Giordana on 10/27/11.
 *  Copyright 2011 Università del Piemonte Orientale. All rights reserved.
 *
 */

class DayProgram: public std::vector<Period> {
public:
	std::string day;
	std::string week;
	std::string month;
	std::string year;
	std::string accessKey();
	virtual int printstring(std::ostringstream&);
	int htmlprint(std::ostringstream&);
	int xmlprint(std::ostringstream&);
	std::vector<std::string>* isActive(int); 
	DayProgram(){day = week = month = year = currentPeriod=""; xmldesc=NULL;};
	DayProgram(std::string&, TiXmlNode*);
    //	DayProgram(Param&);
private:
	std::string currentPeriod;
	TiXmlNode* xmldesc;
	
};