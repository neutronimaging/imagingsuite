/*
 * io_vtk.h
 *
 *  Created on: Sep 13, 2012
 *      Author: anders
 */

#ifndef IO_VTK_H_
#define IO_VTK_H_

#include "../base/imagecast.h"
#include "../base/timage.h"
#include "../strings/filenames.h"
#include "../base/kiplenums.h"
#include <iostream>
#include <iomanip>
#include <cstdio>
#include <cstring>
#include <sstream>
#include <fstream>
#ifdef _MSC_VER
#include <stdint.h>
#else
#include <inttypes.h>
#endif
namespace kipl { namespace io {


std::string VTK_datatype_string(float x) {return "float";}
std::string VTK_datatype_string(double x) {return "double";}
std::string VTK_datatype_string(int8_t x) {return "char";}
std::string VTK_datatype_string(uint8_t x) {return "unsigned_char";}
std::string VTK_datatype_string(int16_t x) {return "short";}
std::string VTK_datatype_string(uint16_t x) {return "unsigned_short";}
std::string VTK_datatype_string(int32_t x) {return "int";}
std::string VTK_datatype_string(uint32_t x) {return "unsigned_int";}
std::string VTK_datatype_string(int64_t x) {return "long";}
std::string VTK_datatype_string(uint64_t x) {return "unsigned_long";}


std::string BuildVTKheader(size_t const * const dims, size_t ndims, float const * const res, std::string datatype, std::string dataname)
{
	std::ostringstream header;

	header<<"# vtk DataFile Version3.0\n";
	header<<dataname<<"\n";
	header<<"BINARY\nDATASET STRUCTURED_POINTS\n";
	header<<"DIMENSIONS ";
	size_t ndata=1;
	for (size_t i=0; i<ndims; i++) {
		ndata*=dims[i];
		header<<dims[i]<<" ";
	}
	header<<"\nSPACING ";

	if (res!=NULL) {
		for (size_t i=0; i<ndims; i++) {
			header<<res[i]<<" ";
		}
	}
	else {
		for (size_t i=0; i<ndims; i++) {
			header<<"1.0"<<" ";
		}
	}
	header<<"\n";
	header<<"ORIGIN ";
	for (size_t i=0; i<ndims; i++) {
		header<<"0.0"<<" ";
	}
	header<<"\n";
	header<<"POINT_DATA "<<ndata<<"\n";
	header<<"SCALARS volume_scalars "<<datatype<<" 1\n";
	header<<"LOOKUP_TABLE default\n";

	return header.str();
}


template <class T, size_t N>
int WriteVTK(kipl::base::TImage<T,N> src,std::string fname, std::string dataname="volume") {
	float res[]={src.info.GetMetricX(),src.info.GetMetricX(),src.info.GetMetricX(), src.info.GetMetricX(),src.info.GetMetricX()};
	std::string header=BuildVTKheader(src.Dims(), N, res, VTK_datatype_string(T(0)),dataname);
	std::ofstream imgfile(fname.c_str(),ios::binary  | ios::out);

	imgfile.write(header.c_str(),header.size());

	imgfile.write(reinterpret_cast<char *>(src.GetDataPtr()),src.Size()*sizeof(T));

	return 0;
}


}}


#endif /* IO_VTK_H_ */
