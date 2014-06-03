#ifndef DIRANALYZER_H
#define DIRANALYZER_H
#include <vector>
#include <string>

namespace kipl { namespace io {
class FileItem
{
public:
    FileItem();
    FileItem(std::string mask, int index, std::string ext);

    std::string m_sMask;
    int m_nIndex;
    std::string m_sExt;
};

class DirAnalyzer
{
public:
    DirAnalyzer();

    void GetDirList(std::string path);
    FileItem GetFileMask(std::string str);
protected:
    std::vector<std::string> m_vDirContents;
};

}}
#endif // DIRANALYZER_H
