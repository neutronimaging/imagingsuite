#ifndef IO_CSV_H
#define IO_CSV_H

#include "../kipl_global.h"

#include <map>
#include <vector>
#include <string>

namespace kipl {
namespace io {
    std::map<std::string, std::vector<float> > KIPLSHARED_EXPORT readCSV(const std::string &fname, char delim =',', bool haveHeader=true);
}
}
#endif // IO_CSV_H
