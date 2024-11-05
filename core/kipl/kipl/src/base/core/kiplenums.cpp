

#include <iostream>
#include <map>
#include <algorithm>
#include <sstream>

#include "../../../include/base/kiplenums.h"
#include "../../../include/base/KiplException.h"

namespace kipl { namespace base {
KIPLSHARED_EXPORT int getConnectivityDims(kipl::base::eConnectivity &conn)
{
    int dims=0;

    switch (conn) {
    case kipl::base::conn4  : dims=2; break;
    case kipl::base::conn8  : dims=2; break;
    case kipl::base::conn6  : dims=3; break;
    case kipl::base::conn18 : dims=3; break;
    case kipl::base::conn26 : dims=3; break;

    default :
        throw kipl::base::KiplException("Could not transform a Connectivity enum to a string", __FILE__, __LINE__);
    }
    return dims;
}

int connectivityNeighbors(kipl::base::eConnectivity conn)
{
    switch (conn) {
    case conn4  : return 4;
    case conn8  : return 8;
    case conn6  : return 6;
    case conn18 : return 18;
    case conn26 : return 26;
    case euclid : return 0;
    default :
        throw kipl::base::KiplException("Unknown connectivity enum value encountered", __FILE__, __LINE__);
    };
}

kipl::base::eOperatingSystem getOperatingSystem()
{
#ifdef _WIN32
    return kipl::base::OSWindows;
#elif __APPLE__
    #include "TargetConditionals.h"
    #if TARGET_OS_MAC
        return kipl::base::OSMacOS;
    #else
        return kipl::base::OSUnknown;
    #endif
#elif __linux__
    return kipl::base::OSLinux;
#elif __unix__ // all unices not caught above
    return kipl::base::OSLinux;
#elif defined(_POSIX_VERSION)
    return kipl::base::OSLinux;
#else
    return kipl::base::OSUnknown;
#endif
}

}

}
std::string enum2string(kipl::base::eImagePlanes plane) {
	std::string s;
	switch (plane) {
		case kipl::base::ImagePlaneXY : s="imageplanexy"; break;
		case kipl::base::ImagePlaneXZ : s="imageplanexz"; break;
		case kipl::base::ImagePlaneYZ : s="imageplaneyz"; break;
		default : s="unknown image plane"; break;
	}
	return s;
}

std::ostream & operator<<(std::ostream & s, const kipl::base::eImagePlanes plane)
{
	s<<enum2string(plane);

	return s;
}

void string2enum(std::string str, kipl::base::eImagePlanes & plane)
{

	if (str=="imageplanexy")      plane=kipl::base::ImagePlaneXY;
	else if (str=="imageplanexz") plane=kipl::base::ImagePlaneXY;
	else if (str=="imageplaneyz") plane=kipl::base::ImagePlaneYZ ;
	else throw kipl::base::KiplException("Unknown plane type", __FILE__, __LINE__);
}


std::ostream &operator<<(std::ostream & s, kipl::base::eImageFlip flip)
{
	switch (flip) {
    case kipl::base::ImageFlipNone:               s<<"imageflipnone"; break;
    case kipl::base::ImageFlipHorizontal:         s<<"imagefliphorizontal"; break;
    case kipl::base::ImageFlipVertical:           s<<"imageflipvertical"; break;
	case kipl::base::ImageFlipHorizontalVertical: s<<"imagefliphorizontalvertical"; break;
    case kipl::base::ImageFlipDefault:            s<<"imageflipdefault"; break;
	default: throw kipl::base::KiplException("Unknown flip type", __FILE__, __LINE__);
	}

	return s;
}

std::ostream &operator<<(std::ostream & s, kipl::base::eImageRotate rot)
{
	switch (rot) {
	case kipl::base::ImageRotateNone: s<<"imagerotatenone"; break;
	case kipl::base::ImageRotate90: s<<"imagerotate90"; break;
	case kipl::base::ImageRotate180: s<<"imagerotate180"; break;
	case kipl::base::ImageRotate270: s<<"imagerotate270"; break;
    case kipl::base::ImageRotateDefault: s<<"imagerotatedefault"; break;
	default: throw kipl::base::KiplException("Unknown rotate type", __FILE__, __LINE__);
	}

	return s;
}

void  string2enum(std::string str, kipl::base::eImageFlip &flip)
{
	if      (str=="imageflipnone")       flip=kipl::base::ImageFlipNone ;
	else if (str=="imagefliphorizontal") flip=kipl::base::ImageFlipHorizontal;
	else if (str=="imageflipvertical")   flip=kipl::base::ImageFlipVertical;
	else if (str=="imagefliphorizontalvertical") flip=kipl::base::ImageFlipHorizontalVertical;
    else if (str=="imageflipdefault")    flip=kipl::base::ImageFlipDefault;
	else throw kipl::base::KiplException("Could not transform string to flip type", __FILE__, __LINE__);

}

void  string2enum(std::string str, kipl::base::eImageRotate &rot)
{
    std::transform(str.begin(),str.end(),str.begin(),::tolower);
    if (str=="imagerotatenone") rot=kipl::base::ImageRotateNone;
    else if (str=="imagerotate90") rot=kipl::base::ImageRotate90;
    else if (str=="imagerotate180") rot=kipl::base::ImageRotate180;
    else if (str=="imagerotate270") rot=kipl::base::ImageRotate270;
    else if (str=="imagerotatedefault") rot=kipl::base::ImageRotateDefault;
	else throw kipl::base::KiplException("Could not transform string to rotation type", __FILE__, __LINE__);
}

std::string enum2string(kipl::base::eDataType dt)
{
    std::string s;
    switch (dt) {
        case kipl::base::UInt4 :  s="UInt4"; break;
        case kipl::base::UInt8 :  s="UInt8"; break;
        case kipl::base::UInt12 : s="UInt12"; break;
        case kipl::base::UInt16 : s="UInt16"; break;
        default : s="Unknown data type"; break;
    }

    return s;
}

void  string2enum(std::string str, kipl::base::eDataType &dt)
{
    if (str=="UInt4") dt=kipl::base::UInt4;
    else if (str=="UInt8") dt=kipl::base::UInt8;
    else if (str=="UInt12") dt=kipl::base::UInt12;
    else if (str=="UInt16") dt=kipl::base::UInt16;
    else throw kipl::base::KiplException("Could not transform string to data type enum", __FILE__, __LINE__);
}

std::ostream &operator<<(std::ostream & s, kipl::base::eDataType dt)
{
    s<<enum2string(dt);

    return s;
}

std::string enum2string(kipl::base::eEndians endian)
{
    std::string s;
    switch (endian) {
        case kipl::base::SmallEndian :  s="SmallEndian"; break;
        case kipl::base::BigEndian : s="BigEndian"; break;
        default : s="Unknown data type"; break;
    }

    return s;
}

void  string2enum(std::string str, kipl::base::eEndians &endian)
{
    if (str=="SmallEndian") endian=kipl::base::SmallEndian;
    else if (str=="BigEndian") endian=kipl::base::BigEndian;
    else throw kipl::base::KiplException("Could not transform string to endian enum", __FILE__, __LINE__);
}

std::ostream &operator<<(std::ostream & s, kipl::base::eEndians endian)
{
    s<<enum2string(endian);

    return s;
}

// --------------------

std::string enum2string(kipl::base::eAxisPermutations perm)
{
    std::string s;
    switch (perm) {
        case kipl::base::PermuteXYZ : s="PermuteXYZ"; break;
        case kipl::base::PermuteXZY : s="PermuteXZY"; break;
        case kipl::base::PermuteYXZ : s="PermuteYXZ"; break;
        case kipl::base::PermuteYZX : s="PermuteYZX"; break;
        case kipl::base::PermuteZXY : s="PermuteZXY"; break;
        case kipl::base::PermuteZYX : s="PermuteZYX"; break;
        default : throw kipl::base::KiplException("Could not transform axis permutation enum to a string", __FILE__, __LINE__); break;
    }

    return s;
}

void  string2enum(std::string str, kipl::base::eAxisPermutations &perm)
{
    std::map<std::string, kipl::base::eAxisPermutations> values;
    values["PermuteXYZ"] = kipl::base::PermuteXYZ;
    values["PermuteXZY"] = kipl::base::PermuteXZY;
    values["PermuteYXZ"] = kipl::base::PermuteYXZ;
    values["PermuteYZX"] = kipl::base::PermuteYZX;
    values["PermuteZXY"] = kipl::base::PermuteZXY;
    values["PermuteZXY"] = kipl::base::PermuteZYX;

    auto it=values.find(str);

    if (it!=values.end())
        perm=it->second;
    else
        throw kipl::base::KiplException("Could not transform string to axis permutation enum", __FILE__, __LINE__);
}

std::ostream &operator<<(std::ostream & s, kipl::base::eAxisPermutations perm)
{
    s<<enum2string(perm);

    return s;
}

/// \brief Converts a rotation direction enum to a string
/// \param rotdir the enum value to convert
/// \note You can also enter an integer number
/// \returns A string with the permutation type
KIPLSHARED_EXPORT std::string enum2string(kipl::base::eRotationDirection rotdir)
{
    std::string str;
    switch (rotdir) {
    case kipl::base::RotationDirCW:     str="RotationDirCW"; break;
    case kipl::base::RotationDirCCW:    str="RotationDirCCW"; break;
    default:                            throw kipl::base::KiplException("Could not transform rotation direction enum to a string", __FILE__, __LINE__); break;
    }

    return str;
}

/// \brief Converts a string to a rotation direction enum
/// \param str a string containing the name to convert
/// \param perm the enum result
KIPLSHARED_EXPORT void  string2enum(std::string str, kipl::base::eRotationDirection &rotdir)
{
    std::map<std::string, kipl::base::eRotationDirection> values;
    values["cw"]  = kipl::base::RotationDirCW;
    values["ccw"] = kipl::base::RotationDirCCW;
    values["CW"]  = kipl::base::RotationDirCW;
    values["CCW"] = kipl::base::RotationDirCCW;
    values["RotationDirCW"]  = kipl::base::RotationDirCW;
    values["RotationDirCCW"] = kipl::base::RotationDirCCW;

    auto it=values.find(str);

    if (it!=values.end())
        rotdir=it->second;
    else
    {   
        std::ostringstream msg;
        msg<<"Could not transform string "<<str<<" to rotation dir enum";
        throw kipl::base::KiplException(msg.str(), __FILE__, __LINE__);
    }
}

/// \brief Stream output operator for eRotationDirection
/// \param s the stream that handles the enum
/// \param rotdir the value to send to the stream
/// \returns a reference to the stream
KIPLSHARED_EXPORT std::ostream &operator<<(std::ostream & s, kipl::base::eRotationDirection rotdir)
{
    s<<enum2string(rotdir);

    return s;
}

/// \brief Converts connectivity enum value to a string
/// \param conn the enum value to convert
/// \note You can also enter an integer number
/// \returns A string with the name of the connectivity type
KIPLSHARED_EXPORT std::string enum2string(kipl::base::eConnectivity conn)
{
    std::string str;
    switch (conn) {
    case kipl::base::conn4   : str = "conn4"; break;
    case kipl::base::conn8  : str = "conn8"; break;
    case kipl::base::conn6  : str = "conn6"; break;
    case kipl::base::conn18 : str = "conn18"; break;
    case kipl::base::conn26 : str = "conn26"; break;

    default :
        throw kipl::base::KiplException("Could not transform a Connectivity enum to a string", __FILE__, __LINE__);
    };

    return str;
}

/// \brief Converts a string to a connectivity enum
/// \param str a string containing the name to convert
/// \param conn the enum result
KIPLSHARED_EXPORT void  string2enum(std::string str, kipl::base::eConnectivity & conn)
{
    std::map<std::string, kipl::base::eConnectivity> values;

    values["conn4"] = kipl::base::conn4;
    values["conn8"] = kipl::base::conn8;
    values["conn6"] = kipl::base::conn6;
    values["conn18"] = kipl::base::conn18;
    values["conn26"] = kipl::base::conn26;

    auto it=values.find(str);

    if (it!=values.end())
        conn=it->second;
    else
        throw kipl::base::KiplException("Could not transform string to an Connectivity enum", __FILE__, __LINE__);
}



/// \brief Stream output operator for eConnectivity
/// \param s the stream that handles the enum
/// \param rotdir the value to send to the stream
/// \returns a reference to the stream
KIPLSHARED_EXPORT std::ostream &operator<<(std::ostream & s, kipl::base::eConnectivity conn)
{
    s<<enum2string(conn);
    return s;

}


/// \brief Converts edge status enum value to a string
/// \param conn the enum value to convert
/// \note You can also enter an integer number
/// \returns A string with the name of the edge type
KIPLSHARED_EXPORT std::string enum2string(kipl::base::eEdgeStatus edge)
{
    std::string str;

    switch (edge) {
        case kipl::base::noEdge       : str="noEdge"; break;
        case kipl::base::edgeX0       : str="edgeX0"; break;
        case kipl::base::edgeX1       : str="edgeX1"; break;
        case kipl::base::edgeY0       : str="edgeY0"; break;
        case kipl::base::edgeY1       : str="edgeY1"; break;
        case kipl::base::cornerX0Y0   : str="cornerX0Y0"; break;
        case kipl::base::cornerX1Y0   : str="cornerX1Y0"; break;
        case kipl::base::cornerX0Y1   : str="cornerX0Y1"; break;
        case kipl::base::cornerX1Y1   : str="cornerX1Y1"; break;
        case kipl::base::edgeZ0       : str="edgeZ0"; break;
        case kipl::base::edgeZ1       : str="edgeZ1"; break;
        case kipl::base::cornerX0Y0Z0 : str="cornerX0Y0Z0"; break;
        case kipl::base::cornerX1Y0Z0 : str="cornerX1Y0Z0"; break;
        case kipl::base::cornerX0Y1Z0 : str="cornerX0Y1Z0"; break;
        case kipl::base::cornerX1Y1Z0 : str="cornerX1Y1Z0"; break;
        case kipl::base::cornerX0Y0Z1 : str="cornerX0Y0Z1"; break;
        case kipl::base::cornerX1Y0Z1 : str="cornerX1Y0Z1"; break;
        case kipl::base::cornerX0Y1Z1 : str="cornerX0Y1Z1"; break;
        case kipl::base::cornerX1Y1Z1 : str="cornerX1Y1Z1"; break;
        case kipl::base::cornerX0Z0   : str="cornerX0Z0"; break;
        case kipl::base::cornerX0Z1   : str="cornerX0Z1"; break;
        case kipl::base::cornerX1Z0   : str="cornerX1Z0"; break;
        case kipl::base::cornerX1Z1   : str="cornerX1Z1"; break;
        case kipl::base::cornerY0Z0   : str="cornerY0Z0"; break;
        case kipl::base::cornerY0Z1   : str="cornerY0Z1"; break;
        case kipl::base::cornerY1Z0   : str="cornerY1Z0"; break;
        case kipl::base::cornerY1Z1   : str="cornerY1Z1"; break;

    default :
        throw kipl::base::KiplException("Could not transform a edge status enum to a string", __FILE__, __LINE__);
    }
    return str;
}

/// \brief Converts a string to a edge status enum
/// \param str a string containing the name to convert
/// \param edge the enum result
KIPLSHARED_EXPORT void  string2enum(std::string str, kipl::base::eEdgeStatus & edge)
{
    std::map<std::string, kipl::base::eEdgeStatus> values;

    values["edgeX0"] = kipl::base::edgeX0;
    values["edgeX1"] = kipl::base::edgeX1;
    values["edgeY0"] = kipl::base::edgeY0;
    values["edgeY1"] = kipl::base::edgeY1;
    values["edgeZ0"] = kipl::base::edgeZ0;
    values["edgeZ1"] = kipl::base::edgeZ1;
    values["cornerX0Y0"] = kipl::base::cornerX0Y0;
    values["cornerX1Y0"] = kipl::base::cornerX1Y0;
    values["cornerX0Y1"] = kipl::base::cornerX0Y1;
    values["cornerX1Y1"] = kipl::base::cornerX1Y1;
    values["cornerX0Z0"] = kipl::base::cornerX0Z0;
    values["cornerX1Z0"] = kipl::base::cornerX1Z0;
    values["cornerX0Z1"] = kipl::base::cornerX0Z1;
    values["cornerX1Z1"] = kipl::base::cornerX1Z1;
    values["cornerY0Z0"] = kipl::base::cornerY0Z0;
    values["cornerY0Z0"] = kipl::base::cornerY1Z0;
    values["cornerY1Z1"] = kipl::base::cornerY0Z1;
    values["cornerY1Z1"] = kipl::base::cornerY1Z1;
    values["cornerX0Y0Z0"] = kipl::base::cornerX0Y0Z0;
    values["cornerX1Y0Z0"] = kipl::base::cornerX1Y0Z0;
    values["cornerX0Y1Z0"] = kipl::base::cornerX0Y1Z0;
    values["cornerX1Y1Z0"] = kipl::base::cornerX1Y1Z0;
    values["cornerX0Y0Z1"] = kipl::base::cornerX0Y0Z1 ;
    values["cornerX1Y0Z1"] = kipl::base::cornerX1Y0Z1 ;
    values["cornerX0Y1Z1"] = kipl::base::cornerX0Y1Z1 ;
    values["cornerX1Y1Z1"] = kipl::base::cornerX1Y1Z1 ;

    auto it=values.find(str);

    if (it!=values.end())
        edge=it->second;
    else
        throw kipl::base::KiplException("Could not transform string to an edge status enum", __FILE__, __LINE__);
}

/// \brief Stream output operator for eEdgeStatus
/// \param s the stream that handles the enum
/// \param edge the value to send to the stream
/// \returns a reference to the stream
KIPLSHARED_EXPORT std::ostream &operator<<(std::ostream & s, kipl::base::eEdgeStatus edge)
{
    s<<enum2string(edge);

    return s;
}

std::string enum2string(kipl::base::eImageRotate &rot)
{
    switch (rot) {
    case kipl::base::ImageRotateNone:    return "imagerotatenone";
    case kipl::base::ImageRotate90:      return "imagerotate90";
    case kipl::base::ImageRotate180:     return "imagerotate180";
    case kipl::base::ImageRotate270:     return "imagerotate270";
    case kipl::base::ImageRotateDefault: return "imagerotatedefault";
    default: throw kipl::base::KiplException("Unknown rotate type", __FILE__, __LINE__);
    }

    return "imagerotatenone";
}

std::string enum2string(kipl::base::eImageFlip flip)
{
    switch (flip) {
    case kipl::base::ImageFlipNone:               return "imageflipnone";
    case kipl::base::ImageFlipHorizontal:         return "imagefliphorizontal";
    case kipl::base::ImageFlipVertical:           return "imageflipvertical";
    case kipl::base::ImageFlipHorizontalVertical: return "imagefliphorizontalvertical";
    case kipl::base::ImageFlipDefault:            return "imageflipdefault";
    default: throw kipl::base::KiplException("Unknown rotate type", __FILE__, __LINE__);
    }

    return "imageflipnone";
}

std::ostream &operator<<(std::ostream &s, kipl::base::eOperatingSystem &os)
{
    s<<enum2string(os);

    return s;
}

std::string enum2string(const kipl::base::eOperatingSystem &os)
{
   switch (os) {
   case kipl::base::OSUnknown     : return "OSUnknown";
       case kipl::base::OSWindows : return "OSWindows";
       case kipl::base::OSMacOS   : return "OSMacOS";
       case kipl::base::OSLinux   : return "OSLinux";
       default : throw kipl::base::KiplException("Failed to convert OS enum");
   }

   return "OSUnknown";
}

void string2enum(const std::string &str, kipl::base::eOperatingSystem &os)
{
    std::map<std::string,kipl::base::eOperatingSystem> osMap;

    osMap["OSUnknown"] = kipl::base::OSUnknown;
    osMap["OSWindows"] = kipl::base::OSWindows;
    osMap["OSMacOS"]   = kipl::base::OSMacOS;
    osMap["OSLinux"]   = kipl::base::OSLinux;

    auto it=osMap.find(str);

    if (it!=osMap.end())
        os=it->second;
    else
        throw kipl::base::KiplException("Could not transform string to an operating system enum", __FILE__, __LINE__);
}



std::ostream & operator<<(std::ostream & s, kipl::base::eThreadingMethod &tm)
{
    s<<enum2string(tm);

    return s;
}

std::string enum2string(const kipl::base::eThreadingMethod &tm)
{
    std::map<kipl::base::eThreadingMethod, std::string> tmMap;

    tmMap[kipl::base::threadingSTL]    = "threadingSTL";
    tmMap[kipl::base::threadingOpenMP] = "threadingOpenMP";

    try
    {
        return tmMap.at(tm);
    }
    catch (std::out_of_range &e)
    {
        throw kipl::base::KiplException("Could not transform string to a threading method enum", __FILE__, __LINE__);
    }

    return "fail";
}

void  string2enum(const std::string &str, kipl::base::eThreadingMethod &tm)
{
    std::map<std::string,kipl::base::eThreadingMethod> tmMap;

    tmMap["threadingSTL"]    = kipl::base::threadingSTL;
    tmMap["threadingOpenMP"] = kipl::base::threadingOpenMP;

    try
    {
        tm=tmMap.at(str);
    }
    catch (std::out_of_range &e)
    {
        throw kipl::base::KiplException("Could not transform string to a threading method enum", __FILE__, __LINE__);
    }



}
