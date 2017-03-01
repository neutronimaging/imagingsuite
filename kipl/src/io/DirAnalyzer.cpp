//<LICENCE>

#include <sstream>
#include <iostream>
#include <stddef.h>
#include <string>
#include <cstdlib>
#include "../../include/base/KiplException.h"
#include "../../include/io/DirAnalyzer.h"

#ifdef _MSC_VER
   #include <io.h>
   #define access    _access_s
#else
   #include <unistd.h>
#endif


namespace kipl { namespace io {

bool FileExists( const std::string &Filename )
{
    return access( Filename.c_str(), 0 ) == 0;
}

FileItem::FileItem() :
    m_sMask("noname_####.tif"),
    m_nIndex(0),
    m_sExt("tif")
{
}

FileItem::FileItem(std::string mask, int index, std::string ext) :
    m_sMask(mask),
    m_nIndex(index),
    m_sExt(ext)
{

}


DirAnalyzer::DirAnalyzer()
{

}

void DirAnalyzer::GetDirList(std::string path)
{

//    m_vDirContents.clear();

//    DIR *dir;
//    struct dirent *ent;
//    if ((dir = opendir (path.c_str())) != NULL) {
//      while ((ent = readdir (dir)) != NULL) {
//        m_vDirContents.push_back(ent->d_name);
//      }
//      closedir (dir);
//    } else {
//        // could not open directory
//        std::stringstream msg;
//        msg<<"Could not open path "<<path;
//        throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
//    }

}

FileItem DirAnalyzer::GetFileMask(std::string str)
{
    std::string::size_type p1=0;
    std::string::size_type p2=0;
    std::string::size_type p10=0;
    std::string::size_type p20=0;
    do {
        p10=p1;
        p20=p2;
        p1=str.find_first_of("0123456789",p2);
        p2=str.find_first_not_of("0123456789",p1);
    } while (p2!=std::string::npos);

    std::string digits="-1";
    std::string mask=str;
    int value=-1;
    std::string::size_type numstart=0;

    if (p10 && p20 && (p1==std::string::npos)) {
        numstart=p10;
        digits=str.substr(p10,p20);
        value=atoi(digits.c_str());

        std::fill(mask.begin()+p10, mask.begin()+p20, '#');
    }
    else {
        if ((p1!=std::string::npos) && (p2==std::string::npos)) {
            numstart=p1;
            digits=str.substr(p1);
            value=atoi(digits.c_str());

            std::fill(mask.begin()+p1, mask.begin()+mask.size(), '#');
        }
    }

    std::string ext="";
    std::string::size_type epos=str.find_last_of('.');
    if ((epos!=std::string::npos) && (numstart<epos))
        ext=str.substr(epos+1);

    return FileItem(mask,value,ext);
}



}}
