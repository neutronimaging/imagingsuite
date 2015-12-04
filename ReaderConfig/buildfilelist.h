#ifndef BUILDFILELIST_H
#define BUILDFILELIST_H

#include <list>
#include <string>

#include "readerconfig_global.h"
#include "datasetbase.h"

std::list<std::string> READERCONFIGSHARED_EXPORT BuildFileList(std::list<ImageLoader> &il);
std::list<std::string> READERCONFIGSHARED_EXPORT BuildFileList(std::list<ImageLoader> &il, std::list<int> &skiplist);

#endif // BUILDFILELIST_H
