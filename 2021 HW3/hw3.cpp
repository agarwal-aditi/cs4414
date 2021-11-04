// (c) Ken Birman.  This code is provided only for use in CS4144, fall 2021, homework 3
// The author specifically prohibits posting of this code on public sites such as
// Chegg, Course Hero, etc.

#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <queue>
#include <string>
#include <vector>

#include "AlertEvent.hpp"
#include "Car.hpp"
#include "Street.hpp"
#include "TrafficIntersection.hpp"
#include "TrafficLight.hpp"
#include "constants.hpp"
#include "csv.hpp"

csv TL, SC;
int verbose = 0;
extern std::queue<std::pair<int, AlertEvent*>> myPriorityQueue;
extern std::map<int64_t, std::shared_ptr<Street>> streets;
extern std::vector<std::shared_ptr<AlertEvent>> cars;
const int SYNC_COLS = 2;

csv read_csv(const std::string& filename) {
    std::string line;
    csv the_csv;
    std::ifstream input_file(filename);
    if(!input_file.is_open()) {
        std::cerr << "Could not open the file - '" << filename << "'" << std::endl;
        return the_csv;
    }

    int nfields = -1, line_count = 0;

    while(std::getline(input_file, line)) {
        size_t idx = line.find('\r');
        if(idx != std::string::npos)
            line = line.substr(0, idx);
        ++line_count;
        if(verbose & V_PARSE)
            std::cout << "Parsing line [" << std::setw(4) << line_count << "] = [" << line << "]" << std::endl;
        std::vector<std::string> parsed;
        while((idx = line.find(',')) != std::string::npos) {
            parsed.push_back(line.substr(0, idx));
            line = line.substr(idx + 1);
        }
        parsed.push_back(line);
        the_csv.push_back(parsed);
        if(nfields == -1)
            nfields = parsed.size();
        else if(nfields != (int)parsed.size())
            std::cerr << "Error: The first line had " << nfields << " fields, but line " << line_count << " had " << parsed.size() << " fields!" << std::endl;
    }

    input_file.close();
    return the_csv;
}

std::string getStreet(csv TL, const std::vector<int>& col, const std::string& cnn1, const std::string& cnn2) {
  if(cnn1 == "0") {
        std::cerr << "ERROR! Using 0 as the origin for a street" << std::endl;
        return "ERROR";
    }
  if(cnn2 == "0")
        return "DESTINATION";
    int row1 = -1, row2 = -1;
    for(uint n = 1; (row1 == -1 || row2 == -1) && n < TL.size(); n++) {
        if(cnn1 == TL[n][0])
            row1 = n;
        if(cnn2 == TL[n][0])
            row2 = n;
    }
    if(row1 == -1 || row2 == -1) {
        return "UNKNOWN CNN";
    }
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
            if(TL[row1][col[i]].size() > 0 && TL[row1][col[i]] == TL[row2][col[j]])
                return (TL[row1][col[i]]);
    return "UNKNOWN";
}

int main(const int argc, const char** argv) {
    int run_length = -1;
    for(int arg = 1; arg < argc; arg++) {
        if(argv[arg][0] != '-') {
            std::cerr << "Arguments must start with -" << std::endl;
            return EXIT_FAILURE;
        }
        switch(argv[arg][1]) {
            case 't': {
                run_length = atoi(argv[arg] + 3);
                break;
            }
            case 'v': {
                int len = strlen(argv[arg]);
                if(len == 2)
                    verbose = -1;  // -v sets all bits if "unqualified"
                else
                    for(int c = 2; c < len; c++)
                        switch(argv[arg][c]) {
                            case 'c':
                                verbose |= V_CARS;
                                break;
                            case 'l':
                                verbose |= V_LIGHTS;
                                break;
                            case 'i':
                                verbose |= V_INTERSECTIONS;
                                break;
                            case 's':
                                verbose |= V_STREETS;
                                break;
                            case 'p':
                                verbose |= V_PARSE;
                                break;
                            case 'a':
                                verbose |= V_ALERTS;
                                break;
                            case 'y':
                                verbose |= V_SYNC;
                                break;
                        }
                break;
            }
            default: {
                std::cerr << "Usage: hw3 -t=# [-v]" << std::endl;
                return EXIT_FAILURE;
            }
        }
    }
    if(run_length == -1) {
        std::cerr << "Usage: hw3 -t=# [-v]" << std::endl;
        return EXIT_FAILURE;
    }
    if(verbose != 0)
        std::cout << "Starting a run of length " << run_length << std::endl;
    TL = read_csv("Traffic_Signals_SF.csv");
    SC = read_csv("Sync_And_Cars.csv");
    std::vector<int> streetColumnIndexes(4);  // Col 0 is never used for STREET
    for(uint n = 0; n < TL[0].size(); n++) {
        // Figure out which columns have street names in them
      if(TL[0][n] == "STREET1")
            streetColumnIndexes[0] = n;
      else if(TL[0][n] == "STREET2")
            streetColumnIndexes[1] = n;
      else if(TL[0][n] == "STREET3")
            streetColumnIndexes[2] = n;
      else if(TL[0][n] == "STREET4")
            streetColumnIndexes[3] = n;
    }
    for(auto& ts : TL) {
      if(ts[0] == "CNN")
            continue;
        auto lights = std::make_shared<std::vector<std::shared_ptr<TrafficLight>>>();
        for(int i = 0; i < 4 && streetColumnIndexes[i] != 0; i++)
            if(ts[streetColumnIndexes[i]].size() != 0)
                lights->push_back(std::make_shared<TrafficLight>(stoi(ts[0]), ts[streetColumnIndexes[i]]));
        auto ti = new TrafficIntersection(ts, lights);
        intersections.emplace(stoi(ts[0]), std::shared_ptr<AlertEvent>(static_cast<AlertEvent*>(ti)));
    }

    int64_t total_runtime[HEAVY + 1][SYNC + 1];
    for(int traffic = LIGHT; traffic <= HEAVY; traffic++) {
        for(int sync = UNSYNC; sync <= SYNC; sync++) {
            if(verbose != 0)
                std::cout << "Traffic=" << traffic << ", Sync=" << (sync == SYNC) << std::endl;
            for(uint c = 0; c < SC[0].size(); c++)
                for(uint n = SKIPROWS; n < SC.size() - 1; n++) {
                    // Only creates a Street object for streets actually used by our cars
                    if(verbose & V_STREETS)
                        std::cout << "[row=" << n << ", col=" << c << "] ";
                    int from = stoi(SC[n][c]), to = stoi(SC[n + 1][c]);
                    long key = ((long)from << 32) | (long)to;
                    std::string name = getStreet(TL, streetColumnIndexes, SC[n][c], SC[n + 1][c]);
                    bool done = SC[n + 1][c] == "0";
                    std::shared_ptr<Street> st;
                    if((st = streets[key]) == nullptr) {
                        if(verbose & V_STREETS)
                            std::cout << "new Street: " << name << ", from " << from << " to " << to << ", synchronized=" << (sync == SYNC && c < SYNC_COLS) << std::endl;
                        st = std::make_shared<Street>(traffic == LIGHT ? 20 : (traffic == MEDIUM ? 5 : 2), from, to, name, done, sync == SYNC && c < SYNC_COLS);
                        streets[key] = st;
                    }
                    if(sync == SYNC && c < SYNC_COLS) {
                        if(verbose & V_SYNC)
                            std::cout << "Synchronizing " << name << " from " << from << " to " << to << std::endl;
                        for(auto& ipae : intersections) {
                            auto ti = static_cast<TrafficIntersection*>(ipae.second.get());
                            auto lights = ti->myLights;
                            for(auto& tl : (*lights))
			      if(tl->myStreet == name && (tl->myCNN == from || tl->myCNN == to)) {
                                    tl->isSynchronized = true;
                                    if(verbose & V_SYNC)
                                        std::cout << "... found a matching traffic light for " << tl->myCNN << " (" << tl->myStreet << ")" << std::endl;
                                }
                        }
                    }
                    if(to == 0)
                        break;
                }
            if(verbose & (V_INTERSECTIONS | V_SYNC))
                std::cout << "Next, resetting intersections" << std::endl;
            // Initialize intersections
            int k = 0;
            TrafficIntersection::reset();
            if(sync == SYNC) {
                if(verbose & (V_INTERSECTIONS | V_SYNC))
                    std::cout << "Next, synchronizing intersections" << std::endl;
                // Initialize the synchronized intersections (columns A and B)
                for(int c = 0; c <= 1; c++)
		  for(uint n = SKIPROWS; n < SC.size() - 1 && SC[n][c] == "0"; n++) {
                        auto itae = intersections[stoi(SC[n][c])];
                        auto it = static_cast<TrafficIntersection*>(itae.get());
                        int beGreenAt = ((n - 2) * 15) % ((SYNCSTAYSGREEN + YELLOWDURATION) + (it->myLights->size() - 1) * 40);
                        std::string streetName;
                        if(n != SKIPROWS)
                            streetName = getStreet(TL, streetColumnIndexes, SC[n - 1][c], SC[n + 1][c]);
                        else
                            streetName = std::string("UNKNOWN");
                        it->setTimer(itae, SYNCSTAYSGREEN, beGreenAt, streetName, true);
                    }
            }
            // Now initialize all remaining intersections
            if(verbose & (V_INTERSECTIONS | V_SYNC))
                std::cout << "Initialize remaining intersections" << std::endl;
            for(auto& tiae : intersections) {
                auto ti = static_cast<TrafficIntersection*>(tiae.second.get());
                if(ti->isInitialized)
                    continue;
                if(k % 25 == 0)
                    ti->setTimer(tiae.second, 90, 0, "INIT-STREET1-90", false);
                else if(k % 25 < 15)
                    ti->setTimer(tiae.second, 60, 0, "INIT-STREET1-60", false);
                else
                    ti->setTimer(tiae.second, 30, 0, "INIT-STREET1-30", false);
                ++k;
            }
            if(verbose & V_CARS)
                std::cout << "Next, creating waves of cars" << std::endl;
            // Now create waves of cars, spaced one second out per wave
            int max_cars = SC[0].size();
            for(int sec = 0; sec < run_length; sec++)
                for(int car_n = 0; car_n < max_cars; car_n++) {
                    auto car = new Car(car_n, sec, traffic);
                    auto sc = std::shared_ptr<AlertEvent>(static_cast<AlertEvent*>(car));
                    cars.push_back(sc);
                    car->setTimer(sc, sec);
                }

            // Run simulation as long as there are still some active cars
            if(verbose & V_CARS)
                std::cout << "Simulating " << Car::active_cars << "cars" << std::endl;

            while(Car::active_cars > 0)
                AlertEvent::runAll();

            if(verbose & (V_CARS | V_STREETS))
                std::cout << "Delete car and street objects" << std::endl;

            cars.clear();
            streets.clear();

            if(verbose)
                std::cout << "Run time was " << Car::total_driving_time << std::endl;
            total_runtime[traffic][sync] = Car::total_driving_time;
            Car::reset();
            AlertEvent::resetQueue();
        }
    }
    // Print output
    std::cout << "		Without synchronization			With synchronization" << std::endl;
    std::cout << "Light traffic		" << std::setw(9) << total_runtime[LIGHT][UNSYNC] << "				" << std::setw(9) << total_runtime[LIGHT][SYNC] << std::endl;
    std::cout << "Medium traffic		" << std::setw(9) << total_runtime[MEDIUM][UNSYNC] << "				" << std::setw(9) << total_runtime[MEDIUM][SYNC] << std::endl;
    std::cout << "Heavy traffic		" << std::setw(9) << total_runtime[HEAVY][UNSYNC] << "				" << std::setw(9) << total_runtime[HEAVY][SYNC] << std::endl;
    return EXIT_SUCCESS;
}
