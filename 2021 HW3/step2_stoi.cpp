#include <string>

int main(){
    std::string input = "3";

    for(int i = 0; i<1000000;i++){
        auto x = std::stoi(input);
    }
    return 0;
}