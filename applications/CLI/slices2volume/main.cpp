#include <string>
#include <iostream>
#include <slice2vol.h>

int main(int argc, char *argv[])
{
    std::cout<<argc<<"\n";
    for (int i=0; i<argc; ++i)
        std::cout<<argv[i]<<"\n";

    std::cout<<argv[1]<<" "<<std::stoul(argv[2])<<" "<<std::stoul(argv[3])<<" "<<std::stoul(argv[4])<<" "<<argv[5]<<"\n";
    Slice2Vol s2v;

    s2v.process(std::string(argv[1]),std::stoul(argv[2]),std::stoul(argv[3]),std::stoul(argv[4]),std::string(argv[5]));


    return 0;
}
