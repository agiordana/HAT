#include "agentlib.h"


MMessage CmdRoute::exec(std::string uri, std::vector<std::string> params, std::string method, std::string body) {
    MMessage mreq, mresp, rpcanswer;
    rpcanswer.clear();
    size_t i;
    string dummy = "*";

    mresp.add("content", "plain/text");    
  
    if (params.size() < 3){
      mresp.add("body", "Not Found");
      return mresp;
    }

    mreq = MMessage("cmd");
    if (uri != "") {
      if(params[0] == "cmd" || params[0] == "cmdw") mreq.mtype= "cmd";
      else if(params[0] == "set" || params[0] == "setw") mreq.mtype= "set";
      mreq.add("url", uri); 
    } else {
      mreq.add("url", "."); 
    }

    if (method == "cmdExec") {
      mreq.msubtype = params[1];
      mreq.add("value", params[2]);
      if(body !="") addBody(mreq, body);;
      if (SubjectSet::check_subject(mreq.mtype, params[1])||SubjectSet::check_subject(mreq.mtype,dummy)) {
	 if((params[0] == "cmdw" || params[0] == "setw") && hsrv::rpctab != NULL) hsrv::rpctab->callRegister(mreq);
	 SubjectSet::notify(mreq); 
      }
      if((params[0] == "cmdw" || params[0] == "setw") && hsrv::rpctab != NULL) {
	 rpcanswer = hsrv::rpctab->wait(mreq.getString("tag"));
      }
    }
    else if (method == "webcmdExec") {
      for (i=0; i<params.size() && params[i]!="Devices"; i++);
      if (i >= params.size()-2 || params[i] != "Devices") {
	mresp.add("body", "Not Found");
	return mresp;
      }
      mreq.msubtype = params[i+1];
      mreq.add("value", FileManager::getStem(params[i+2]));
      if (SubjectSet::check_subject(mreq.mtype, mreq.msubtype)||SubjectSet::check_subject(mreq.mtype,dummy)) {
	SubjectSet::notify(mreq);
      }
    }
    if(rpcanswer.getString("answer_tag") != "")  {
       string value = rpcanswer.getString("value");
       mresp.add("body", "done:"+value);
    }
    else mresp.add("body", "done");
    return mresp;
}

bool CmdRoute::addBody(MMessage& m, std::string body) {
   if(isHtml(body)) {
      NameList tmp;
      tmp.init(body,'&');
      for(size_t i=0; i<tmp.size(); i++) {
         NameList row;
         row.init(tmp[i], '=');
	 if(row.size()==2) m.add(row[0], row[1]);
      }
   }
   else {
     m.add("body", body);
   }
   return true;
}

bool CmdRoute::isHtml(std::string& body) {
   if(body.find("&")!=std::string::npos) return true;
   else return false;
}
