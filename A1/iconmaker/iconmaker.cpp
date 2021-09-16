#include "pngwriter.hpp"
#include <array>
using namespace std;

void horiz_line(pngwriter &png, string &color){
    if(!color.compare("r")) png.filledsquare(0,29,64,35,1.0,0.0,0.0);
    else if(!color.compare("y")) png.filledsquare(0,29,64,35,1.0,1.0,0.0);
    else if(!color.compare("g")) png.filledsquare(0,29,64,35,0.0,1.0,0.0);
}

void vert_line(pngwriter &png, string &color){
    if(!color.compare("r")) png.filledsquare(29,0,35,64,1.0,0.0,0.0);
    else if(!color.compare("y")) png.filledsquare(29,0,35,64,1.0,1.0,0.0);
    else if(!color.compare("g")) png.filledsquare(29,0,35,64,0.0,1.0,0.0);
}

void lr_diag(pngwriter &png, string &color){
    double colors[3] = {0.0,0.0,0.0};
    
    if(!color.compare("r")) colors[0] = 1.0;
    else if(!color.compare("y")){
        colors[0] = 1.0;
        colors[1] = 1.0;
    }
    else if(!color.compare("g")) colors[1] = 1.0;

    png.line(1,64,64,1,colors[0],colors[1],colors[2]);
    for(int i = 1; i < 4; i++){
        png.line(1,64-i,64-i,1,colors[0],colors[1],colors[2]);
        png.line(1+i,64,64,1+i,colors[0],colors[1],colors[2]);
    }

}

void diag_one(pngwriter &png, string &color){
    double colors[3] = {0.0,0.0,0.0};
    
    if(!color.compare("r")) colors[0] = 1.0;
    else if(!color.compare("y")){
        colors[0] = 1.0;
        colors[1] = 1.0;
    }
    else if(!color.compare("g")) colors[1] = 1.0;
    png.line(1,64,64,1,colors[0],colors[1],colors[2]);
    for(int i = 1; i < 4; i++){
        png.line(1,64-i,64-i,1,colors[0],colors[1],colors[2]);
        png.line(1+i,64,64,1+i,colors[0],colors[1],colors[2]);
    }
}

void diag_two(pngwriter &png, string &color){
    double colors[3] = {0.0,0.0,0.0};
    
    if(!color.compare("r")) colors[0] = 1.0;
    else if(!color.compare("y")){
        colors[0] = 1.0;
        colors[1] = 1.0;
    }
    else if(!color.compare("g")) colors[1] = 1.0;
    png.line(1,1,64,64,colors[0],colors[1],colors[2]);
    for(int i = 1; i < 4; i++){
        png.line(1,1+i,64-i,64,colors[0],colors[1],colors[2]);
        png.line(1+i,1,64,64-i,colors[0],colors[1],colors[2]);
    }
}

int main()
{
    std::array<string,3> colors = {"r","y","g"};
    
    for(auto i = 0; i<colors.size();i++){
        for(auto j = 0; j<colors.size();j++){
            //2 streets
            string file_path = "./files/i2"+colors[i]+colors[j]+".png";
            pngwriter png(64,64,65535,file_path.c_str());
            // string horiz = colors[i];
            // string vert = colors[j];
            horiz_line(png,colors[i]);
            vert_line(png,colors[j]);
            png.close();

            for(auto k=0; k<colors.size();k++){
                //3 streets
                string file_path = "./files/i3"+colors[i]+colors[j]+colors[k]+".png";
                pngwriter png(64,64,65535,file_path.c_str());
                horiz_line(png,colors[i]);
                diag_one(png,colors[j]);
                diag_two(png,colors[k]);
                png.close();

                for(auto m=0; m<colors.size();m++){
                    //4 streets
                    string file_path = "./files/i4"+colors[i]+colors[j]+colors[k]+colors[m]+".png";
                    pngwriter png(64,64,65535,file_path.c_str());
                    horiz_line(png,colors[i]);
                    diag_one(png,colors[j]);
                    vert_line(png,colors[k]);
                    diag_two(png,colors[m]);
                    png.close();
                }
            }
        }
    }

    // pngwriter test(64,64,65535,"./files/test.png");
    // test.filledsquare(0,29,64,35,0.0,1.0,0.0);
    // test.filledsquare(29,0,35,64,0.0,1.0,0.0);
    // test.close();

    // pngwriter png(64,64,65535,"./files/test.png");
   
    // // 2 streets intersection
    // // png.line(0,32,64,32,65535,0,0);
    // png.filledsquare(0,29,64,35,65535,0,0);
    // png.filledsquare(29,0,35,64,65535,0,0);
    // png.close();
   
    return 0;
}



