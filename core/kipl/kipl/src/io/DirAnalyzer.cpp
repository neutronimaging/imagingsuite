//<LICENCE>

#include <sstream>
#include <iostream>
#include <stddef.h>
#include <string>
#include <cstdlib>
#include <limits>
#include <algorithm>
#include <iterator>
#include <dirent.h>
#include "../../include/base/KiplException.h"
#include "../../include/io/DirAnalyzer.h"
#include "../../include/strings/filenames.h"

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
    m_sExt("tif"),
    m_sPath("")
{
}

FileItem::FileItem(std::string mask, int index, std::string ext, std::string path) :
    m_sMask(mask),
    m_nIndex(index),
    m_sExt(ext),
    m_sPath(path)
{

}


DirAnalyzer::DirAnalyzer() :
    logger("DirAnalyzer")
{

}

std::vector<std::string> DirAnalyzer::GetDirList(std::string path)
{
    kipl::strings::filenames::CheckPathSlashes(path,false);

    m_vDirContents.clear();

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (path.c_str())) != nullptr) {
      while ((ent = readdir (dir)) != nullptr) {
        m_vDirContents.push_back(ent->d_name);
      }
      closedir (dir);
    } else {
        // could not open directory
        std::stringstream msg;
        msg<<"Could not open path "<<path;
        throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
    }

    return m_vDirContents;
}

FileItem DirAnalyzer::GetFileMask(std::string str)
{
    kipl::strings::filenames::CheckPathSlashes(str,false);
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

    std::string path="";
    epos=str.find_last_of(kipl::strings::filenames::slash);
    if (epos!=std::string::npos)
        path=str.substr(0,epos+1);

    return FileItem(mask,value,ext,path);
}

void DirAnalyzer::AnalyzeMatchingNames(std::string mask,
                                       int &nFiles,
                                       int &firstIndex,
                                       int &lastIndex,
                                       char wildcard)
{
    std::ostringstream msg;
    nFiles     = 0;
    firstIndex = std::numeric_limits<int>::max();
    lastIndex  = 0;

    std::string m2=mask;
    kipl::strings::filenames::CheckPathSlashes(m2,false);
    size_t wpos0 = m2.find_last_of(strings::filenames::slash);
    wpos0 = wpos0 !=std::string::npos ? wpos0+1 : 0L;

    std::string path = wpos0 != 0 ? m2.substr(0,wpos0) :"./";
    GetDirList(path);

    std::string maskname = m2.substr(wpos0);
    size_t wpos1 = maskname.find_first_of(wildcard);

    if (wpos1== std::string::npos) {
        logger(logger.LogWarning,"Didn't find the wildcard character.");
        return;
    }

    ptrdiff_t wpos2 = maskname.find_first_not_of(wildcard,wpos1);

    std::string maskbase = maskname.substr(0,wpos1);
    std::string maskext  = maskname.substr(wpos2);

    std::string base,ext;
    std::string idxstr;

    for (auto it=m_vDirContents.begin(); it!=m_vDirContents.end(); ++it) {
        std::string &name = *it;

        if (name.size()==maskname.size()) {
            base = name.substr(0,wpos1);
            ext  = name.substr(wpos2);
            if ((base == maskbase) && (ext == maskext)) {
                ++nFiles;
                idxstr = name.substr(wpos1,wpos2-wpos1);
                int idx=atoi(idxstr.c_str());
                firstIndex = std::min(firstIndex,idx);
                lastIndex  = std::max(lastIndex,idx);
            }
        }
    }
}

void DirAnalyzer::AnalyzeFileList(std::string fname,
                     int &nFiles)
{
    std::ifstream reader(fname.c_str());

    if (reader.bad())
        throw kipl::base::KiplException("Could not open list file.",__FILE__,__LINE__);

    // new lines will be skipped unless we stop it from happening:
    reader.unsetf(std::ios_base::skipws);

    // count the newlines with an algorithm specialized for counting:
    unsigned line_count = std::count(
          std::istream_iterator<char>(reader),
          std::istream_iterator<char>(),
          '\n');

    nFiles=line_count;
}

void DirAnalyzer::AnalyzeDirList(std::vector<std::string> &dirList, std::map<std::string,std::pair<int,int>> &masks)
{
    masks.clear();

    FileItem fi;

    for (auto it = dirList.begin(); it != dirList.end(); ++it) {
        if ((*it)[0]=='.')
            continue;

        fi=GetFileMask(*it);
        if (masks.find(fi.m_sMask) != masks.end()) {
            masks[fi.m_sMask].first=std::min(masks[fi.m_sMask].first,fi.m_nIndex);
            masks[fi.m_sMask].second=std::max(masks[fi.m_sMask].second,fi.m_nIndex);
        }
        else
            masks[fi.m_sMask]=std::make_pair(fi.m_nIndex,fi.m_nIndex);
    }

}

}}
