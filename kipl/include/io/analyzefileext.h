#ifndef ANALYZEFILEEXT_H
#define ANALYZEFILEEXT_H
#include <iostream>
#include <string>

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
    ExtensionMAT,
    ExtensionHDF
};

eExtensionTypes GetExtensionType(std::string ext);

eExtensionTypes GetFileExtensionType(std::string fname);

}}

std::ostream & operator<<(std::ostream &s, kipl::io::eExtensionTypes etype);

#endif // ANALYZEFILEEXT_H

