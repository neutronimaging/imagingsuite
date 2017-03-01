//<LICENCE>

#ifndef DIRANALYZER_H
#define DIRANALYZER_H
#include "../kipl_global.h"

#include <vector>
#include <string>

namespace kipl { namespace io {

/// \brief File list item class
class KIPLSHARED_EXPORT FileItem
{
public:
    /// \brief C'tor to create an empty file item
    FileItem();
    /// \brief C'tor to initalize a file item
    /// \param mask A file mask with wildcard for file index
    /// \param index the index number of the file
    /// \param ext file extension of the file
    FileItem(std::string mask, int index, std::string ext);

    std::string m_sMask;    ///< File name mask
    int m_nIndex;           ///< File index number
    std::string m_sExt;     ///< File extension
};


/// \brief Helper class to analyze file names and directories
class KIPLSHARED_EXPORT DirAnalyzer
{
public:
    DirAnalyzer();

    /// \brief Gets a list of the contents of a directory
    ///
    /// The method shall be able to identify groups of file name sequences and tell the index number intervals.
    /// \param path the path to the directory to analyze
    /// \todo Implement the file list...
    void GetDirList(std::string path);

    /// \brief Gets the file mask from a file name string. The last set of numbers before the file extension are replaced by #'s
    FileItem GetFileMask(std::string str);
protected:
    std::vector<std::string> m_vDirContents;
};

}}
#endif // DIRANALYZER_H
