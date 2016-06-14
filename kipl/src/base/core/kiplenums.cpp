#include "../../../include/base/kiplenums.h"
#include "../../../include/base/KiplException.h"
#include <iostream>

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
	case kipl::base::ImageFlipNone: s<<"imageflipnone"; break;
	case kipl::base::ImageFlipHorizontal: s<<"imagefliphorizontal"; break;
	case kipl::base::ImageFlipVertical: s<<"imageflipvertical"; break;
	case kipl::base::ImageFlipHorizontalVertical: s<<"imagefliphorizontalvertical"; break;
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
	else throw kipl::base::KiplException("Could not transform string to flip type", __FILE__, __LINE__);

}

void  string2enum(std::string str, kipl::base::eImageRotate &rot)
{
	if (str=="imagerotatenone") rot=kipl::base::ImageRotateNone;
	else if (str=="imagerotate90") rot=kipl::base::ImageRotate90;
	else if (str=="imagerotate180") rot=kipl::base::ImageRotate180;
	else if (str=="imagerotate270") rot=kipl::base::ImageRotate270;
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
    switch (endian) {
        case kipl::base::PermuteXYZ :perm="PermuteXYZ"; break;
        case kipl::base::PermuteXZY :perm="PermuteXZY"; break;
        case kipl::base::PermuteYXZ :perm="PermuteYXZ"; break;
        case kipl::base::PermuteYZX :perm="PermuteYZX"; break;
        case kipl::base::PermuteZXY :perm="PermuteZXY"; break;
        case kipl::base::PermuteZYX :perm="PermuteZYX"; break;
        default : kipl::base::KiplException("Could not transform axis permutation enum to a string", __FILE__, __LINE__); break;
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

    auto it=values.find(s);

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
