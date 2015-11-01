#ifndef BUILDFILELIST_H
#define BUILDFILELIST_H

#include <list>
#include <string>

#include "datasetbase.h"

std::list<std::string> BuildFileList(std::list<ImageLoader> &il);
std::list<std::string> BuildFileList(std::list<ImageLoader> &il, std::list<int> &skiplist);

#endif // BUILDFILELIST_H
