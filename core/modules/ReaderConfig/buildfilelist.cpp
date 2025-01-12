#include <map>
#include <list>
#include <strings/filenames.h>
#include <logging/logger.h>

#include "buildfilelist.h"

std::vector<std::string> BuildFileList(FileSet &il)
{
   std::vector<std::string> flist;

   std::string fname,ext;
   int fcnt=0;

   FileSet &loader = il;
   auto skipit=loader.m_nSkipList.begin();
   for (int i=loader.m_nFirst; i<=loader.m_nLast; i++, fcnt++)
   {
       if ((!loader.m_nSkipList.empty()) && (skipit!=loader.m_nSkipList.end()))
       {
           if (fcnt==*skipit)
           {
               skipit++;
               continue;
           }
       }

       kipl::strings::filenames::MakeFileName(loader.m_sFilemask,i,fname,ext,'#','0');
       flist.push_back(fname);
   }

   return flist;
}

std::vector<std::string> BuildFileList(std::vector<FileSet> &il)
{
    std::vector<std::string> flist,tmplist;
    std::string fname,ext;

    for (auto & loader : il )
    {
        tmplist=BuildFileList(loader);
        flist.insert(flist.end(),tmplist.begin(),tmplist.end());
    }
    return flist;

}

std::map<float,std::string> BuildProjectionFileList(std::vector<FileSet> &il, int sequence, int goldenStartIdx, double arc)
{
    std::vector<int> skiplist;

    return BuildProjectionFileList(il,skiplist,sequence,goldenStartIdx,arc);
}

std::map<float,std::string> BuildProjectionFileList(std::vector<FileSet> &il, std::vector<int> &skiplist, int sequence, int goldenStartIdx, double arc)
{
    kipl::logging::Logger logger("BuildProjectionFileList");

    std::ostringstream msg;
    std::map<float, std::string> flist;
    std::string fname,ext;
    const float phi=(1.0f+sqrt(5.0f))*0.5f;

    int fcnt=0;
    auto skipit=skiplist.begin();
    int j=il.begin()->m_nFirst-goldenStartIdx;

    for (auto & loader : il)
    {
        msg.str("");
        msg<<"Current loader has file mask "<<loader.m_sFilemask<<", first="<<loader.m_nFirst<<", last="<<loader.m_nLast;;
        logger.verbose(msg.str());

        for (int i=loader.m_nFirst; i<=loader.m_nLast; ++i,++fcnt)
        {
            if ((!skiplist.empty()) && (skipit!=skiplist.end()))
            {
                if (fcnt==*skipit)
                {
                    skipit++;
                    continue;
                }
            }

            kipl::strings::filenames::MakeFileName(loader.m_sFilemask,i,fname,ext,'#','0');
            if (sequence==0)
                flist.insert(make_pair(static_cast<float>(j),fname));
            else
                flist.insert(make_pair(static_cast<float>(fmod(j*phi*arc,arc)),fname));

            ++j;
        }
    }
    msg.str(""); msg<<"Built list size="<<flist.size();
    logger.verbose(msg.str());

    return flist;
}
