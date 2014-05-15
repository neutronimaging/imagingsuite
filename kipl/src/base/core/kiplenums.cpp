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



