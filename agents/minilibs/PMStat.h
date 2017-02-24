#ifndef PMSTAT_H
#define PMSTAT_H

#define DAILY 0
#define MONTHLY 1
#define ANNUAL 2


using namespace std;

class PMStat {
  public:
    PMStat() { energy = 0; time = "0"; value=0;};
    PMStat(float e, std::string t) { 
        energy = e; time=t; value=0; 
    };
    PMStat(float e, std::string t, float v) { 
        energy = e; time=t; value=v; 
    };
    
    string getJsonString(int mode) {
        stringstream setdata;
        setdata<<"{\"time\":\""<<getTime();
        setdata<<"\",\"energy\":\"";
        setdata<<getEnergy();
        setdata<<"\",\"value\":\"";
        setdata<<getValue();
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

    bool update(float e, string t, float v) {
       if(e>energy) {
	  energy = e;
	  time = t;
	  value = v;
       }
       return true;
    };

    float getValue() { return value; };
    float getEnergy() { return energy; };
    std::string getTime() { return time; };

  private:
    float value;
    float energy;
    std::string time;
};

#endif
