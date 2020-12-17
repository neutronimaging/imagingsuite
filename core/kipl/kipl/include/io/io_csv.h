#ifndef IO_CSV_H
#define IO_CSV_H

#include <map>
#include <vector>
#include <string>

namespace kipl {
namespace io {
    std::map<std::string, std::vector<float> > readCSV(const std::string &fname, char delim =',', bool haveHeader=true);
}
}
#endif // IO_CSV_H
