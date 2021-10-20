#include <string>
#include <iostream>
#include <queue>
#include "rapidcsv.h"
#include <memory>
using namespace std;

class TrafficLight{
private:
    static int counter;
    
    int length; //green cycle
    std::string street;
    bool updated;
public:
    std::string color;
    std::string getColor();
    int getLength();
    void nextColor();
    std::string getStreet();
    bool isUpdated();
    bool canGo();
    TrafficLight() = default;
    TrafficLight(std::string street_in){
        int mod = counter%25;
        if(mod == 0) length = 90;
        else if(mod < 15) length = 60;
        else length = 30;
        counter++;
        color = "r";
        street = street_in;
    };

};
int TrafficLight::counter = 0;

class TrafficController{
public:
    std::vector<TrafficLight> street_lights;
    std::vector<std::string> street_names;
    int getCurrTime();
    void setCurrTime(int time_in);
    void setPrevTime(int time_in);
    std::string getCNN();
    std::string getCoordinate();
    void updateLights();
    bool allRed();
    TrafficController(){
        CNN = "";
        coordinate = "";
        prev_green = -1;
        curr_time = -1;
    }
    TrafficController(int time_in, std::string cnn_in, std::string point,
                        std::string st1,std::string st2,
                        std::string st3 = "",std::string st4 = ""){
        CNN = cnn_in;
        TrafficLight first(st1);
        street_lights.push_back(first);
        street_names.push_back(st1);
        street_names.push_back(st2);
        TrafficLight second(st2);
        street_lights.push_back(second);
        if(!st3.empty()){
            TrafficLight third(st3);
            street_lights.push_back(third);
            street_names.push_back(st3);
        }
        if(!st4.empty()){
            TrafficLight fourth(st4);
            street_lights.push_back(fourth);
            street_names.push_back(st4);
        }
        curr_time = time_in;
        prev_time = curr_time;
        prev_green = -1;
        coordinate = point.substr(point.find('(')+1,point.find(')')-point.find('(')-1);
        std::replace(coordinate.begin(),coordinate.end(),' ',',');
    };
    
    ///////////////////////////////////////////////////////////
    //https://neutrofoton.github.io/blog/2016/12/29/c-plus-plus-priority-queue-with-comparator/
    friend bool operator< (const TrafficController& lhs, const TrafficController& rhs);
    friend bool operator> (const TrafficController& lhs, const TrafficController& rhs);
    //////////////////////////////////////////////////////////
private:
    std::string CNN; 
    std::string coordinate;
    int prev_green;
    int curr_time; 
    int prev_time; 
    
};

/////////////////////////////////////////////////////////
// portion taken from: https://www.cplusplus.com/articles/1UqpX9L8/

class splitstring : public std::string {
    vector<std::string> flds;
public:
    splitstring(std::string s) : std::string(s) { };
    vector<std::string>& split(char delim, int rep=0);
};
/////////////////////////////////////////////////////////

class Street{
public:
    double compute_time(const double distance, const uint32_t speed);
    void increase_car_count();
    void decrement_car_count();
    bool hasSpace();
    int getCurrTime();
    double getDriveTime();
    std::string getStreetName();
    bool isDestination();
    void HeavyTraffic();
    bool canSync();
    void synchronize();
    std::shared_ptr<TrafficController> tc_begin;
    std::shared_ptr<TrafficController> tc_end;

    Street(std::string name, bool light_traffic, bool street_unknown, std::shared_ptr<TrafficController> tc1, std::shared_ptr<TrafficController> tc2, bool syncable=false, double dist=0.0){
        street_name = name;
        distance = dist;
        unknown_street = street_unknown;
        if(!name.compare("DESTINATION")){
            isDest = true;
            drive_time = 0.0;
        } 
        else{
            isDest = false;
            uint32_t speed = light_traffic ? 30 : 3;
            capacity = light_traffic ? 20 : 2;
            drive_time = std::ceil((dist * 3600) / speed);
        }
        tc_begin = tc1;
        tc_end = tc2;
        can_sync = syncable && (!isDest);
    };
    Street() = default;

private:
    std::string street_name;
    double distance;
    bool isDest;
    double drive_time;
    int capacity;
    int car_count = 0;
    bool unknown_street;
    bool can_sync;


};

class Car{
public:
    
    void update_car();
    int getInitTime();
    bool reachedDest() const;
    int getTotalTime() const;
    void HeavyTraffic();
    std::queue<Street> street_queue;
    std::queue<Street> init_street_queue;
    Car(int time_in, std::queue<Street> st_q, bool v = false){
        init_time = time_in;
        street_queue = st_q;
        init_street_queue = st_q;
        current_street = st_q.front();
        st_q.pop();
        accum_time = 0;
        verbose = v;
    }
private:
    
    Street current_street;
    int init_time; 
    double accum_time;
    bool unknown_street = false;
    bool reached_dest = false;
    bool verbose;
};