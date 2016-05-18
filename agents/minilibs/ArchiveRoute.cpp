#include "agentlib.h"


MMessage ArchiveRoute::exec(std::string uri, std::vector<std::string> params, std::string method, std::string body) {
    std::string response, filetype="";
    MMessage mreq, mresp;
    
    mreq = MMessage("httprequest");
    if (uri != "") {
      mreq.add("url", uri); 
    } else {
      mreq.add("url", "."); 
    }


    if (method == "getFirst") {
	response = hsrv::archive->getFirst(mreq, filetype);

    } else if (method == "getLast") {
	response = hsrv::archive->getLast(mreq, filetype);
    
    } else if (method == "getAll") {
	response = hsrv::archive->getAll(mreq);
    
    } else if (method == "getFile") {
	response = hsrv::archive->getFile(mreq);
	filetype = FileManager::getExt(uri);        
  
    } else if (method == "getDir") {
	response = hsrv::archive->getDir(mreq);
	filetype = FileManager::getExt(uri);        
  
    } else if (method == "getList") {
	response = hsrv::archive->getList(mreq);        
  
    } else if (method == "getSubSet") {
	mreq.add("body", body);
	response = hsrv::archive->getSubSet(mreq);

    } else if (method == "getDynamicPage") {
        response = hsrv::getDynamicPage(uri);
	filetype = FileManager::getExt(uri);
    } else if (method == "putConf") {
	mreq.add("body", body);
	MMessage mresp = hsrv::archive->putConf(mreq);
	response = mresp.getString("body");
	filetype = FileManager::getExt(uri);
    } else if (method == "deleteRes") {
	bool done = hsrv::archive->deleteResource(uri);
	if (done) {
	  response = "OK";
	} else {
	  response = "Forbidden";
	}
	filetype = "plain/text";

    } else {
        response = "Invalid method";
    }

    if (response == "Not Found" or response == "Invalid Method" or response == "Bad Request") {
    	mresp.add("content", "plain/text");
    
    } else if (filetype == "") {
	response = "<\?xml version=\"1.0\" \?><list>"+response+"</list>";	
        mresp.add("content", "plain/text");
    
    } else if (filetype == "xml") { 
        std::string type = "text/"+filetype;
	mresp.add("content", type);

    } else if (filetype == "html") { 
        std::string type = "text/"+filetype;
	mresp.add("content", type);

    } else if (filetype == "json") {
        std::string type = "application/json";
        mresp.add("content", type);

    } else if (filetype == "js") {
        std::string type = "application/javascript";
        mresp.add("content", type);

    } else if (filetype == "jpg"||filetype == "jpeg") {
        std::string type = "image/jpeg";
        mresp.add("content", type);

    } else if (filetype == "gif"||filetype == "png") {
        std::string type = "image/"+filetype;
        mresp.add("content", type);

    } else if (filetype == "css") {
        std::string type = "text/css";
        mresp.add("content", type);
    }

    mresp.add("body", response);

    return mresp;
}
