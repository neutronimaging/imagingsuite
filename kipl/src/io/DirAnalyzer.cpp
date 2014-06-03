//#include <dirent.h>
#include <sstream>
#include <iostream>
#include <string>
#include "../../include/base/KiplException.h"
#include "../../include/io/DirAnalyzer.h"

namespace kipl { namespace io {

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
    /*
    m_vDirContents.clear();

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (path.c_str())) != NULL) {
      while ((ent = readdir (dir)) != NULL) {
        m_vDirContents.push_back(ent->d_name);
      }
      closedir (dir);
    } else {
        // could not open directory
        std::stringstream msg;
        msg<<"Could not open path "<<path;
        throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
    }
    */
}

FileItem DirAnalyzer::GetFileMask(std::string str)
{
    ptrdiff_t p1=0;
    ptrdiff_t p2=0;
    ptrdiff_t p10=0;
    ptrdiff_t p20=0;
    do {
        p10=p1;
        p20=p2;
        p1=str.find_first_of("0123456789",p2);
        p2=str.find_first_not_of("0123456789",p1);
    } while (p2!=-1);

    std::string digits="-1";
    std::string mask=str;
    int value=-1;
    int numstart=0;

    if (p10 && p20 && (p1==-1)) {
        numstart=p10;
        digits=str.substr(p10,p20);
        value=atoi(digits.c_str());

        std::fill(mask.begin()+p10, mask.begin()+p20, '#');
    }
    else {
        if ((p1!=-1) && (p2==-1)) {
            numstart=p1;
            digits=str.substr(p1);
            value=atoi(digits.c_str());

            std::fill(mask.begin()+p1, mask.begin()+mask.size(), '#');
        }
    }

    std::string ext="";
    int epos=str.find_last_of(".");
    if ((epos!=-1) && (numstart<epos))
        ext=str.substr(epos+1);

    return FileItem(mask,value,ext);
}



}}
