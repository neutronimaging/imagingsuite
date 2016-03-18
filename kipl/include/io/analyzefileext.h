#ifndef ANALYZEFILEEXT_H
#define ANALYZEFILEEXT_H
#include <iostream>
#include <string>
#include "../kipl_global.h"

namespace kipl { namespace io {

enum eExtensionTypes {
    ExtensionTXT,
    ExtensionDMP,
    ExtensionDAT,
    ExtensionXML,
    ExtensionRAW,
    ExtensionFITS,
    ExtensionTIFF,
    ExtensionPNG,
    ExtensionJPG,
    ExtensionMAT,
    ExtensionHDF
};

eExtensionTypes KIPLSHARED_EXPORT GetExtensionType(std::string ext);

eExtensionTypes  KIPLSHARED_EXPORT GetFileExtensionType(std::string fname);

}}

std::ostream KIPLSHARED_EXPORT & operator<<(std::ostream &s, kipl::io::eExtensionTypes etype);

#endif // ANALYZEFILEEXT_H

