#include "agentlib.h"


MMessage LoggerRoute::exec(std::string uri, std::vector<std::string> params, std::string method, std::string body) {
    std::string response, devname, filetype="";
    MMessage mreq, mresp;
    NameList nl;
    size_t pos;
    
    mreq = MMessage("httprequest");

    nl.init(uri, '/');
    devname = nl.back();
    pos = devname.find_last_of('.');
    if (pos != std::string::npos) {
    	devname = devname.substr(0, pos);
    }

    if (method == "getLightStatus") {
	response = getDevStatus(devname);
    
    } else if (method == "getSwitchedLightStatus") {
        uri = "/event/"+devname+"_state/last";
        mreq.add("url", uri);
	response = hsrv::archive->getLast(mreq, filetype);
    
    } else if (method == "getDimmerStatus") {
	response = getDimmerStatus(devname);

    } else if (method == "getShutterStatus") {
	response = getShutterStatus(devname);

    } else if (method == "getSocketStatus") {
	response = getDevStatus(devname);
    } else if (method == "getGateStatus") {
	response = getDevStatus(devname);
    
    } else if (method == "getThermostateStatus") {
        uri = "/event/"+devname+"/last";
	mreq.add("url", uri);
	response = hsrv::archive->getLast(mreq, filetype);

    } else if (method == "getUmidityStatus") {
        uri = "/event/"+devname+"/last";
	mreq.add("url", uri);
	response = hsrv::archive->getLast(mreq, filetype);
    
    } else if (method == "getGasValveStatus") {
	response = getDevStatus(devname);
   
    } else if (method == "getFireValveStatus") {
	response = getDevStatus(devname);
   
    } else if (method == "getIrrigatorStatus") {
	response = getDevStatus(devname);
   
    } else if (method == "getCatStatus") {
	response = getCatStatus(devname);
   
    } else if (method == "putLog") {
    	uri = "/event/"+devname+"_state/";
	mreq.add("url", uri);			
	mreq.add("body", body);	
	MMessage mresp = hsrv::archive->putLog(mreq);
	response = mresp.getString("body");   

    } else if (method == "postLog") {
    	uri = "/event/"+devname+"_state/";
        mreq.add("url", uri);			
	mreq.add("body", body);
	MMessage mresp = hsrv::archive->postLog(mreq);
	response = mresp.getString("body");   

    } else if (method == "getPMLog") {
        mreq.add("url", uri);			
	mreq.add("body", body);
    	response = hsrv::archive->getPMLog(mreq);  

    } else {
        response = "Invalid method";
    }

    if (response == "Not Found" or response == "Invalid Method" or response == "Bad Request") {
    	mresp.add("content", "plain/text");
    } else {
        mresp.add("content", "application/json");
    }
    mresp.add("body", response);

    return mresp;
}


std::string LoggerRoute::getDimmerStatus(std::string devname) {
    std::string filetype, last, response, uri;
    boost::property_tree::ptree pt; 
    stringstream ss;   
    MMessage m;
    int val;   
    string color; 

    uri = "/event/"+devname+"_state/last";
    m.add("url", uri);
    last = hsrv::archive->getLast(m, filetype);
    ss << last;
    response = "{\"response\":\n\n\t{\"message\":\"ok\",";
    response += "\"device\":\n\n\t\t{\"name\":";
    response += "\"" + devname + "\",\"status\":";  

    read_json(ss, pt);
    if(devname.find("RG_") != string::npos) color =  pt.get<string>("color");
	else color = "";
    val = (int)pt.get<float>("value");
    response += hsrv::int2a(val);
    if(color != "") {
	response += ",\"color\":\"";
	response += color;
	response += "\"";
    }
       
    response += "}\n\n\t}\n\n}";
 
    return response;
}


std::string LoggerRoute::getDevStatus(std::string devname) {
    std::string filetype, last, response, uri;
    boost::property_tree::ptree pt; 
    stringstream ss;   
    MMessage m;
    int val;   
    string aval;
 
    uri = "/event/"+devname+"_state/last";
    m.add("url", uri);
    last = hsrv::archive->getLast(m, filetype);
    
    if (last == "Not Found" or last == "Forbidden") {
	return last;
    }
    ss << last;
    response = "{\"response\":\n\n\t{\"message\":\"ok\",";
    response += "\"device\":\n\n\t\t{\"name\":";
    response += "\"" + devname + "\",\"status\":";  

    read_json(ss, pt);
    aval = pt.get<string>("value");
    if (aval=="1" || aval=="ON") {
	response += "\"true\"";
    } else {
	response += "\"false\"";
    }
       
    response += "}\n\n\t}\n\n}";
    return response;
}

std::string LoggerRoute::getCatStatus(std::string catname) {
    std::string filetype, last, response, uri;
    vector<string> filelist;
    MMessage m;
    size_t j;
    string catpath = hsrv::homedir+"/devices/"+catname; 
    FileManager::dirList(catpath, filelist);
    response = "{\"response\":\n\n\t{\"message\":\"ok\",\"devices\": [\n";
    for(size_t i=0; i<filelist.size(); i++) {
      m.clear();
      uri = "/devices/"+catname+"/" + filelist[i] + "/last";
      m.add("url", uri);
      last = hsrv::archive->getLast(m, filetype);
      if (last == "Not Found" && last == "Forbidden") return last;
      response += "\t\t\t{\"name\":\"";
      response += filelist[i];
      response += "\",";
      for(j=0; j<last.size()&&last[j]!='{'; j++);
      response += last.substr(++j);
      if(i<filelist.size()-1) response += ",\n";
	else response += "\n";
    }
    response += "\t\t]\n\t}\n}";
    return response;
}


std::string LoggerRoute::getShutterStatus(std::string devname) {
    std::string filetype, last, response, uri;
    boost::property_tree::ptree pt; 
    stringstream ss;   
    MMessage m;
    int val;   
    string aval;   
    
    uri = "/event/"+devname+"_state/last";
    m.add("url", uri);
    last = hsrv::archive->getLast(m, filetype);

    if (last == "Not Found") {
 	return last;
    
    }
    else {
	ss << last;
        response = "{\"response\":\n\n\t{\"message\":\"ok\",";
    	response += "\"device\":\n\n\t\t{\"name\":";
    	response += "\"" + devname;
    	read_json(ss, pt);
    	aval = pt.get<string>("value");
    	if (aval=="UP") {
	  response +=  + "\",\"status\":\"up\"";  
    	} else if(aval=="DOWN") {
  	  response +=  + "\",\"status\":\"down\"";  
	}
	else {
	  response +=  + "\",\"status\":\"off\"";
	}
    }
       
    response += "}\n\n\t}\n\n}";
    return response;
}

