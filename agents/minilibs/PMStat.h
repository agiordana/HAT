#ifndef PMSTAT_H
#define PMSTAT_H

#define DAILY 0
#define MONTHLY 1
#define ANNUAL 2


using namespace std;

class PMStat {
  public:
    PMStat() { energy = 0; time = "0";};
    PMStat(float e, std::string t) { 
        energy = e; time=t; 
    };
    
    string getJsonString(int mode) {
        stringstream setdata;
        setdata<<"{\"time\":\""<<getTime();
        setdata<<"\",\"energy\":\"";
        setdata<<getEnergy();
        setdata<<"\"}";

        return setdata.str();
    };

    bool update(float e, string t) {
       if(e>energy) {
	  energy = e;
	  time = t;
       }
       return true;
    };

    float getEnergy() { return energy; };
    std::string getTime() { return time; };

  private:
    float energy;
    std::string time;
};

#endif
