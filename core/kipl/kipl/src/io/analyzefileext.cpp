//<LICENSE>

#include <string>
#include <sstream>
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
    case kipl::io::ExtensionTXT:   ext="txt"; break;
    case kipl::io::ExtensionDMP:   ext="dmp"; break;
    case kipl::io::ExtensionDAT:   ext="dat"; break;
    case kipl::io::ExtensionXML:   ext="xml"; break;
    case kipl::io::ExtensionRAW:   ext="raw";  break;
    case kipl::io::ExtensionFITS:  ext="fits"; break;
    case kipl::io::ExtensionPNG:   ext="png"; break;
    case kipl::io::ExtensionJPG:   ext="jpg"; break;
    case kipl::io::ExtensionTIFF:  ext="tif"; break;
    case kipl::io::ExtensionHDF4:  ext="hd4"; break;
    case kipl::io::ExtensionHDF5:  ext="hd5"; break;
    case kipl::io::ExtensionHDF:   ext="hdf"; break;
    case kipl::io::ExtensionSEQ:   ext="seq"; break;

    default: throw kipl::base::KiplException("Unable to translate the extension enum to a string",__FILE__, __LINE__);
    }

    return ext;
}

/// \brief Translates a string to an extension enum
/// \param ext a string containing a file extension
/// \param et target enum variable
void KIPLSHARED_EXPORT string2enum(std::string ext, kipl::io::eExtensionTypes &et)
{
    std::ostringstream msg;
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
    extmap["hd4"]   = kipl::io::ExtensionHDF4;
    extmap["hd5"]   = kipl::io::ExtensionHDF5;
    extmap["hdf"]   = kipl::io::ExtensionHDF;
    extmap["seq"]   = kipl::io::ExtensionSEQ;

    auto it=extmap.find(e);

    if (it!=extmap.end())
        et=it->second;
    else {
        msg.str("");
        msg<<"Unknown file extension "<<ext<<" -> "<<e;
        throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
    }
}

std::ostream & operator<<(std::ostream &s, const kipl::io::eExtensionTypes &etype)
{
    s<<enum2string(etype);

    return s;
}

std::ostream & operator<<(std::ostream &s, const kipl::io::eFileType &ft)
{
    s<<enum2string(ft);

    return s;
}

std::string enum2string(const kipl::io::eFileType & ft)
{
std::string s;
switch (ft) {
case kipl::io::TIFF8bits    :           s="TIFF8bits";    break;
case kipl::io::TIFF16bits   :           s="TIFF16bits";   break;
case kipl::io::TIFFfloat    :           s="TIFFfloat";    break;
case kipl::io::TIFF16bitsMultiFrame   : s="TIFF16bitsMultiFrame";   break;
case kipl::io::TIFFfloatMultiFrame    : s="TIFFfloatMultiFrame";    break;
case kipl::io::NeXusfloat   :           s="NeXusfloat";   break;
case kipl::io::NeXus16bits  :           s="NeXus16bits";  break;
case kipl::io::PNG8bits     :           s="PNG8bits";     break;
case kipl::io::PNG16bits    :           s="PNG16bits";    break;

//default : throw kipl::base::KiplException("Unknown file type",__FILE__,__LINE__); break;
}

return s;
}

void string2enum(const std::string &str, kipl::io::eFileType &ft)
{
     if (str=="TIFF8bits")              ft=kipl::io::TIFF8bits;
else if (str=="TIFF16bits")             ft=kipl::io::TIFF16bits;
else if (str=="TIFFfloat")              ft=kipl::io::TIFFfloat;
else if (str=="TIFF16bitsMultiFrame")   ft=kipl::io::TIFF16bitsMultiFrame;
else if (str=="TIFFfloatMultiFrame")    ft=kipl::io::TIFFfloatMultiFrame;
else if (str=="NeXusfloat")             ft=kipl::io::NeXusfloat;
else if (str=="NeXus16bits")            ft=kipl::io::NeXus16bits;
else if (str=="PNG8bits")               ft=kipl::io::PNG8bits;
else if (str=="PNG16bits")              ft=kipl::io::PNG16bits;
else throw kipl::base::KiplException("Unknow matrix file type string",__FILE__,__LINE__);

}
