#include "../../include/io/analyzefileext.h"
#include "../../include/strings/filenames.h"

#include <string>
#include <map>
#include <algorithm>

namespace kipl { namespace io {

eExtensionTypes GetExtensionType(std::string ext)
{
    std::string e=ext;
    std::transform(e.begin(),e.begin(),e.end(),tolower);
    if (e[0]=='.')
        e=e.substr(1);

    std::map<std::string,eExtensionTypes> extmap;

    extmap["txt"]  = ExtensionTXT;
    extmap["dmp"]  = ExtensionDMP;
    extmap["dat"]  = ExtensionDAT;
    extmap["xml"]  = ExtensionXML;
    extmap["raw"]  = ExtensionRAW;
    extmap["fit"]  = ExtensionFITS;
    extmap["fts"]  = ExtensionFITS;
    extmap["fits"] = ExtensionFITS;
    extmap["png"]  = ExtensionPNG;
    extmap["jpg"]  = ExtensionJPG;
    extmap["jpeg"]  = ExtensionJPG;
    extmap["tif"]  = ExtensionTIFF;
    extmap["tiff"] = ExtensionTIFF;
    extmap["mat"]  = ExtensionMAT;
    extmap["hd4"]  = ExtensionHDF;
    extmap["hd5"]  = ExtensionHDF;
    extmap["hdf"]  = ExtensionHDF;

    return extmap[e];
}

eExtensionTypes GetFileExtensionType(std::string fname)
{
    std::string ext=kipl::strings::filenames::GetFileExtension(fname);

    return GetExtensionType(ext);
}

}}

std::ostream & operator<<(std::ostream &s, kipl::io::eExtensionTypes etype)
{
    switch (etype) {
    case kipl::io::ExtensionTXT : s<<"ExtensionTXT";  break;
    case kipl::io::ExtensionDMP : s<<"ExtensionDMP";  break;
    case kipl::io::ExtensionDAT : s<<"ExtensionDAT";  break;
    case kipl::io::ExtensionXML : s<<"ExtensionXML";  break;
    case kipl::io::ExtensionRAW : s<<"ExtensionRAW";  break;
    case kipl::io::ExtensionFITS: s<<"ExtensionFITS"; break;
    case kipl::io::ExtensionTIFF: s<<"ExtensionTIFF"; break;
    case kipl::io::ExtensionPNG : s<<"ExtensionPNG";  break;
    case kipl::io::ExtensionMAT : s<<"ExtensionMAT";  break;
    case kipl::io::ExtensionHDF : s<<"ExtensionHDF";  break;
    }

    return s;
}

