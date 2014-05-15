//
// This file is part of the i KIPL image processing library by Anders Kaestner
// (c) 2008 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author: kaestner $
// $Date: 2008-11-11 21:09:49 +0100 (Di, 11 Nov 2008) $
// $Rev: 13 $
//

#include <sstream>
#include <string>
#include "../../include/io/io_stack.h"
#include "../../include/base/KiplException.h"

namespace kipl { namespace io {
	std::ostream & operator<<(std::ostream &s, kipl::io::eFileType ft)
{
	switch (ft) {
	case kipl::io::MatlabVolume : s<<"MatlabVolume"; break;
	case kipl::io::MatlabSlices : s<<"MatlabSlices"; break;
	case kipl::io::TIFF8bits    : s<<"TIFF8bits";    break;
	case kipl::io::TIFF16bits   : s<<"TIFF16bits";   break;
	case kipl::io::TIFFfloat    : s<<"TIFFfloat";    break;
	case kipl::io::PNG8bits     : s<<"PNG8bits";     break;
	case kipl::io::PNG16bits    : s<<"PNG16bits";    break;
	default : throw kipl::base::KiplException("Unknown matrix file type",__FILE__,__LINE__); break;
	}

	return s;
}

void string2enum(const std::string str, kipl::io::eFileType &ft)
{
	if      (str=="MatlabVolume") ft=kipl::io::MatlabVolume;
	else if (str=="MatlabSlices") ft=kipl::io::MatlabSlices;
	else if (str=="TIFF8bits")    ft=kipl::io::TIFF8bits;
	else if (str=="TIFF16bits")   ft=kipl::io::TIFF16bits; 
	else if (str=="TIFFfloat")    ft=kipl::io::TIFFfloat;
	else if (str=="PNG8bits")     ft=kipl::io::PNG8bits;
	else if (str=="PNG16bits")    ft=kipl::io::PNG16bits;
	else throw kipl::base::KiplException("Unknow matrix file type string",__FILE__,__LINE__);
	 
}
}}
