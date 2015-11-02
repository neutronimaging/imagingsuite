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

