#include <string>
#include <iostream>
#include "rapidcsv.h"
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
    int getCurrTime();
    void setCurrTime(int time_in);
    void setPrevTime(int time_in);
    std::string getCNN();
    std::string getCoordinate();
    void updateLights();
    bool allRed();

    TrafficController(int time_in, std::string cnn_in, std::string point,
                        std::string st1,std::string st2,
                        std::string st3 = "",std::string st4 = ""){
        CNN = cnn_in;
        TrafficLight first(st1);
        street_lights.push_back(first);
        TrafficLight second(st2);
        street_lights.push_back(second);
        if(!st3.empty()){
            TrafficLight third(st3);
            street_lights.push_back(third);
        }
        if(!st4.empty()){
            TrafficLight fourth(st4);
            street_lights.push_back(fourth);
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
    Street(std::string name, double dist=0.0){
        street_name = name;
        distance = dist;
    }
private:
    std::string street_name;
    std::string traffic;
    double distance;
    bool isDest;
    pair<double,double> start;
    pair<double,double> end;
};