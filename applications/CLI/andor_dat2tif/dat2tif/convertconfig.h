#ifndef CONVERTCONFIG_H
#define CONVERTCONFIG_H

#include <iostream>
#include <base/kiplenums.h>
#include <string>
#include <list>

class ConvertConfig
{
public:
    ConvertConfig();
    ~ConvertConfig();

    void ParseCLI(std::list<std::string> &args);

    size_t size[2];
    int first;
    int last;
    size_t offset; //< Given in bytes
    size_t stride; //< Given in bytes
    int imagesperfile;
    kipl::base::eDataType dt;
    kipl::base::eEndians endian;
    int imageindex;
    size_t nCrop[4];
    bool reverseindex;
    bool rotate;
    std::string src_mask;
    std::string dest_mask;
};

std::ostream & operator<<(std::ostream &s, ConvertConfig &c);
#endif // CONVERTCONFIG_H
