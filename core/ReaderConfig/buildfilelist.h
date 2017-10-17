#ifndef BUILDFILELIST_H
#define BUILDFILELIST_H

#include <list>
#include <map>
#include <string>

#include "readerconfig_global.h"
#include "datasetbase.h"

std::list<std::string> READERCONFIGSHARED_EXPORT BuildFileList(std::list<ImageLoader> &il);
std::list<std::string> READERCONFIGSHARED_EXPORT BuildFileList(std::list<ImageLoader> &il, std::list<int> &skiplist);
std::map<float,std::string> READERCONFIGSHARED_EXPORT BuildProjectionFileList(std::list<ImageLoader> &il, int sequence, double arc);
std::map<float,std::string> READERCONFIGSHARED_EXPORT BuildProjectionFileList(std::list<ImageLoader> &il, std::list<int> &skiplist, int sequence, double arc);
#endif // BUILDFILELIST_H
