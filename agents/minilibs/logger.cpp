//
//  logger.cpp
//  phoneagent
//
//  Created by Davide Monfrecola on 11/14/11.
//  Copyright 2011 Università del Piemonte Orientale. All rights reserved.
//

using namespace std;

#include "agentlib.h"

/***************************************************************************************************
 *------------------------------------------- Logger ----------------------------------------------*
 **************************************************************************************************/
HorusLogger::HorusLogger(const string &logPath) throw() {
    initLayouts();
#if _DEBUG_ == 1
    setOutputType("stdout");
#else
    setOutputType("file");
#endif
    setLayout("verbose"); // basic layout
}

void HorusLogger::debug(const std::string& message, const std::string& file, const std::string& function, int line, const std::string& level) {
    send(message, file, function, line, level);
}

void HorusLogger::info(const std::string& message, const std::string& file, const std::string& function, int line, const std::string& level) {
    send(message, file, function, line, level);
}

void HorusLogger::error(const std::string& message, const std::string& file, const std::string& function, int line, const std::string& level) {
    send(message, file, function, line, level);
}

void HorusLogger::alert(const std::string& message, const std::string& file, const std::string& function, int line, const std::string& level) {
    send(message, file, function, line, level);
}

void HorusLogger::warning(const std::string& message, const std::string& file, const std::string& function, int line, const std::string& level) {
    send(message, file, function, line, level);
}

void HorusLogger::fatal(const std::string& message, const std::string& file, const std::string& function, int line, const std::string& level) {
    send(message, file, function, line, level);
}


void HorusLogger::setLayout(string selLayout) {
    layout = selLayout;
}

string HorusLogger::getLayout() {
    return layouts[layout];	
}

void HorusLogger::setOutputType(const string& type) {
    if (type == "stdout")       // output su standard output
        output_type = 0;
    else if(type == "stderr")   // output su standard error
        output_type = 1;
    else
        output_type = 2;        // output su file di log
    
}

void HorusLogger::initLayouts() {
    layouts.insert(pair<string, string>("basic", "(%level%) %message%"));
    layouts.insert(pair<string, string>("verbose", "[%datetime%](%level%) %message% [%file%:%line%]"));
}

int HorusLogger::send(const std::string& message, const std::string& file, const std::string& function, int line, const std::string& level) {
    Message log_message;
    
    log_message.add("message", message);
    log_message.add("level", level);
    log_message.add("file", file);
    log_message.add("function", function);
    log_message.add("line", line);
    
    MsgQueue<Message>::send(log_message);
    
    return 0;
}

void HorusLogger::writeLog(Message& log_message) {
    
    //string output_string(layouts[layout]);
    string output_string(getLayout());
    stringstream ss;
    string header("--- Logger ---"), footer("--------------"), file(log_message.getString("file")); 
    int line = log_message.getInt("line");
    
    hsrv::strReplace(output_string, "%level%", log_message.getString("level"));
    hsrv::strReplace(output_string, "%datetime%", hsrv::mySQLdate()+" "+hsrv::getasciitimeday());
    hsrv::strReplace(output_string, "%message%", log_message.getString("message"));
     
    if(file != "")
        hsrv::strReplace(output_string, "%file%", file);
    else
        hsrv::strReplace(output_string, "%file%", "(file not specified)");
     
    if(line != 0)
         hsrv::strReplace(output_string, "%line%", hsrv::int2a(line));
    else
         hsrv::strReplace(output_string, "%line%", "(line not specified)");
     
    ss << output_string << endl;
    switch (output_type) {
        // standard output 
        case 0:
            cout << header << endl;
            cout << ss.str() << endl;
            cout << footer << endl;
            fflush(stdout);
        break;
            // standard error
        case 1:
            cerr << header << endl;
            cerr << ss.str() << endl;
            cerr << footer << endl;
            fflush(stderr);
        break;
         // file
        case 2:
            ofstream logfile;
            string file_path;
	    if(hsrv::logfile[0]=='/') file_path = hsrv::logfile;
	      else file_path = hsrv::homedir+"/"+hsrv::logfile;
            logfile.open(file_path.c_str(), ios::app);
            logfile << ss.str();
            logfile.close();
        break;
     
     }
 }


