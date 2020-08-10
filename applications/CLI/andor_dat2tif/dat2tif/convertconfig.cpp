#include "convertconfig.h"
#include <string>
#include <cstdlib>

ConvertConfig::ConvertConfig() :
    first(0),
    last(0),
    offset(0),
    stride(100),
    imagesperfile(1),
    dt(kipl::base::UInt8),
    endian(kipl::base::BigEndian),
    imageindex(0),
    reverseindex(false),
    rotate(false),
    src_mask("img_####.dmp"),
    dest_mask("img_####.tif")
{
    size[0]=size[1]=100;
    nCrop[0]=nCrop[1]=nCrop[2]=nCrop[3]=0;
}

ConvertConfig::~ConvertConfig()
{

}

void ConvertConfig::ParseCLI(std::list<std::string> &args)
{
    std::list<std::string>::iterator it;

    for (it=args.begin(); it!=args.end(); it++) {
        std::cout<<*it<<std::endl;

        if (*it=="-size") {
            size[0]=atoi((*(++it)).c_str());
            size[1]=atoi((*(++it)).c_str());
            continue;
        }
        if (*it=="-offset") {
            offset=atoi((*(++it)).c_str());
            continue;
        }

        if (*it=="-first") {
            first=atoi((*(++it)).c_str());
            continue;
        }

        if (*it=="-last") {
            last=atoi((*(++it)).c_str());
            continue;
        }

        if (*it=="-stride") {
            stride=atoi((*(++it)).c_str());
            continue;
        }
        if (*it=="-imagesperfile") {
            imagesperfile=atoi((*(++it)).c_str());
            continue;
        }
        if (*it=="-imageindex") {
            imageindex=atoi((*(++it)).c_str());
            continue;
        }

        if (*it=="-crop") {
            nCrop[0]=atoi((*(++it)).c_str());
            nCrop[1]=atoi((*(++it)).c_str());
            nCrop[2]=atoi((*(++it)).c_str());
            nCrop[3]=atoi((*(++it)).c_str());
            continue;
        }

        if (*it=="-revind") {
            reverseindex=true;
        }

        if (*it=="-src") {
            it++;
            src_mask=*it;
            continue;
        }

        if (*it=="-dest") {
            it++;
            dest_mask=*it;
            continue;
        }

        if (*it=="-intype") {
            it++;
            string2enum(*it,dt);
        }

        if (*it=="-rotate") {
            rotate=true;
        }
      //  kipl::base::eEndians endian;
    }
}

std::ostream & operator<<(std::ostream &s, ConvertConfig &c)
{
    s<<"src_mask="<<c.src_mask<<std::endl;
    s<<"dest_mask="<<c.dest_mask<<std::endl;
    s<<"first="<<c.first<<std::endl;
    s<<"last="<<c.last<<std::endl;
    s<<"size="<<c.size[0]<<", "<<c.size[1]<<std::endl;
    s<<"offset="<<c.offset<<std::endl;
    s<<"stride="<<c.stride<<std::endl;
    s<<"imagesperfile="<<c.imagesperfile<<std::endl;
    s<<"datatype="<<c.dt<<std::endl;
    s<<"endian="<<c.endian<<std::endl;
    s<<"imageindex="<<c.imageindex<<std::endl;
    s<<"crop="<<c.nCrop[0]<<" "<<c.nCrop[1]<<" "<<c.nCrop[2]<<" "<<c.nCrop[3]<<std::endl;
    s<<"revind="<<(c.reverseindex ? "true": "false")<<std::endl;

    return s;
}
