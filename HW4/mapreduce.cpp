#include <iostream>
#include "ngram_counter.hpp"

int main(int argc, char* argv[]) {
  if (argc < 4) {
      std::cout << "Usage: " << argv[0] << " <dir> -t=<num-threads> -n=<n>" << std::endl;
      return 1;
  }
  int args[2];
  for(int i = 2; i<4;i++){
    std::string flag = argv[i];
    if(flag.length()<4){
        std::cout << "invalid entry \n" << std::endl;
        return 1;
    }
    int t = std::stoi(flag.substr(3));
    if(!flag.substr(1,1).compare("t")) args[0] = t;
    else args[1] = t;
  }
  

  nc::ngramCounter ngram_counter(argv[1], args[0],args[1]);
  ngram_counter.compute();
  ngram_counter.display();
}