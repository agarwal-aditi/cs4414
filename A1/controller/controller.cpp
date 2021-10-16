#include "controller.hpp"
#include <cstdio>
#include <cmath>
#include <string>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <utility>


double compute_distance(const std::pair<double, double>& point1, std::pair<double, double>& point2)
{
    const std::pair<double, double> DEGREE_TO_MILES(54.74, 68.97);
    const double long_miles = (point2.first - point1.first) * DEGREE_TO_MILES.first;
    const double lat_miles = (point2.second - point1.second) * DEGREE_TO_MILES.second;
    return std::sqrt(long_miles * long_miles + lat_miles * lat_miles);
}


std::string TrafficLight::getColor() {
    return color;
}

std::string TrafficLight::getStreet(){
    return street;
}

int TrafficLight::getLength(){
    return length;
}

void TrafficLight::nextColor() {
    if(!color.compare("r")) color = "g";
    else if(!color.compare("y")) color = "r";
    else if(!color.compare("g"))color = "y";
}

bool TrafficLight::isUpdated(){
    return updated;
}

int TrafficController::getCurrTime(){
    return curr_time;
}

void TrafficController::setCurrTime(int time_in){
    curr_time = time_in;
}

void TrafficController::setPrevTime(int time_in){
    prev_time = time_in;
}

std::string TrafficController::getCNN(){
    return CNN;
}

std::string TrafficController::getCoordinate(){
    return coordinate;
}


void TrafficController::updateLights(){
    int time_diff = curr_time - prev_time; 
    for(int i = 0;i<street_lights.size();i++){
        std::string curr_color = street_lights[i].getColor();
        if((!(curr_color.compare("g"))) && (street_lights[i].getLength() >= time_diff)) street_lights[i].nextColor();
        if(!(curr_color.compare("y"))) street_lights[i].nextColor();
    }
    if(allRed()){
            if(prev_green+1 == street_lights.size()){
                street_lights[0].nextColor();
                prev_green = 0;
            }else{
                prev_green++;
                street_lights[prev_green].nextColor();
            }
            prev_time = curr_time;
    }
}


bool TrafficController::allRed(){
    for(int i = 0; i<street_lights.size();i++){
        if((street_lights[i].getColor()).compare("r")) return false;
    }
    return true;
}


double Street::compute_time(const double distance, const uint32_t speed) {
    return std::ceil((distance * 3600) / speed);
}


void Street::increase_car_count() {
    car_count++;
}


void Street::decrement_car_count() {
    car_count--;
}


bool Street::hasSpace() {
    return (capacity-car_count > 0);
}


/////////////////////////////////////////////////////////
// portion taken from: https://www.cplusplus.com/articles/1UqpX9L8/

// split: receives a char delimiter; returns a vector of strings
// By default ignores repeated delimiters, unless argument rep == 1.
vector<std::string>& splitstring::split(char delim, int rep) {
    if (!flds.empty()) flds.clear();  // empty vector if necessary
    string work = data();
    string buf = "";
    int i = 0;
    while (i < work.length()) {
        if (work[i] != delim)
            buf += work[i];
        else if (rep == 1) {
            flds.push_back(buf);
            buf = "";
        } else if (buf.length() > 0) {
            flds.push_back(buf);
            buf = "";
        }
        i++;
    }
    if (!buf.empty())
        flds.push_back(buf);
    return flds;
}
////////////////////////////////////////////////////////////

void printIconKML(ofstream &kml_file,std::string id){
    std::string file_path = "./files/"+id+".png";
    kml_file <<"  <Style id=\""+id+"\">"<<endl;
    kml_file<<"    <IconStyle id=\""+id+"\">"<<endl;
    kml_file<<"      <Icon>"<<endl;
    kml_file<<"        <href>"+file_path+"</href>"<<endl;
    kml_file<<"        <scale>1.0</scale>"<<endl;
    kml_file<<"      </Icon>"<<endl;
    kml_file<<"    </IconStyle>"<<endl;
    kml_file<<"  </Style>"<<endl;
}

void printIntersectionKML(ofstream &kml_file, TrafficController &tc){
    kml_file<<"  <Placemark>"<<endl;
    kml_file<<"    <name>"+tc.getCNN()+"</name>"<<endl;
    std::string street_names = "";
    std::string light_colors = std::to_string(tc.street_lights.size());
    for(int i = 0; i<tc.street_lights.size();i++){
        street_names += tc.street_lights[i].getStreet();
        if(i!=tc.street_lights.size()-1) street_names += " and ";
        light_colors += tc.street_lights[i].getColor();
    }
    kml_file<<"    <description>"+street_names+"</description>"<<endl;
    kml_file<<"    <styleUrl>#i"+light_colors+"</styleUrl>"<<endl;
    kml_file<<"    <Point>"<<endl;
    kml_file<<"      <coordinates>"+tc.getCoordinate()+"</coordinates>"<<endl;
    kml_file<<"    </Point>"<<endl;
    kml_file<<"  </Placemark>"<<endl;
}

std::string csvColor(std::string color_first_letter){
    if(!color_first_letter.compare("r")) return "RED";
    else if(!color_first_letter.compare("y")) return "YELLOW";
    else if(!color_first_letter.compare("g")) return "GREEN";
    else return "";
}
int main(int argc, char* argv[]){
    std::cout.precision(20);
    ///////////////////////////////////////////////////////////////
//https://neutrofoton.github.io/blog/2016/12/29/c-plus-plus-priority-queue-with-comparator/
struct CompareTC{
    bool operator()( TrafficController& lhs,  TrafficController& rhs){
        return lhs.getCurrTime() < rhs.getCurrTime();
    }
};
///////////////////////////////////////////////////////////////
    if(argc<2){
        cout << "should be: ./controller -t=n" <<endl;
        return 1;
    }
    
    string flag = argv[1];
    if(flag.length()<4){
        cout << "invalid entry \n" << endl;
        return 1;
    }
    int t = std::stoi(flag.substr(3));
    

    // read the CNN, TBC, STREET1/2/3/4 and shape values from csv    
    ifstream in("Traffic_Signals_SF.csv");
    std::string line;
    vector<string> row;
    std::vector<std::vector<string>> traffic_signals_arr;
    unordered_map<std::string,TrafficController> tcMap;
    int intersection_count = 0;
    bool start = false;
    std::vector<TrafficController> tcVec;
    while(getline(in,line)){
        splitstring split_row(line);
        row = split_row.split(',',1);
        traffic_signals_arr.push_back(row);
        if(start && !line.empty()){
            intersection_count++;
            std::string TBC = row[12];
            if(!TBC.compare("GPS")){
                TrafficController tc(-1,row[0],row[34],row[3],row[4],row[6],row[8]);
                tcVec.push_back(tc);
                std::pair<std::string,TrafficController> tc_pair (row[0],tc);
                tcMap.insert(tc_pair);
            }
        }else start = true; //skip the header
    }
    
    in.close();


    
    ifstream in2("Sync_And_Cars.csv");
    std::vector<std::vector<string>> cars_arr;
    int idx;
    std::string line2;
    vector<std::string> row2;
    while(getline(in2,line2)){
        splitstring split_row2(line2);
        row2 = split_row2.split(',',1);
        cars_arr.push_back(row2);
    }
    // cout << cars_arr[3][12].size() << endl;
    
    in2.close();

    //create streets from the array
    int i, j;
    std::unordered_map<uint64_t,Street> streets;
    uint32_t first_CNN;
    uint32_t second_CNN;
    uint64_t name;
    std::string st_name;
    double distance;
    std::string coord_as_string1;
    std::string coord_as_string2;
    double first1;
    double second1;
    double first2;
    double second2;
    int end_of_line = cars_arr[0].size();
    bool ignore = false;
    std::pair<double,double> pair1, pair2;
    std::vector<std::string> st_names1;
    std::vector<std::string> st_names2;
    std::string street_name;
    bool street_name_unknown = false;
    bool found_street = false;
    
    for(i = 2; i<cars_arr.size()-1;i++){
        for(j=0; j<end_of_line;j++){
            if(!cars_arr[i][j].empty() && cars_arr[i][j].compare("\n") && cars_arr[i][j].size() > 0){
                
                first_CNN = std::stoi(cars_arr[i][j]);
                
                if(!cars_arr[i+1][j].empty() && cars_arr[i+1][j].compare("\n") && cars_arr[i+1][j].size() > 0){
                
                    second_CNN = std::stoi(cars_arr[i+1][j]);
                
                    name = first_CNN;
                    name = name << 32;
                    name = name | second_CNN;

                    if(second_CNN == 0) {
                        street_name = "DESTINATION";
                        distance = 0;
                        if(j == 14) end_of_line--;
                    }
                    else{
                        if(cars_arr[i+1][j].size() > 1){
                            st_name = std::to_string(name);
                            auto tc_find = tcMap.find(cars_arr[i][j]);
                            if(tc_find != tcMap.end()){
                                coord_as_string1 = (tc_find->second).getCoordinate();
                                st_names1 = (tc_find->second).street_names;
                            }else ignore = true;

                            auto tc_find2 = tcMap.find(cars_arr[i+1][j]);
                            if(tc_find2 != tcMap.end()){
                                coord_as_string2 = (tc_find2->second).getCoordinate();
                                st_names2 = (tc_find->second).street_names;
                            }else ignore = true;

                            if(!ignore){
                                std::string coord_as_string1_1 = coord_as_string1.substr(0,coord_as_string1.find(','));
                                first1 = std::stod(coord_as_string1_1);
                                
                                std::string coord_as_string1_2 = coord_as_string1.substr(coord_as_string1.find(',')+1);
                                second1 = std::stod(coord_as_string1_2);
                                std::pair<double,double> pair1 (first1,second1);

                                std::string coord_as_string2_1 = coord_as_string2.substr(0,coord_as_string2.find(','));
                                first2 = std::stod(coord_as_string2_1);
                                
                                std::string coord_as_string2_2 = coord_as_string2.substr(coord_as_string2.find(',')+1);
                                second2 = std::stod(coord_as_string2_2);

                                std::pair<double,double> pair2 (first2,second2);
                                
                                distance = compute_distance(pair1,pair2);
                                cout << compute_distance(pair1,pair2) << endl;


                                for(auto i : st_names1){
                                    if(std::find(st_names2.begin(),st_names2.end(),i) != st_names2.end()){
                                        street_name = i + " FROM " + std::to_string(first_CNN) + "to " + std::to_string(second_CNN);
                                        found_street = true;
                                    }
                                }
                                if(!found_street){
                                    street_name = "UNKNOWN FROM " + std::to_string(first_CNN) + "to " + std::to_string(second_CNN);
                                    street_name_unknown = true;
                                }
                            }
                        } else ignore = true;
                    }
                    if(!ignore){
                        auto it = streets.find(name);
                        if(it == streets.end()){
                            Street st(street_name,true,street_name_known, tc_find, tc_find2, distance);
                            std::pair<uint64_t,Street> street_pair (name,st);
                            streets.insert(street_pair);
                        }
                    } else ignore = false;
                }
            } 
        }
    }

    /////////////////////////////////////////////////////////////////
    //https://neutrofoton.github.io/blog/2016/12/29/c-plus-plus-priority-queue-with-comparator/
    priority_queue<TrafficController,vector<TrafficController>, CompareTC> pq;
    for(auto i = tcVec.cbegin(); i!=tcVec.cend();i++){
        pq.push(*i);
    }
    ////////////////////////////////////////////////////////////////////
    tcVec.clear();
    for(int i = 0; i<=t; i+=10){
        tcVec.clear();
        while(!pq.empty()){
            auto temp = pq.top();
            temp.setCurrTime(i);
            temp.updateLights();
            tcVec.push_back(temp);
            pq.pop();
        }
        for(auto i = tcVec.cbegin(); i!=tcVec.cend();i++){
            pq.push(*i);
        }
    }
    ofstream kml_file("myfile.kml");

    kml_file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
    kml_file << "<kml xmlns=\"http://www.opengis.net/kml/2.2\">" << endl;
    kml_file << "<Document>" << endl;
    std::array<string,3> colors = {"r","y","g"};
    for(auto i = 0; i<colors.size();i++){
        for(auto j = 0; j<colors.size();j++){
            //2 streets
            std::string id = "i2"+colors[i]+colors[j];
            printIconKML(kml_file,id);
            for(auto k = 0; k<colors.size();k++){
                //3 streets
                std::string id = "i3"+colors[i]+colors[j]+colors[k];
                printIconKML(kml_file,id);
                for(auto m = 0; m<colors.size();m++){
                    //4 streets
                    std::string id = "i4"+colors[i]+colors[j]+colors[k]+colors[m];
                    printIconKML(kml_file,id);
                }
            }
        }
    }
    

    ofstream output_file("myfile.csv");
    output_file << "CNN,STREET,COLOR" << endl;
    for(int i = 0; i<pq.size();i++){
        auto temp = pq.top();
        string CNN = temp.getCNN();
        for(int j = 0; j<temp.street_lights.size();j++){
            output_file << CNN +","+ temp.street_lights[j].getStreet() +"," +csvColor(temp.street_lights[j].getColor()) <<endl;
        }
        printIntersectionKML(kml_file,temp);
        pq.pop();
    }
    kml_file<<"</Document>"<<endl;
    kml_file<<"</kml>"<<endl;
    kml_file.close();
    output_file.close();


    return 0;
}