//<LICENCE>

#include <string>
#include <map>
#include <algorithm>

#include "../../include/io/analyzefileext.h"
#include "../../include/strings/filenames.h"
#include "../../include/base/KiplException.h"

namespace kipl {

namespace io {

eExtensionTypes GetFileExtensionType(std::string fname)
{
    std::string ext=kipl::strings::filenames::GetFileExtension(fname);

    eExtensionTypes et;
    string2enum(ext,et);
    return et;
}

}}

/// \brief Translates the value of an extension enum to a string
/// \param et The enum value to translate
/// \returns A string with the value
std::string KIPLSHARED_EXPORT enum2string(kipl::io::eExtensionTypes et)
{
    std::string ext;

    switch (et) {
    case kipl::io::ExtensionTXT:  ext="txt"; break;
    case kipl::io::ExtensionDMP:  ext="dmp"; break;
    case kipl::io::ExtensionDAT:  ext="dat"; break;
    case kipl::io::ExtensionXML:  ext="xml"; break;
    case kipl::io::ExtensionRAW:  ext="raw";  break;
    case kipl::io::ExtensionFITS: ext="fits"; break;
    case kipl::io::ExtensionPNG:  ext="png"; break;
    case kipl::io::ExtensionJPG:  ext="jpg"; break;
    case kipl::io::ExtensionTIFF: ext="tif"; break;
    case kipl::io::ExtensionMAT:  ext="mat"; break;
    case kipl::io::ExtensionHDF4:  ext="hd4"; break;
    case kipl::io::ExtensionHDF5:  ext="hd5"; break;
    case kipl::io::ExtensionHDF:  ext="hdf"; break;

    default: throw kipl::base::KiplException("Unable to translate the extension enum to a string",__FILE__, __LINE__);
    }

    return ext;
}

/// \brief Translates a string to an extension enum
/// \param ext a string containing a file extension
/// \param et target enum variable
void KIPLSHARED_EXPORT string2enum(std::string ext, kipl::io::eExtensionTypes &et)
{
    std::string e=ext;
    std::transform(e.begin(),e.begin(),e.end(),tolower);
    if (e[0]=='.')
        e=e.substr(1);

    std::map<std::string,kipl::io::eExtensionTypes> extmap;

    extmap["txt"]   = kipl::io::ExtensionTXT;
    extmap["dmp"]   = kipl::io::ExtensionDMP;
    extmap["dat"]   = kipl::io::ExtensionDAT;
    extmap["xml"]   = kipl::io::ExtensionXML;
    extmap["raw"]   = kipl::io::ExtensionRAW;
    extmap["fit"]   = kipl::io::ExtensionFITS;
    extmap["fts"]   = kipl::io::ExtensionFITS;
    extmap["fits"]  = kipl::io::ExtensionFITS;
    extmap["png"]   = kipl::io::ExtensionPNG;
    extmap["jpg"]   = kipl::io::ExtensionJPG;
    extmap["jpeg"]  = kipl::io::ExtensionJPG;
    extmap["tif"]   = kipl::io::ExtensionTIFF;
    extmap["tiff"]  = kipl::io::ExtensionTIFF;
    extmap["mat"]   = kipl::io::ExtensionMAT;
    extmap["hd4"]   = kipl::io::ExtensionHDF4;
    extmap["hd5"]   = kipl::io::ExtensionHDF5;
    extmap["hdf"]   = kipl::io::ExtensionHDF;

    auto it=extmap.find(ext);

    if (it!=extmap.end())
        et=it->second;
    else {
        throw kipl::base::KiplException("Unknown file extension",__FILE__,__LINE__);
    }
}

std::ostream & operator<<(std::ostream &s, kipl::io::eExtensionTypes etype)
{
    s<<enum2string(etype);

    return s;
}

