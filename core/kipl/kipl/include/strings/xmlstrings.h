#ifndef XMLSTRINGS_H
#define XMLSTRINGS_H

#include "../../include/kipl_global.h"
#include <string>

namespace kipl {
namespace strings {

std::string KIPLSHARED_EXPORT xmlString(std::string tag, std::string value, int indent=4);
std::string KIPLSHARED_EXPORT xmlString(std::string tag, int value, int indent=4);
std::string KIPLSHARED_EXPORT xmlString(std::string tag, size_t value, int indent=4);
std::string KIPLSHARED_EXPORT xmlString(std::string tag, float value, int indent=4);
std::string KIPLSHARED_EXPORT xmlString(std::string tag, double value, int indent=4);
std::string KIPLSHARED_EXPORT xmlString(std::string tag, bool value, int indent=4);

}
}
#endif // XMLSTRINGS_H
