#include "agentlib.h"

bool Route::match_regexp(std::string exp, std::string str) {
    regex_t reg;
    regcomp(&reg, exp.c_str(), REG_EXTENDED);
    int match = regexec(&reg, str.c_str(), 0, NULL, 0);

    if (match != REG_NOMATCH) {
        return true;
    } else {
        return false;
    }
}



RoutingMap::RoutingMap(std::string file) {
    
    this->loadClasses();
    load(file);
}
    
bool RoutingMap::load(std::string file) {
    TiXmlDocument *doc = new TiXmlDocument(file.c_str());
    TiXmlElement *root, *elem, *sub, *c;
    this->clear();
    if (doc->LoadFile()) {
        root = doc->FirstChildElement();     
        elem = root->FirstChildElement();
        
        for (; elem != NULL; elem=elem->NextSiblingElement()) {
            Route r;
            for (sub = elem->FirstChildElement(); sub!=NULL; sub=sub->NextSiblingElement()) {

                if (string(sub->Value()) == "uri") {
                    sub->QueryStringAttribute("path", &r.format);
                    sub->QueryStringAttribute("method", &r.method);
                
                } else if (string(sub->Value()) == "call") {
                    bool found = false;
                    std::string i_class;
                    sub->QueryStringAttribute("method", &r.called_method);
                    sub->QueryStringAttribute("class", &i_class);
                    
                    //link to the proper existing routerExec
                    for (int i=0; i<(int)instances.size(); ++i) {

			if (instances[i]->name == i_class) {
                            r.target = instances[i];
                            found = true;
                            break;
                        }
                    }
                    
                    if (not found) {
                        r.target = instances.back();
                    }
                    
                    
                } else if (string(sub->Value()) == "conditions") {
                    for (c = sub->FirstChildElement(); c!=NULL; c=c->NextSiblingElement()) {
                        Condition cond;
                        c->QueryStringAttribute("param", &cond.param);
                        c->QueryStringAttribute("regexp", &cond.regexp);
                        r.conditions.push_back(cond);
                    }
                }
            }
            this->push_back(r);
        }
    } else {
//	std::cout << "No route config file found!" << std::endl;
	return false;
    }
    

    for (int i=0; i<(int)this->size(); ++i) {
        Route r = this->at(i);
        NameList nl;
        
        nl.init(r.format, '/');
        if (nl.front() == "") {
            nl.erase(nl.begin());
        }
        
        for (int j=0; j<(int)nl.size(); ++j) {
            if (nl[j][0] == ':') {
                for (int k=0; k<(int)r.conditions.size(); ++k) {
                    if (r.conditions[k].param == string(nl[j])) {
                        nl[j] = r.conditions[k].regexp;
                        break;
                    }
                }
            }
        }
        
        this->at(i)._template = nl;
    }   
    return true;
}


void RoutingMap::loadClasses() {
    ArchiveRoute *i1;
    CmdRoute *i2;
    LoggerRoute *i3;
    RouterExec *def;
    
    i1 = new ArchiveRoute();
    i2 = new CmdRoute();
    i3 = new LoggerRoute();
    def = new RouterExec("Default");
    
    instances.push_back(i1);
    instances.push_back(i2);
    instances.push_back(i3);    
    instances.push_back(def);
    
    return;
}



MMessage RoutingMap::route(std::string uri, std::string method, std::string body) {
    NameList uriNL;
    int idx = -1;
    bool match;

    uriNL.init(uri, '/');
    if (uriNL.front() == "") {
        uriNL.erase(uriNL.begin());
    }
        
    for (int i=0; i<(int)this->size(); ++i) {
        Route r = this->at(i);
        match = false;
     
        if (r._template.size() == uriNL.size() and method == r.method) {
            match = true;
	    for (int j = 0; j < (int) r._template.size(); ++j) {
		if (not Route::match_regexp(r._template[j] , uriNL[j])) {
              	    match = false;
                    break;
                }
            }
        }
        
        if (match) {
            idx = i;
            break;
        }
    }    
    
    if (idx < 0 ) {
        MMessage mresp;
	mresp.add("body","Not Found");
	mresp.add("content", "plain/text");
	return mresp;    

    } else {
        Route r = this->at(idx);
//	std::cout <<r.target->name << " " << r.called_method << std::endl;
	return r.target->exec(uri, (std::vector<string>(uriNL)), r.called_method, body);
    }   
}

