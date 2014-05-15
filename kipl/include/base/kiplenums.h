//
// This file is part of the i KIPL image processing library by Anders Kaestner
// (c) 2008 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author$
// $Date$
// $Rev$
//

#ifndef KIPLENUMS_H_
#define KIPLENUMS_H_
#include <iostream>

namespace kipl { namespace base {

enum eImagePlanes {
	ImagePlaneXY = 1,
	ImagePlaneXZ = 2,
	ImagePlaneYZ = 4
};

enum eImageAxes {
	ImageAxisX,
	ImageAxisY,
	ImageAxisZ
};

enum ePadType {
	PadMirror=0,
	PadPeriodic,
	PadZero,
	PadSP0,
	PadSP1
};

enum eImageFlip {
	ImageFlipNone=0,
	ImageFlipHorizontal,
	ImageFlipVertical,
	ImageFlipHorizontalVertical
};

enum eImageRotate {
	ImageRotateNone=0,
	ImageRotate90,
	ImageRotate180,
	ImageRotate270
};


}}

std::ostream & operator<<(std::ostream & s, kipl::base::eImagePlanes plane);

std::string enum2string(kipl::base::eImagePlanes plane);
void  string2enum(std::string str, kipl::base::eImagePlanes &plane);

std::ostream &operator<<(std::ostream & s, kipl::base::eImageFlip flip);
std::ostream &operator<<(std::ostream & s, kipl::base::eImageRotate rot);

void  string2enum(std::string str, kipl::base::eImageFlip &flip);
void  string2enum(std::string str, kipl::base::eImageRotate &rot);


#endif /*KIPLENUMS_H_*/
