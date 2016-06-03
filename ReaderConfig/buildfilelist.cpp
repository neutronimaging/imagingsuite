#include <map>
#include <list>
#include <strings/filenames.h>

#include "buildfilelist.h"

std::list<std::string> BuildFileList(std::list<ImageLoader> &il)
{
    std::list<int> skiplist;

    return BuildFileList(il,skiplist);
}

std::list<std::string> BuildFileList(std::list<ImageLoader> &il, std::list<int> &skiplist)
{
    std::list<std::string> flist;
    std::string fname,ext;
    int fcnt=0;
    auto skipit=skiplist.begin();

    for (auto ilit=il.begin(); ilit!=il.end(); ilit++ ) {
        ImageLoader &loader = *ilit;

        for (int i=loader.m_nFirst; i<=loader.m_nLast; i++, fcnt++) {
            if ((!skiplist.empty()) && (skipit!=skiplist.end())) {
                if (fcnt==*skipit) {
                    skipit++;
                    continue;
                }
            }
            kipl::strings::filenames::MakeFileName(loader.m_sFilemask,i,fname,ext,'#','0');
            flist.push_back(fname);
        }
    }
    return flist;
}

std::map<float,std::string> BuildProjectionFileList(std::list<ImageLoader> &il, int sequence, double arc)
{
    std::list<int> skiplist;

    return BuildProjectionFileList(il,skiplist,sequence,arc);
}

std::map<float,std::string> BuildProjectionFileList(std::list<ImageLoader> &il, std::list<int> &skiplist, int sequence, double arc)
{
    std::map<float, std::string> flist;
    std::string fname,ext;
    const float phi=(1.0f+sqrt(5))*0.5f;

    int fcnt=0;
    auto skipit=skiplist.begin();


    for (auto ilit=il.begin(); ilit!=il.end(); ilit++ ) {
        ImageLoader &loader = *ilit;

        for (int i=loader.m_nFirst, j=i; i<=loader.m_nLast; i++,fcnt++) {
            if ((!skiplist.empty()) && (skipit!=skiplist.end())) {
                if (fcnt==*skipit) {
                    skipit++;
                    continue;
                }
            }
            j++;
            kipl::strings::filenames::MakeFileName(loader.m_sFilemask,i,fname,ext,'#','0');
            if (sequence==0)
                flist.insert(make_pair(j,fname));
            else
                flist.insert(make_pair(fmod(j*phi*arc,arc),fname));
        }
    }
    return flist;
}
