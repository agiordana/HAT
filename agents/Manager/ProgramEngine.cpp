#include "ProgramEngine.h"

using namespace std;

ProgramEngine::ProgramEngine(string n) {
    id = n;
 //   MParams params("threads/ProgramEngine");
 //   params.load();
    MParams global("/global");
    global.load();
    string startprg = global.get("startprogram");
    program = new Program();
    program->install(startprg);
    program->load();

    previous_program = "";
    id = "ProgramEngine";
}

ProgramEngine::~ProgramEngine() {
    
}

void ProgramEngine::start_service_threads() {
    this->service_thread.push_back(new Thread(&ProgramEngine::do_work, this));
}

void ProgramEngine::do_work(ProgramEngine* obj) {
    MMessage out_mess;
    MMessage msg;
    bool mod=false;
    setof<MMessage> action_todo;
    obj->program_notify(obj->program, active, true);
    obj->makeStatusPage(*obj->program, obj->active);
    obj->makeVersionPage(*obj->program, obj->active);
 //   hsrv::threadcheck->monitor(obj->id);
    while(true) {
        msg = obj->receive_message();
        
        if(msg.mtype == "trigger" && msg.msubtype == "prgupdate") {
            mod = program_notify(obj->program, active);
        }
        else if (msg.mtype == "cmd" && msg.msubtype == "setprogram") {
            setprogram(msg, obj->program);
	    if(msg.getString("tag") != "") hsrv::rpcNotify(msg.getString("tag"), "value", ("value="+program->currentProgram()));
            mod = program_notify(obj->program, active, true);
            //program_info(msg);
            
        }
        else if(msg.mtype == "trigger" && msg.msubtype == "checkcycle")
//            hsrv::threadcheck->IAAcheck(id);

        if (mod) {
            obj->makeStatusPage(*obj->program, obj->active);
            obj->makeVersionPage(*obj->program, obj->active);
        }
    }
}

bool ProgramEngine::setprogram(MMessage& m, Program* p){
    if (p == NULL) return false;
    string progdir = hsrv::homedir + "/programs";
    string prog = m.getString("program");
    if(prog=="") prog = m.getString("value");
    string currentp = hsrv::homedir + "/CURRENTP";
    string previousp = "PREVIOUSP";
    string swapped = hsrv::homedir + "/SWAPPEDOUT";
    string swapped_program;
    string active_program;
    bool found = false;
    vector<string> pl;
    if(prog == "_SWITCH") {
        active_program = FileManager::readFile(currentp);
        if(FileManager::isFile(swapped)) swapped_program = FileManager::readFile(swapped);
        else swapped_program = "";
        if(active_program != "OFF" && active_program != "off" && active_program != "Off") {
            prog = "OFF";
            FileManager::moveFile(currentp, hsrv::homedir, previousp);
            FileManager::saveFile(currentp, prog);
            FileManager::saveFile(swapped, active_program);
            p->load(prog);
            return true;
        }
        else if(active_program == "OFF" || active_program == "off" || active_program == "Off") {
            if(swapped_program !="" && swapped_program != "OFF" && swapped_program != "off" && swapped_program != "Off") prog = swapped_program;
            else prog = "MANUAL";
            FileManager::moveFile(currentp, hsrv::homedir, previousp);
            FileManager::saveFile(currentp, prog);
            FileManager::deleteFile(swapped);
            p->load(prog);
            return true;
        }
    }
    if(prog == "_ON") {
        active_program = FileManager::readFile(currentp);
        if(FileManager::isFile(swapped)) swapped_program = FileManager::readFile(swapped);
        else swapped_program = "";
        if(active_program == "OFF" || active_program == "off" || active_program == "Off") {
            if(swapped_program !="" && swapped_program != "OFF" && swapped_program != "off" && swapped_program != "Off") prog = swapped_program;
            else prog = "MANUAL";
            FileManager::moveFile(currentp, hsrv::homedir, previousp);
            FileManager::saveFile(currentp, prog);
            FileManager::deleteFile(swapped);
            p->load(prog);
            return true;
        }
    }
    if(prog!="" && FileManager::fileList(progdir, pl) >0)
        for(unsigned i=0; i<pl.size() && !found; i++)
            if(pl[i]==prog) found = true;
    if(found) {
        p->load(prog);
        FileManager::moveFile(currentp, hsrv::homedir, previousp);
        FileManager::saveFile(currentp, prog);
        FileManager::deleteFile(swapped);
    }
    return found;
}

bool ProgramEngine::program_notify(Program* p, vector<string> &al, bool change) {
    MMessage out_message("program", "prgstatus");
    vector<string>* tal;
    string status;
    bool has_changed = false;

    if (change){ 
	out_message.add("program", p->currentProgram());
        if(p->currentProgram() == "OFF" || p->currentProgram()=="off" || p->currentProgram() == "Off") {
            out_message.add("status", "OFF");
            al.clear();
            status = "OFF";
            has_changed = true;
        }
	else if(previous_program != p->currentProgram()) {
            tal = p->isActive();
	    if(tal != NULL) al = *tal;
		else al.clear();
            delete tal;
            has_changed = true;
	}
        else {
            tal = p->isActive();
            if (tal==NULL && al.size()>0) {
                has_changed = true;
                al.clear();
            }
            else if (tal && *tal != al) {
                has_changed = true;
                al = *tal;
                delete tal;
            }
        }
    }
    else {
        tal = p->isActive();
        if(tal == NULL && al.size()>0){
            al.clear();
            has_changed = true;
        }
        else if (tal!=NULL && *tal != al) {
            has_changed = true;
            al = *tal;
            delete tal;
        }
    }
    if (has_changed) {
	out_message.add("program", p->currentProgram());
	out_message.add("status", p->currentProgram());
        out_message.add("active", al);
	if(p->currentProgram()!="OFF" && al.size()>0) out_message.add("program_active", "true");
	else out_message.add("program_active", "false");
        out_message.add("time", hsrv::gettime());
        SubjectSet::notify(out_message);
	hsrv::publish("prgstatus", "program=\""+p->currentProgram()+"\" active=\""+out_message.getString("program_active")+"\"");
	previous_program = p->currentProgram();
        return true;
    }
    return false;
}

bool ProgramEngine::program_info(MMessage& m) {
    MMessage m_out;
    string pname = m.getString("program");
    m_out.mtype="program";
    m_out.msubtype = "setprogram";
    m_out.add("value", pname);
    m_out.add("source", hsrv::agentname);
    m_out.add("mode", pname);
    SubjectSet::notify(m_out);
    return true;
}


bool ProgramEngine::makeStatusPage(Program& p, vector<string>& al) {
    MMessage page;
    vector<string> service_list;
    map<string, string> program_list;
    map<string, string>::iterator it;
    page.add("active_program", p.currentProgram());
    page.add("active_actions", al);
    p.getProgramsList(program_list);
    for(it = program_list.begin();it != program_list.end();it++) {
        service_list.push_back(it->first);
    }
    page.add("available_programs", service_list);
    page.add("language", "xml");
    AgentConf::pageRegister("/programstatus.xml", page);
    page.add("language", "html");
    AgentConf::pageRegister("/programstatus", page);
    return true;
}

bool ProgramEngine::makeVersionPage(Program& p, vector<string>& al) {
    MMessage page;
    map<string, string> program_list;
    map<string, string>::iterator it;
    program_list.clear();
    p.getProgramsVersions(program_list);
    for(it = program_list.begin();it != program_list.end();it++) {
        page.add(it->first, it->second);
    }
    page.add("language", "xml");
    AgentConf::pageRegister("/programversions.xml", page);
    page.add("language", "html");
    AgentConf::pageRegister("/programversions", page);
    return true;
}

