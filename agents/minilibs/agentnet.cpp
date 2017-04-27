#include <pthread.h>
#include <ifaddrs.h>
#include <arpa/inet.h>

#include "agentlib.h"
#include "cipher.h"
#include "udpclient.h"
#include "udpserver.h"

#define MULTICAST_GROUP "226.0.0.2"
#define MULTICAST_PORT "54321"

#ifdef MACOS
#define NET_TYPE "en0"
#define NET_TYPE1 "en1"
#else
#define NET_TYPE "eth0"
#define NET_TYPE1 "eth1"
#endif

using namespace std;

MParams NetSubjects::importedSubjects;
MParams NetSubjects::exportedSubjects;

std::map<std::string, ObserverTCP*> AgentNet::tcp_observers;
MParams* AgentNet::netpar;
std::string AgentNet::mport;
std::string AgentNet::mgroup;
std::string AgentNet::public_key;
std::string AgentNet::private_key;
int AgentNet::validityTime;
int AgentNet::check_wait;
int AgentNet::send_wait;
bool AgentNet::debug;
bool AgentNet::send_agent_info = false;
std::map<std::string, AgentInfo> AgentNet::foreign_agents;

std::string AgentNet::wsserver_ip = "";
std::string AgentNet::wsserver_port = "";
boost::mutex AgentNet::mutexclusion;

std::vector<unsigned> AgentNet::ports;
AgentInfo* AgentNet::myinfo;
std::string AgentNet::crypt;
std::vector<std::string> AgentNet::mysubjects;
std::vector<std::string> AgentNet::externsubjects;
std::vector<boost::thread*> AgentNet::service_thread;
boost::thread AgentNet::client;
boost::thread AgentNet::server;
time_t AgentNet::last_subject_update = time(NULL);
// novelty announces in net_info that the agents is just started/restarted or that it ha changed the configuration
// the announce is repeated three times
int AgentNet::novelty = 3;

bool NetSubjects::initSubjects(string fname, MParams& subj, string ty) {
    NameList accepted;
    MParams param(fname);
    param.load();
    if (ty == "import") {
        accepted.init(param.get("import", "netsubjects"), '+');
        for (size_t i=0; i<subj.size(); i++) {
            if(subj[i].category == "observe" && accepted.member(subj[i].value)) {
                NetSubjects::importedSubjects.add(subj[i].name, "observe", subj[i].value);
            }
        }
    }
    else {
        accepted.init(param.get("export", "netsubjects"), '+');
        for (size_t i=0; i<subj.size(); i++) {
            if(subj[i].category == "subject" && accepted.member(subj[i].value)) {
                NetSubjects::exportedSubjects.add(subj[i].name, "subject", subj[i].value);
            }
        }
    }
    return true;
}

bool NetSubjects::print(string ty) {
    if(ty=="import") {
        for (size_t i=0; i<NetSubjects::importedSubjects.size(); i++) {
            cout<<NetSubjects::importedSubjects[i].value<<" "<<NetSubjects::importedSubjects[i].name<<endl;
        }
    }
    else {
        for (size_t i=0; i<NetSubjects::exportedSubjects.size(); i++) {
            cout<<NetSubjects::exportedSubjects[i].value<<" "<<NetSubjects::exportedSubjects[i].name<<endl;
        }
    }
    return true;
}

/**
**************************************************************
******************* METODI STATICI PUBBLICI *******************
***************************************************************/

// inizializza la mappa, settando per ogni agente:
// numero di porta standard, indirizzo IP iniziale "0.0.0.0", timestamp iniziale nullo
// setta il proprio indirizzo IP nella tabella
bool AgentNet::init(string n) {

	//inizializzo variabili statiche
    check_wait=30;
    send_wait=10;
    validityTime=180;
    debug=false;
    string tmp;

	//inizializzo tabella
    setof<MAttribute> p;
    setof<MAttribute> subj;
    setof<MAttribute> obs;
    netpar = new MParams(n);
    netpar->load();

// initialize the multicast port and group;
    MParams multicast;
    string path = FileManager::getRoot(hsrv::configdir) +"/miniwww/multicast.xml";
    if(FileManager::isFile(path)) multicast.xmlLoad(path);
    if(multicast.get("mport") != "") mport = multicast.get("mport");
	else mport = MULTICAST_PORT;
    if(multicast.get("mgroup") != "") mgroup = multicast.get("mgroup");
	else mgroup = MULTICAST_GROUP;
// now multicast port and group have been set!

    private_key = hsrv::configdir + "/" +netpar->get("private_key","generic");
    public_key = hsrv::configdir + "/" +netpar->get("public_key","generic");
    crypt = netpar->get("crypt","mode");
    if (netpar->get("send_agent_info")== "YES" || netpar->get("send_agent_info")=="yes" ) {
        send_agent_info = true;
	NetSubjects::exportedSubjects.add("agent_info", "subject", "net");
    }
    p=netpar->extractCategory("port");
    subj = netpar->extractCategory("subject");
    obs = netpar->extractCategory("observe");
    for(size_t i=0; i<subj.size(); i++) {
        tmp = subj[i].name + "." + subj[i].value;
        mysubjects.push_back(tmp);
    }
    for(size_t i=0; i<NetSubjects::exportedSubjects.size(); i++) {
        tmp = NetSubjects::exportedSubjects[i].name + "." + NetSubjects::exportedSubjects[i].value;
        mysubjects.push_back(tmp);
    }
    for(size_t i=0; i<obs.size(); i++) {
        tmp = obs[i].name + "." + obs[i].value;
        externsubjects.push_back(tmp);
    }
    for(size_t i=0; i<NetSubjects::importedSubjects.size(); i++) {
        tmp = NetSubjects::importedSubjects[i].name + "." + NetSubjects::importedSubjects[i].value;
        externsubjects.push_back(tmp);
    }
    AgentNet::MakePage();
    for(unsigned i=0; i<p.size(); i++) {
        if (p[i].name!=hsrv::agentname) ports.push_back(hsrv::a2unsigned(p[i].value));
    }
    myinfo = retrieveInfo();
    if((tmp = netpar->get("send_period","time"))!="") send_wait = hsrv::a2int(tmp);
    if((tmp = netpar->get("check_period","time"))!="") check_wait = hsrv::a2int(tmp);
    if((tmp = netpar->get("validity_time","time"))!="") validityTime = hsrv::a2int(tmp);
    
    string agentnet_debug("");
    if((tmp = netpar->get("debug","mode"))!="") agentnet_debug = tmp;
    if(agentnet_debug == "ON") debug = true;
    
    addSubjects();
    start();
    return true;
}

bool AgentNet::SubjectsUpdate() {
   string tmp;
   for(size_t i=0; i<NetSubjects::exportedSubjects.size(); i++) {
        tmp = NetSubjects::exportedSubjects[i].name + "." + NetSubjects::exportedSubjects[i].value;
       if(!isIn(mysubjects, tmp)) {
           mysubjects.push_back(tmp);
           SubjectSet::add_subject(NetSubjects::exportedSubjects[i].value, NetSubjects::exportedSubjects[i].name);
       }
    }
   for(size_t i=0; i<NetSubjects::importedSubjects.size(); i++) {
        tmp = NetSubjects::importedSubjects[i].name + "." + NetSubjects::importedSubjects[i].value;
       if(!isIn(externsubjects, tmp)) {
            externsubjects.push_back(tmp);
           SubjectSet::add_subject(NetSubjects::importedSubjects[i].value, NetSubjects::importedSubjects[i].name);
       }
    }
   myinfo = retrieveInfo();
   AgentNet::MakePage();
   if(myinfo != NULL) delete myinfo;
   myinfo = retrieveInfo();
   return true;
}

//stampa informazioni sull'agente stesso e il contenuto della sua mappa (se stesso escluso)
bool AgentNet::printMap() {
    map<string,AgentInfo>::iterator it;
    for (it=foreign_agents.begin(); it!=foreign_agents.end(); it++) it->second.Print();    
    return true;
}

//recupera l'indirizzo IP associato all'agente di nome agentname, se i suoi dati sono validi; altrimenti restituisce "127.0.0.1"
string AgentNet::getAgentIp(string agentname) {
	if (agentname==hsrv::agentname || check_agent_status(agentname)) 
        return foreign_agents[agentname].getString("ip");	 
	return "127.0.0.1";
}

string AgentNet::getAgentIp() {
	return myinfo->getString("ip");
}

//recupera la port associato all'agente di nome agentname, se i suoi dati sono validi; altrimenti restituisce "127.0.0.1"
string AgentNet::getAgentPort(string agentname) {
	if (agentname==hsrv::agentname || check_agent_status(agentname)) 
        return foreign_agents[agentname].getString("port");	 
	return "8888";
}

string AgentNet::getAgentPort() {
    return myinfo->getString("port");	 
}

double AgentNet::getSendWait() {
    return (double)send_wait;	 
}

/**************************************************************
*************** METODI PUBBLICI DELL'ISTANZA ******************
***************************************************************/

//fa partire il thread di tipo n, se non ancora attivo.
//Il tempo di attesa tra un ciclo e l'altro e' predefinito e dipende dal tipo di thread.
bool AgentNet::start() {
    AgentNet::client = boost::thread(AgentNet::send_status);
    AgentNet::server = boost::thread(AgentNet::recv_status);
    return true;
}


/************************************************************************
******************** METODI PRIVATI: GENERICI *******************
*************************************************************************/

//recupera dalla tabella le informazioni associate all'agente con nome agent
AgentInfo *AgentNet::retrieveInfo(string agent) {
	if (foreign_agents.count(agent)) {
		return &foreign_agents[agent];
	}
	return NULL;
}

//recupera dalla tabella le informazioni associate all'agente con nome agent
AgentInfo *AgentNet::retrieveInfo() {
	AgentInfo* mydata;
    vector<string> events;

   for(size_t i=0; i<externsubjects.size(); i++)
       events.push_back(externsubjects[i]);
    
    mydata = new AgentInfo();
    mydata->add("agentname", hsrv::agentname);
    mydata->add("ip", retrieve_my_ip());
    mydata->add("port", retrieve_my_port());
    mydata->add("observe", events);
    mydata->xmlformat = mydata->xmlEncode(1);
    mydata->add("status", "active");
    AgentConf::setStatus(ON);
    return mydata;
}

//recupero l'indirizzo IP della macchina
string AgentNet::retrieve_my_ip() {
    struct ifaddrs * ifAddrStruct=NULL;
    struct ifaddrs * ifa=NULL;
    void * tmpAddrPtr=NULL;

    //recupero la lista delle interfacce di rete e relativi indirizzi IP
    if (getifaddrs(&ifAddrStruct)<0) return "127.0.0.1";

    //scorro la lista alla ricerca dell'interfaccia NET_TYPE con indirizzo IPv4
    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa ->ifa_addr->sa_family==AF_INET && strncmp(ifa->ifa_name,NET_TYPE,5)==0) { 
            //recupero e restituisco una stringa contenente l'indirizzo IP
            tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            return addressBuffer;
        }
    }
    //scorro la lista alla ricerca dell'interfaccia NET_TYPE1 con indirizzo IPv4
    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa ->ifa_addr->sa_family==AF_INET && strncmp(ifa->ifa_name,NET_TYPE1,5)==0) { 
            //recupero e restituisco una stringa contenente l'indirizzo IP
            tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            return addressBuffer;
        }
     }
    //libero struttura
    if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);
    //se l'indirizzo cercato non e' stato trovato, restituisco 127.0.0.1
    return "127.0.0.1";
}

string AgentNet::retrieve_my_port() {
    return netpar->get("port");
}

bool AgentNet::mkTcpObserverXML(MMessage &m) {
    string agent = m.getString("agentname");
    string filename = "observers/"+agent;
    string ty;
    string subty;
    MParams desc(filename);
    desc.ty=agent;
    vector<string> events = m.getList("observe");
    evFilter(events);
    desc.add("name",agent);
    desc.add("agent", agent);
    desc.add("address", m.getString("ip"));
    desc.add("port", m.getString("port"));
    desc.add("language", "xml");
    desc.add("status", "ON");
    for (unsigned i=0; i<events.size(); i++) {
        split(events[i],ty,subty);
        desc.add(subty,"observe",ty);
    }
    desc.save();
    return true;
}

bool AgentNet::split(string& source, string& ty, string& subty) {
    subty = ty = "";
    unsigned i;
    for(i=0; i<source.size()&&source[i]!='.'; i++) subty+=source[i];
    i++;
    while (i<source.size()) {
        ty += source[i++];
    }
    return true;
}

bool AgentNet::addSubjects() {
    string ty;
    string subty;
    for(unsigned i=0; i<mysubjects.size(); i++) {
        split(mysubjects[i], ty, subty);
        SubjectSet::add_subject(ty, subty); 
    }
    for(unsigned i=0; i<externsubjects.size(); i++) {
        split(externsubjects[i], ty, subty);
        SubjectSet::add_subject(ty, subty); 
    }
    return true;
}

/**************************************************************
************* METODI PRIVATI: CHECK EXPIRED ************
***************************************************************/

// thread del check: controlla il contenuto della tabella ogni *wait secondi
bool AgentNet::check_expired() {
    string dir = "observers/";
    string fname;
    string info;
    vector<string> tokill;
    map<string, AgentInfo>::iterator it;
    for(it=foreign_agents.begin(); it!=foreign_agents.end(); it++)
        if(!check_agent_status(it->first)) {
            info = "agent "+it->first+" doesn't autenticate: --- DISCONNECTED!!!";
            tokill.push_back(it->first);
            fname = it->first+".xml";
            hsrv::logger->info(info,__FILE__, __FUNCTION__, __LINE__);
            if(FileManager::isFile(dir, fname))
                FileManager::deleteFile(dir,fname);
        }
    for(unsigned i=0; i<tokill.size(); i++) {
        info = "agent "+tokill[i]+" doesn't autenticate: --- DISCONNECTED!!!";
        hsrv::logger->info(info,__FILE__, __FUNCTION__, __LINE__);
        disactivateObserverTCP(tokill[i]);
        foreign_agents.erase(foreign_agents.find(tokill[i]));
        fname = tokill[i]+".xml";
        FileManager::deleteFile(dir,fname);
    }
    return true;
}


//controlla se l'agente agent è ancora valido,
//ovvero se il suo timestamp è stato aggiornato negli ultimi validityTime secondi
//se l'agente è valido, restituisce TRUE, altrimenti FALSE
bool AgentNet::check_agent_status(string agent) {
	time_t now=time(NULL);
    if (foreign_agents.count(agent)) {
		time_t timestamp=foreign_agents[agent].timestamp;
		//l'agente non e' piu' valido
        if((now-timestamp) >validityTime) return false;
        else return true;
    }
    return false;
}


/************************************************************************
****************** METODI PRIVATI: THREAD SEND ******************
*************************************************************************/

//thread del send: invia informazioni relative all'agente ogni *wait secondi
void *AgentNet::send_status() {
	int time=send_wait;
	time_t now;
    int r=1;

    //attiva porta UDP
    UDPClient client(mgroup, mport);
    if(crypt=="ON") Cipher::init(private_key, public_key);
    r = client.start(crypt);
    if (r!=1) { //socket fallita, esce con errore, segnalare!
        if (debug) cout << "Send_status: " << client.getErr() << endl;
        return NULL;
    }
	
	//invio il messaggio a tutti gli altri agenti in multicast, attendo e reinvio
    while (1) {
	//costruisco il messaggio con nome agente agentname, indirizzo IP myself->ip, numero di porta myself->port
	string msg = myinfo->xmlformat;

        if(myinfo->getString("status")!="active") {
            now = hsrv::gettime();
	    if(foreign_agents.count(hsrv::agentname)>0) myinfo->timestamp = foreign_agents[hsrv::agentname].timestamp;
            if(now - myinfo->timestamp > validityTime) {
		myinfo->add("status", "active");
		AgentConf::setStatus(ON);
		}
        }
        if(myinfo->getString("status") == "active") {
            if (client.sendMsg(msg)<0) {
                string info = "Invio multicast fallito!";
                hsrv::logger->info(info, __FILE__, __FUNCTION__, __LINE__);
            }
        }
        if(send_agent_info) {
            
            MMessage tcp_msg("net", "agent_info");
            tcp_msg.add("agentname", myinfo->getString("agentname"));
            tcp_msg.add("agenttype", hsrv::agenttype);
            tcp_msg.add("address", myinfo->getString("ip"));
            tcp_msg.add("port", myinfo->getString("port"));
            tcp_msg.add("status", myinfo->getString("status"));
            tcp_msg.add("time", hsrv::gettime());
	    if(testNovelty()) {
		tcp_msg.add("first_announce","TRUE");
	    }
        
            if(debug) hsrv::logger->debug("--- Sending agent info message: " + tcp_msg.xmlEncode(1), __FILE__, __FUNCTION__, __LINE__);
        
            SubjectSet::notify(tcp_msg);
        }
        sleep(time);
    }

	//chiudo la connessione ed esco
	client.stop();
    return NULL;
}


/************************************************************************
****************** METODI PRIVATI: THREAD RECV ******************
*************************************************************************/

//thread recv: ricevo informazioni sullo stato dei vari agenti e li salvo nella tabella
void *AgentNet::recv_status() {	
    int r;
	//creazione connessione	
	UDPServer server(mgroup, mport);	
	if (!server.start(crypt)) {
		string info = "Recv_status: inizializzazione fallita, impossibile ricevere lo stato degli altri agenti";
		hsrv::logger->info(info, __FILE__, __FUNCTION__, __LINE__);
		return NULL;
	}
    	
	while(1) {
	
		//leggo messaggio ricevuto
		string decifrato;
		if ((r=server.recvMsg(decifrato,check_wait))<0) {
			string info = "Recv_status: impossibile ricevere dati";
			hsrv::logger->info(info, __FILE__, __FUNCTION__, __LINE__);
			continue;
		}
		if(r>0) {
		//decodifico contenuto messaggio
            MMessage m("agentinfo");
            m.xmlDecode(decifrato);
            //modifico contenuto della mappa
	    if(m.getString("agentname")==hsrv::agentname&&hsrv::agentname!="configurator") {
		if(m.getString("ip") >myinfo->getString("ip")) {
			myinfo->timestamp = hsrv::gettime();
			myinfo->add("status", "waiting");
			AgentConf::setStatus(OFF);
		}
	    }
            else {
		update(m);
		
                //stampo mappa modificata
                if (debug) AgentNet::printMap();
	    }
        }
        check_expired();
    }
	
	server.stop();
	return NULL;
}

//controllo se il messaggio ricevuto è valido
bool AgentNet::checkMsg(MMessage m, struct sockaddr_in this_addr) {
	if (foreign_agents.count(m.getString("name"))<=0) return false;	
	if (inet_ntoa(this_addr.sin_addr)!=m.getString("ip")) return false;
	return true;
}

//aggiorno dati all'interno della mappa
//Da modificare secondo le nuove strutture!!!!!!!!!!
bool AgentNet::update(MMessage& m) {
    string agent = m.getString("agentname");
    vector<string> ev;
    ev = m.getList("observe");
    /**
      setta l'indirizzo del wsserver;
      se il nome dell'agente e' wsserver.
    */
    if(agent == "wsserver") {
	wsServerUpdate(m);
    }
    if(!evFilter(ev)) return false;
    if (foreign_agents.count(agent)==0) {
        string info = "agent "+agent+" requests communication: --- CONNECTED "+m.getString("ip")+":"+m.getString("port");
        hsrv::logger->info(info,__FILE__, __FUNCTION__, __LINE__);
        foreign_agents[agent] = AgentInfo(m.xmlEncode(0));
        foreign_agents[agent].add("agentname", agent);
        foreign_agents[agent].add("ip", m.getString("ip"));
        foreign_agents[agent].add("port", m.getString("port"));
        foreign_agents[agent].add("observe", ev);
        mkTcpObserverXML(m);
        activateObserverTCP(agent);
    }
	else {
        if (m.getString("ip")!=foreign_agents[agent].getString("ip")) {
            disactivateObserverTCP(agent);
            string info = "agent "+agent+" has now ip: "+m.getString("ip");
            hsrv::logger->info(info,__FILE__, __FUNCTION__, __LINE__);
            foreign_agents[agent].add("ip", m.getString("ip"));
            mkTcpObserverXML(m);
            activateObserverTCP(agent);
        }
        if ((foreign_agents[agent].getList("events")!=m.getList("events"))||last_subject_update > foreign_agents[agent].timestamp) {
            disactivateObserverTCP(agent);
            string info = "agent "+agent+" updated event-list";
            hsrv::logger->info(info,__FILE__, __FUNCTION__, __LINE__);
            ev = m.getList("events");
            foreign_agents[agent].add("events", ev);
            mkTcpObserverXML(m);
            activateObserverTCP(agent);
        }
    }
    foreign_agents[agent].timestamp=time(NULL);
	return true;
}

bool AgentNet::evFilter(vector<string>& ev) {
    vector<string> tev;
    for(unsigned i=0; i<ev.size(); i++) {
        for(unsigned j=0; j<mysubjects.size(); j++)
            if (mysubjects[j]==ev[i]) {
                tev.push_back(ev[i]);
                    
            }
            else {
                string mys1 = mysubjects[j].substr(0,mysubjects[j].find("."));
                string mys2 = mysubjects[j].substr(mysubjects[j].find(".")+1);
                string eev1 = ev[i].substr(0,ev[i].find("."));
                string eev2 = ev[i].substr(ev[i].find(".")+1);

                if ((isStar(mys1)||isStar(eev1)) && eev2 == mys2) {
                    tev.push_back(ev[i]);
                }
            }
    }
    ev = tev;
    if(ev.size()>0) return true;
    else return false;
}

bool AgentNet::isStar(string& subj) {
    size_t found = subj.find("*");
    return found != string::npos;
}

bool AgentNet::activateObserverTCP(string& name) {
    tcp_observers.insert(pair<string, ObserverTCP*>(name, new ObserverTCP("observers/", name)));
    tcp_observers[name]->start();
    return true;
}

bool AgentNet::disactivateObserverTCP(string& name) {
    map<std::string, ObserverTCP*>::iterator it;
    it = tcp_observers.find(name);
    it->second->stop();
    SubjectSet::remove_observer(it->second);
    tcp_observers.erase(it);
    return true;
}

bool AgentNet::MakePage() {
    MMessage page("subjects");
    for (size_t i=0; i<AgentNet::mysubjects.size(); i++) {
        page.add(AgentNet::mysubjects[i], "exported");
    }
    for (size_t i=0; i<AgentNet::externsubjects.size(); i++) {
        page.add(AgentNet::externsubjects[i], "imported");
    }
    page.add("language", "xml");
    AgentConf::pageRegister("/subjects.xml", page);
    page.add("language", "html");
    AgentConf::pageRegister("/subjects", page);

    return true;
}

bool AgentNet::isIn(vector<string>& v, string& s) {
  for(size_t i=0; i<v.size(); i++)
    if(v[i]==s) return true;
  return false;
}

bool AgentNet::SubjectDateUpdate() {
    last_subject_update = time(NULL);
    return true;
}


bool AgentNet::SubjectRegister() {
    ofstream imported;
    imported.open("imported.xml");
    imported << "<set type=\"events\">";
    for(size_t i=0; i<AgentNet::externsubjects.size(); i++) {
	imported <<"<attribute name=\""<<externsubjects[i]<<"\" value=\"ON\"/>";
    }
    imported<< "</set>"<<endl;
    imported.close();
    string localurl = "subjects/imported/data";
    string pathname = hsrv::homedir+"/imported.xml";
    hsrv::archive->addResource(localurl, pathname, true);
    FileManager::deleteFile(hsrv::homedir, "imported.xml");
    ofstream exported;
    exported.open("exported.xml");
    exported << "<set type=\"events\">";
    for(size_t i=0; i<AgentNet::mysubjects.size(); i++) {
	exported <<"<attribute name=\""<<mysubjects[i]<<"\" value=\"ON\"/>";
    }
    exported<< "</set>"<<endl;
    exported.close();
    localurl = "subjects/exported/data";
    pathname = hsrv::homedir+"/exported.xml";
    hsrv::archive->addResource(localurl, pathname, true);
    FileManager::deleteFile(hsrv::homedir, "exported.xml");
    return true;
}

bool AgentNet::wsServerUpdate(MMessage& m) {
    boost::unique_lock<boost::mutex> lock(mutexclusion);
    if(m.getString("agentname") != "wsserver") return false;
    wsserver_ip = m.getString("ip");
    wsserver_port = m.getString("port");
    return true;
}

std::string AgentNet::getWSServer() {
    boost::unique_lock<boost::mutex> lock(mutexclusion);
    if(wsserver_ip !="" && wsserver_port != "") {
       std::string answer = wsserver_ip + ":" + wsserver_port;
       return answer;
    }
    return "";
}

bool AgentNet::setNovelty(int n) {
    boost::unique_lock<boost::mutex> lock(mutexclusion);
    novelty = n;
    return true;
}

bool AgentNet::testNovelty() {
    boost::unique_lock<boost::mutex> lock(mutexclusion);
    if(novelty > 0) {
	novelty--;
	return true;
    }
    else return false;
}
