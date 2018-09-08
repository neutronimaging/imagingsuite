#include <map>
#include <list>
#include <strings/filenames.h>
#include <logging/logger.h>

#include "buildfilelist.h"

std::list<std::string> BuildFileList(ImageLoader &il)
{
   std::list<std::string> flist;

   std::string fname,ext;
   int fcnt=0;

   ImageLoader &loader = il;
   auto skipit=loader.m_nSkipList.begin();
   for (int i=loader.m_nFirst; i<=loader.m_nLast; i++, fcnt++) {
       if ((!loader.m_nSkipList.empty()) && (skipit!=loader.m_nSkipList.end())) {
           if (fcnt==*skipit) {
               skipit++;
               continue;
           }
       }
       kipl::strings::filenames::MakeFileName(loader.m_sFilemask,i,fname,ext,'#','0');
       flist.push_back(fname);
   }

   return flist;
}

std::list<std::string> BuildFileList(std::list<ImageLoader> &il)
{
    std::list<std::string> flist,tmplist;
    std::string fname,ext;
    int fcnt=0;


    for (auto ilit=il.begin(); ilit!=il.end(); ilit++ ) {
        ImageLoader &loader = *ilit;
        tmplist=BuildFileList(loader);
        flist.insert(flist.end(),tmplist.begin(),tmplist.end());
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
    kipl::logging::Logger logger("BuildProjectionFileList");

    std::ostringstream msg;
    std::map<float, std::string> flist;
    std::string fname,ext;
    const float phi=(1.0f+sqrt(5))*0.5f;

    int fcnt=0;
    auto skipit=skiplist.begin();
    int j=il.begin()->m_nFirst;

    for (auto ilit=il.begin(); ilit!=il.end(); ++ilit ) {
        ImageLoader &loader = *ilit;
        msg.str("");
        msg<<"Current loader has file mask "<<loader.m_sFilemask<<", first="<<loader.m_nFirst<<", last="<<loader.m_nLast;;
        logger(logger.LogDebug,msg.str());

        for (int i=loader.m_nFirst; i<=loader.m_nLast; ++i,++fcnt) {
            if ((!skiplist.empty()) && (skipit!=skiplist.end())) {
                if (fcnt==*skipit) {
                    skipit++;
                    continue;
                }
            }

            kipl::strings::filenames::MakeFileName(loader.m_sFilemask,i,fname,ext,'#','0');
            if (sequence==0)
                flist.insert(make_pair(j,fname));
            else
                flist.insert(make_pair(fmod(j*phi*arc,arc),fname));

            ++j;
        }
    }
    msg.str("");
    msg<<"Built list size="<<flist.size();
    logger(logger.LogDebug,msg.str());

    return flist;
}
