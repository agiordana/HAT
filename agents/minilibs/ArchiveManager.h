
#ifndef ARCHIVEMANAGER_H
#define ARCHIVEMANAGER_H

#include <boost/filesystem.hpp>
#include <fstream>
#include <iterator>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include "PMStat.h"

using namespace std;

class MParams;

class RootDescriptor {
public:
  int temporalWindow;
  std::string prefix;
  std::vector<std::string> visibility;
  RootDescriptor(){ visibility = std::vector<std::string>(); temporalWindow = 0;};
};



class ArchiveManager: public MParams {
public:  
  static boost::mutex mutexLock;
  ArchiveManager() {};

  ArchiveManager(const std::string name): MParams(name) { archiveHome = hsrv::homedir; archiveName=name;};
/*
 * MParams::load() e installazione del fs dell'archivio
 * Salvataggio in accessInfo delle specifiche relative all'accessibilità
 * delle directory
 */
  bool init(); 
  
 /*
  * Processa richieste POST
  * Se m contiene un url invalido ritorna errore
  * Se m contiene un url relativo ad una istanza esistente fa l'append
  * del dato
  * Se m contiene un url corretto ma relativo ad una istanza non esistente
  * crea l'istanza corrispondente 
 */
  MMessage postExec(MMessage& m);  
 
 /*
  * Processa richieste PUT
  * Se m contiene un url invalido ritorna errore
  * Se m contiene un url relativo ad una istanza esistente sovrascrive
  * il dato
  * Se m contiene un url corretto ma relativo ad una istanza non esistente
  * crea l'istanza corrispondente 
 */ 
  MMessage putExec(MMessage& m); 
  
  MMessage putConf(MMessage& m); 

  MMessage putLog(MMessage& m);

  MMessage postLog(MMessage& m);  

  MMessage deleteExec(MMessage& m);
/*
 * Pulizia delle istanze outdated
 * Esplora le foglie dell'archivio e sposta (rinomina/cancella) le istanze
 * che fanno riferimento a date piu vecchie di archivePeriod e messagePeriod
 */
    void purge(bool video=false);
  
    string getFirst(MMessage req, std::string& filetype);
    
    string getLast(MMessage req, std::string& filetype);
    
    string getAll(MMessage req);
    
    string getFile(MMessage req);

    string getDir(MMessage req);
    
    string getSubSet(MMessage req);  

    string getSubLog(MMessage req);  

    string getList(MMessage req);

    string getPMLog(MMessage req);
    
    string getPMDaily(MMessage req);

    string getPMMonthly(MMessage req);

    string getPMMonthlySummary(MMessage req);
   
    string getPMAnnual(MMessage req);
    
    string getEnergyData(MMessage req);

    static bool scanLine(string&, float&, float&, float&, string&, float&);
  
  bool addResource(std::string pathname, std::string src="", bool overwrite=true);
  
  bool updateResource(std::string resPath, std::string s);

  bool deleteResource(std::string url);  

  vector<string> getSubPaths(string src, int depth);
  void exploreArchive(string src, vector<string>& list, bool noDir=false);
  void getSubPaths(string src, vector<string>& list);
  
  static string getPMDaySummary(std::string datafile);
  static string getCurrentDay(string &day, string &month, string &year);
  static string getParentDirectory(string filename);
  static int getDateDifference(string date1, string date2, char separator='_');
  static bool isDate(string date, char separator='_');
  static bool evalDescriptor(string start, string end, string descr);  

private:
    std::string archiveName;
    std::string archiveHome;
    std::vector<std::string> methods;
    map<std::string, RootDescriptor> accessInfo; //visibilità directories

  /*
  * Procedura comune a tutti le chiamate di tipo GET (first, last, all, file).
  * Verifica la correttezza dell'uri ricevuto e ritorna il pathname assoluto della
  * risorsa corrispondente
  */
    std::string getExec(MMessage& m);  
     
    std::string getLatestData(std::string dir);

    bool checkPermission(std::vector<std::string> url, int mode);
    
    bool fullAccess(string archiveName) {
        if (accessInfo.count(archiveName) == 0) false;
        else return (accessInfo.at(archiveName).visibility.front() == "*");
    }

    vector<string> getSubDir(string archiveName) {
        if (accessInfo.count(archiveName) == 0) return vector<string>();
        else return accessInfo.at(archiveName).visibility;
    };

    int getWindow(string archiveName) {
        if (accessInfo.count(archiveName) == 0) return 0;
        else return accessInfo.at(archiveName).temporalWindow;
    };
    
    bool isInstance(std::string pathname);
    
    bool isValidMethod(string method);
    
    string getRelativePath(string path);
            
    string extractURL(string reqURL);
    
    void restify(MMessage &m, bool conf=false);
    
    std::string getRootPath(std::string uri);

    void clear(std::string filename);
  
    static bool getTimeRangeFromJson(std::string body, std::string& start, std::string& end); 
    static bool getCategoriesJson(std::string body, NameList& category); 
    static string normalizeDateTime(std::string t);
    static std::vector<int> MDAYS;
    static const int READ = 0;
    static const int WRITE = 1;
};


#endif /* ARCHIVEMANAGER_H */
