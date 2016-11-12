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
#include "../kipl_global.h"

#include <iostream>



namespace kipl { namespace base {
/// \brief Image plane selection enum
enum eImagePlanes {
    ImagePlaneXY = 1, ///< selects the XY plane
    ImagePlaneXZ = 2, ///< selects the XZ plane
    ImagePlaneYZ = 4  ///< selects the YZ plane
};

/// \brief Image axis selection enum
enum eImageAxes {
    ImageAxisX, ///< selects the X-axis
    ImageAxisY, ///< selects the Y-axis
    ImageAxisZ  ///< selects the Z-axis
};

/// \brief Filter edge processing method selector
enum ePadType {
    PadMirror=0,    ///< Mirrors the edge information
    PadPeriodic,    ///< Repeats the images at the edge
    PadZero,        ///< Pads the edge by zeros
    PadSP0,         ///< Edge Padding for the wavelet transform
    PadSP1          ///< Edge Padding for the wavelet transform
};

/// \brief Image flip selector
enum eImageFlip {
    ImageFlipNone=0,            ///< No flipping
    ImageFlipHorizontal,        ///< Flip about the vertical axis
    ImageFlipVertical,          ///< Flip about the horizontal axis
    ImageFlipHorizontalVertical ///< Flip both horizontal and vertial
};

/// \brief Image axis permutation selector
enum eAxisPermutations {
        PermuteXYZ,
        PermuteXZY,
        PermuteYXZ,
        PermuteYZX,
        PermuteZXY,
        PermuteZYX
};

/// \brief Image rotation selector
enum eImageRotate {
    ImageRotateNone=0,  ///< No rotation
    ImageRotate90,      ///< Rotate 90 degrees clockwise
    ImageRotate180,     ///< Rotate 180 degrees clockwise
    ImageRotate270      ///< Rotate 270 degrees clockwise
};

///< Selector for the rotation direction
enum eRotationDirection {
    RotationDirCW = 0,  ///< Rotation clock wise
    RotationDirCCW      ///< Rotation counter clock wise
};

/// \brief Unsigned integer data type selector
enum eDataType {
    UInt4,      ///< 4bits unsigned integer
    UInt8,      ///< 8bits unsigned integer
    UInt12,     ///< 12bits unsigned integer
    UInt16      ///< 16bits unsigned integer
};

/// \brief Endian selector
enum eEndians {
    SmallEndian,    ///< Use small endian, ie first bit is least significant
    BigEndian       ///< Use big endian, ie first bit is most significant
};

/// \brief selector for neighborhood connectivity
enum eConnectivity {
    conn4=4,
    conn8=8,
    conn6=6,
    conn18=18,
    conn26=26,
    euclid=0
};

enum eEdgeStatus {
    noEdge = 0,
    edgeX0 = 1,
    edgeX1 = 2,
    edgeY0 = 4,
    cornerX0Y0 = edgeX0+edgeY0,//1+4,
    cornerX1Y0 = edgeX1+edgeY0,//2+4,
    edgeY1 = 8,
    cornerX0Y1 = edgeX0+edgeY1,//1+8,
    cornerX1Y1 = edgeX1+edgeY1,//2+8,
    edgeZ0 = 16,
    edgeZ1 = 32
};

}}

/// \brief Stream output operator for eImagePlanes
/// \param s the stream that handles the enum
/// \param plane the value to send to the stream
/// \returns a reference to the stream
KIPLSHARED_EXPORT std::ostream & operator<<(std::ostream & s, kipl::base::eImagePlanes plane);

/// \brief Converts an image plane enum to a string
/// \param plane the enum value to convert
/// \note You can also enter an integer number
/// \returns A string with the image plane name
KIPLSHARED_EXPORT std::string enum2string(kipl::base::eImagePlanes plane);

/// \brief Converts a string to an image plane enum
/// \param str a string containing the name to convert
/// \param plane the enum result
KIPLSHARED_EXPORT void  string2enum(std::string str, kipl::base::eImagePlanes &plane);

/// \brief Stream output operator for eImageFlip
/// \param s the stream that handles the enum
/// \param flip the value to send to the stream
/// \returns a reference to the stream
KIPLSHARED_EXPORT std::ostream &operator<<(std::ostream & s, kipl::base::eImageFlip flip);

/// \brief Stream output operator for eImageRotate
/// \param s the stream that handles the enum
/// \param rot the value to send to the stream
/// \returns a reference to the stream
KIPLSHARED_EXPORT std::ostream &operator<<(std::ostream & s, kipl::base::eImageRotate rot);

/// \brief Converts a string to an image flip enum
/// \param str a string containing the name to convert
/// \param flip the enum result
KIPLSHARED_EXPORT void  string2enum(std::string str, kipl::base::eImageFlip &flip);

/// \brief Converts a string to an image rotation enum
/// \param str a string containing the name to convert
/// \param rot the enum result
KIPLSHARED_EXPORT void  string2enum(std::string str, kipl::base::eImageRotate &rot);

/// \brief Converts a data type enum to a string
/// \param dt the enum value to convert
/// \note You can also enter an integer number
/// \returns A string with the data type name
KIPLSHARED_EXPORT std::string enum2string(kipl::base::eDataType dt);

/// \brief Converts a string to a data type enum
/// \param str a string containing the name to convert
/// \param dt the enum result
KIPLSHARED_EXPORT void  string2enum(std::string str, kipl::base::eDataType &dt);

/// \brief Stream output operator for eDataType
/// \param s the stream that handles the enum
/// \param dt the value to send to the stream
/// \returns a reference to the stream
KIPLSHARED_EXPORT std::ostream &operator<<(std::ostream & s, kipl::base::eDataType dt);


/// \brief Converts an endian enum to a string
/// \param endian the enum value to convert
/// \note You can also enter an integer number
/// \returns A string with the image plane name
KIPLSHARED_EXPORT std::string enum2string(kipl::base::eEndians endian);

/// \brief Converts a string to an endian enum
/// \param str a string containing the name to convert
/// \param endian the enum result
KIPLSHARED_EXPORT void  string2enum(std::string str, kipl::base::eEndians &endian);

/// \brief Stream output operator for eEndians
/// \param s the stream that handles the enum
/// \param endian the value to send to the stream
/// \returns a reference to the stream
KIPLSHARED_EXPORT std::ostream &operator<<(std::ostream & s, kipl::base::eEndians endian);

/// \brief Converts a permutation enum to a string
/// \param perm the enum value to convert
/// \note You can also enter an integer number
/// \returns A string with the permutation type
KIPLSHARED_EXPORT std::string enum2string(kipl::base::eAxisPermutations perm);

/// \brief Converts a string to a permutation enum
/// \param str a string containing the name to convert
/// \param perm the enum result
KIPLSHARED_EXPORT void  string2enum(std::string str, kipl::base::eAxisPermutations &perm);

/// \brief Stream output operator for eImagePermutation
/// \param s the stream that handles the enum
/// \param perm the value to send to the stream
/// \returns a reference to the stream
KIPLSHARED_EXPORT std::ostream &operator<<(std::ostream & s, kipl::base::eAxisPermutations perm);

/// \brief Converts a rotation direction enum to a string
/// \param rotdir the enum value to convert
/// \note You can also enter an integer number
/// \returns A string with the permutation type
KIPLSHARED_EXPORT std::string enum2string(kipl::base::eRotationDirection rotdir);

/// \brief Converts a string to a rotation direction enum
/// \param str a string containing the name to convert
/// \param perm the enum result
KIPLSHARED_EXPORT void  string2enum(std::string str, kipl::base::eRotationDirection &rotdir);

/// \brief Stream output operator for eRotationDirection
/// \param s the stream that handles the enum
/// \param rotdir the value to send to the stream
/// \returns a reference to the stream
KIPLSHARED_EXPORT std::ostream &operator<<(std::ostream & s, kipl::base::eRotationDirection rotdir);

/// \brief Converts connectivity enum value to a string
/// \param conn the enum value to convert
/// \note You can also enter an integer number
/// \returns A string with the name of the connectivity type
KIPLSHARED_EXPORT std::string enum2string(kipl::base::eConnectivity conn);

/// \brief Converts a string to a connectivity enum
/// \param str a string containing the name to convert
/// \param conn the enum result
KIPLSHARED_EXPORT void  string2enum(std::string str, kipl::base::eConnectivity & conn);

/// \brief Stream output operator for eConnectivity
/// \param s the stream that handles the enum
/// \param rotdir the value to send to the stream
/// \returns a reference to the stream
KIPLSHARED_EXPORT std::ostream &operator<<(std::ostream & s, kipl::base::eConnectivity conn);


/// \brief Converts edge status enum value to a string
/// \param conn the enum value to convert
/// \note You can also enter an integer number
/// \returns A string with the name of the edge type
KIPLSHARED_EXPORT std::string enum2string(kipl::base::eEdgeStatus edge);

/// \brief Converts a string to a edge status enum
/// \param str a string containing the name to convert
/// \param edge the enum result
KIPLSHARED_EXPORT void  string2enum(std::string str, kipl::base::eEdgeStatus & edge);

/// \brief Stream output operator for eEdgeStatus
/// \param s the stream that handles the enum
/// \param edge the value to send to the stream
/// \returns a reference to the stream
KIPLSHARED_EXPORT std::ostream &operator<<(std::ostream & s, kipl::base::eEdgeStatus edge);
#endif /*KIPLENUMS_H_*/
