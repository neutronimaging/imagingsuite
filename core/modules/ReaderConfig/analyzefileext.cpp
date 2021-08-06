//<LICENSE>

#include <string>
#include <sstream>
#include <map>
#include <algorithm>

#include "analyzefileext.h"
#include <strings/filenames.h>
#include <base/KiplException.h>

namespace readers {



eExtensionTypes GetFileExtensionType(std::string fname)
{
    std::string ext=kipl::strings::filenames::GetFileExtension(fname);

    eExtensionTypes et;
    string2enum(ext,et);
    return et;
}

}

/// \brief Translates the value of an extension enum to a string
/// \param et The enum value to translate
/// \returns A string with the value
std::string READERCONFIGSHARED_EXPORT enum2string(readers::eExtensionTypes et)
{
    std::string ext;

    switch (et) {
    case readers::ExtensionTXT:   ext="txt"; break;
    case readers::ExtensionDMP:   ext="dmp"; break;
    case readers::ExtensionDAT:   ext="dat"; break;
    case readers::ExtensionXML:   ext="xml"; break;
    case readers::ExtensionRAW:   ext="raw";  break;
    case readers::ExtensionFITS:  ext="fit"; break;
    case readers::ExtensionPNG:   ext="png"; break;
    case readers::ExtensionJPG:   ext="jpg"; break;
    case readers::ExtensionTIFF:  ext="tif"; break;
    case readers::ExtensionHDF4:  ext="hd4"; break;
    case readers::ExtensionHDF5:  ext="hd5"; break;
    case readers::ExtensionHDF:   ext="hdf"; break;
    case readers::ExtensionSEQ:   ext="seq"; break;

    default: throw kipl::base::KiplException("Unable to translate the extension enum to a string",__FILE__, __LINE__);
    }

    return ext;
}

/// \brief Translates a string to an extension enum
/// \param ext a string containing a file extension
/// \param et target enum variable
void READERCONFIGSHARED_EXPORT string2enum(std::string ext, readers::eExtensionTypes &et)
{
    std::ostringstream msg;
    std::string e=ext;
    std::transform(ext.begin(),ext.end(),e.begin(),tolower);
    if (e[0]=='.')
        e=e.substr(1);

    std::map<std::string,readers::eExtensionTypes> extmap;

    extmap["txt"]   = readers::ExtensionTXT;
    extmap["dmp"]   = readers::ExtensionDMP;
    extmap["dat"]   = readers::ExtensionDAT;
    extmap["xml"]   = readers::ExtensionXML;
    extmap["raw"]   = readers::ExtensionRAW;
    extmap["fit"]   = readers::ExtensionFITS;
    extmap["fts"]   = readers::ExtensionFITS;
    extmap["fits"]  = readers::ExtensionFITS;
    extmap["png"]   = readers::ExtensionPNG;
    extmap["jpg"]   = readers::ExtensionJPG;
    extmap["jpeg"]  = readers::ExtensionJPG;
    extmap["tif"]   = readers::ExtensionTIFF;
    extmap["tiff"]  = readers::ExtensionTIFF;
    extmap["hd4"]   = readers::ExtensionHDF4;
    extmap["hdf4"]  = readers::ExtensionHDF4;
    extmap["hd5"]   = readers::ExtensionHDF5;
    extmap["hdf5"]  = readers::ExtensionHDF5;
    extmap["hdf"]   = readers::ExtensionHDF;
    extmap["seq"]   = readers::ExtensionSEQ;

    auto it=extmap.find(e);

    if (it!=extmap.end())
        et=it->second;
    else {
        msg.str("");
        msg<<"Unknown file extension "<<ext<<" -> "<<e;
        throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
    }
}

std::ostream & operator<<(std::ostream &s, readers::eExtensionTypes etype)
{
    s<<enum2string(etype);

    return s;
}

