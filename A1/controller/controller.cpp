#include "controller.hpp"
#include <string>
#include <iostream>
#include <queue>


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
    cout << t <<endl;

    // read the CNN, TBC, STREET1/2/3/4 and shape values from csv    
    ifstream in("Traffic_Signals_SF.csv");
    std::string line;
    vector<string> row;
    int intersection_count = 0;
    bool start = false;
    std::vector<TrafficController> tcVec;
    while(getline(in,line)){
        if(start && !line.empty()){
            intersection_count++;
            splitstring split_row(line);
            row = split_row.split(',',1);
            std::string TBC = row[12];
            if(!TBC.compare("GPS")){
                TrafficController tc(-1,row[0],row[34],row[3],row[4],row[6],row[8]);
                tcVec.push_back(tc);
            }
        }else start = true; //skip the header
    }
    in.close();
    /////////////////////////////////////////////////////////////////
    //https://neutrofoton.github.io/blog/2016/12/29/c-plus-plus-priority-queue-with-comparator/
    priority_queue<TrafficController,vector<TrafficController>, CompareTC> pq;
    for(auto i = tcVec.cbegin(); i!=tcVec.cend();i++){
        pq.push(*i);
    }
    // /////////////////////////////////////////////////////////////////
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
    output_file.close();


    return 0;
}