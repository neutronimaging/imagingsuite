#ifndef BUILDFILELIST_H
#define BUILDFILELIST_H

#include <vector>
#include <map>
#include <string>

#include "readerconfig_global.h"
#include "fileset.h"
std::vector<std::string> READERCONFIGSHARED_EXPORT BuildFileList(FileSet &il);
std::vector<std::string> READERCONFIGSHARED_EXPORT BuildFileList(std::vector<FileSet> &il);
std::vector<std::string> READERCONFIGSHARED_EXPORT BuildFileList(std::vector<FileSet> &il, std::vector<int> &skiplist);
std::map<float,std::string> READERCONFIGSHARED_EXPORT BuildProjectionFileList(std::vector<FileSet> &il, int sequence, int goldenStartIdx, double arc);
std::map<float,std::string> READERCONFIGSHARED_EXPORT BuildProjectionFileList(std::vector<FileSet> &il, std::vector<int> &skiplist, int sequence, int goldenStartIdx, double arc);
#endif // BUILDFILELIST_H
