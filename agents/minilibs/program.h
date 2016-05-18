/*
 *  program.h
 *  phoneagent
 *
 *  Created by Attilio Giordana on 10/27/11.
 *  Copyright 2011 Università del Piemonte Orientale. All rights reserved.
 *
 */

class Program: public std::map<std::string, DayProgram> {
public:
    std::string name;
    virtual int printstring(std::ostringstream&);
    int htmlprint(std::ostringstream&){std::cout<<"htmlprint: to be done!"<<std::endl; return 1;};
    int xmlprint(std::ostringstream&, int md= 0);
    int mkhtmlchoice(std::vector<std::string>*, std::ostringstream&) {return 0;};
    int mkxmlplist(std::vector<std::string>*, std::ostringstream&);
    std::vector<std::string> activeEvents();
    int install(std::string prg="");
    int save(std::string fname) {doc->SaveFile(fname.c_str()); return 1;};
    int log(std::ofstream&);
    int prgversions(std::ostringstream&);
    bool getProgramsList(std::map<std::string, std::string>&);
    bool getProgramsVersions(std::map<std::string, std::string>&);
    std::vector<std::string>* isActive();
    std::string isalarmon();
    Program(std::string);
    Program(){current=""; doc = NULL;};
    ~Program() {if(doc) delete doc;};
    int load(std::string);
    int load();
    std::string currentProgram() {return current; };
private:
    std::string year;
    std::string version;
    TiXmlDocument* doc;
    std::string current;
    int clear();
    std::vector<std::string> mkaccesskey(std::vector<int>&);
    DayProgram* getDay(std::vector<std::string>&);
    int plistSort(std::vector<std::string>*);
    std::string readattr(std::string, std::string);
    bool isFile(std::string file);
        
};
