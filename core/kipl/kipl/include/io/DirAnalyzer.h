//<LICENCE>

#ifndef DIRANALYZER_H
#define DIRANALYZER_H
#include "../kipl_global.h"

#include <vector>
#include <string>

#include "../logging/logger.h"

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
    FileItem(std::string mask, int index, std::string ext, std::string path);

    std::string m_sMask;    ///< File name mask
    int m_nIndex;           ///< File index number
    std::string m_sExt;     ///< File extension
    std::string m_sPath;
};


/// \brief Helper class to analyze file names and directories
class KIPLSHARED_EXPORT DirAnalyzer
{
    kipl::logging::Logger logger;
public:
    DirAnalyzer();

    /// \brief Gets a list of the contents of a directory
    ///
    /// The method shall be able to identify groups of file name sequences and tell the index number intervals.
    /// \param path the path to the directory to analyze
    /// \todo Implement the file list...
    std::vector<std::string> GetDirList(std::string path);

    void AnalyzeMatchingNames(std::string mask,
                              int &nFiles,
                              int &firstIndex,
                              int &lastIndex,
                              char wildcard='#');

    void AnalyzeFileList(std::string fname,
                         int &nFiles);

    /// \brief Gets the file mask from a file name string. The last set of numbers before the file extension are replaced by #'s
    FileItem GetFileMask(std::string str);

    /// \brief Analyses the contents of a file list for different file masks.
    /// \param dirlist A vector containing filenames as privided by GetDirList
    /// \param masks A list of file masks and a pair containing first and last index
    void AnalyzeDirList(std::vector<std::string> &dirList, std::map<std::string,pair<int,int> > &masks);
protected:
    std::vector<std::string> m_vDirContents;
};

}}
#endif // DIRANALYZER_H
