/*
(***************************************************************************)
(*                                                                         *)
(*                     Fast Wavelet Transform Library                      *)
(*                                                                         *)
(* Release Version 0.0.1                                                   *)
(* Copyright (c) 2007-2008 Yuriy Chesnokov                                 *)
(* Copyright (c) 2008 Arash Partow                                         *)
(* All Rights Reserved.                                                    *)
(*                                                                         *)
(* The Fast Wavelet Transform Library and all its components are supplied  *)
(* under the terms of the General Public License. The contents of the      *)
(* Fast Wavelet Transform Library and all its components may not be copied *)
(* or disclosed except in accordance with the terms of that agreement.     *)
(*                                                                         *)
(* URL: http://www.gnu.org/licenses/gpl.txt                                *)
(*                                                                         *)
(***************************************************************************)
*/


#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "../libwavelet/fwt2d_padded.hpp"
//#include "../libwavelet/fwt2d.hpp"
#include "../libwavelet/vec1d.hpp"
#include "../libwavelet/wavelet_utils.hpp"
#include <wavelets/wavelets.h>

#include <base/timage.h>
#include <io/io_tiff.h>

using namespace std;

template<typename T>
void test(const std::string& type_name,
          const unsigned int& width,
          const unsigned int& height)
{
   const unsigned int frame_count = 10;
   fwt2d<T> wt("filters/daub10.flt");
   wt.init(width,height);
   T* data = new T[width * height];
   timer time;
   time.start();
   for(std::size_t i = 0; i < frame_count; ++i)
   {
      wt.transform(data,3);
      wt.synthesize(data);
   }
   time.stop();
   std::cout << type_name<< "[" << width << "," << height <<"] Rate: " << (1.0 * frame_count) / time.time() << "fps" << std::endl;
   delete [] data;
}

void testlw(int n, int p)
{
	kipl::base::TImage<float,2> img,spec;
	//kipl::io::ReadTIFF(img,"tablet_0006.tif");
	kipl::io::ReadTIFF(img,"sino_0010.tif");
	//kipl::io::ReadTIFF(img,"sine.tif");

	spec.Resize(img.Dims());

	fwt2d<float> wt("filters/daub10.flt");
	wt.init(img.Size(0),img.Size(1));
#ifdef HAVE_FWT_PADDING
	wt.padtype=static_cast<ePadType>(p);
#endif

	wt.transform(img.GetDataPtr(),n);

	memcpy(spec.GetDataPtr(),wt.spectrum(),spec.Size()*sizeof(float));
        std::ostringstream fname;
	fname<<"spec_N"<<n<<"_"<<p<<".tif";
	kipl::io::WriteTIFF32(spec,fname.str().c_str());

	wt.synthesize(img.GetDataPtr());

	fname.str("");
	fname<<"result_N"<<n<<"_"<<p<<".tif";
	kipl::io::WriteTIFF32(img,fname.str().c_str());
}

void testkipl(int n, int p)
{
	kipl::base::TImage<float,2> img,spec;
	//kipl::io::ReadTIFF(img,"tablet_0006.tif");
	kipl::io::ReadTIFF(img,"sino_0010.tif");
	//kipl::io::ReadTIFF(img,"sine.tif");


	kipl::wavelets::WaveletTransform<float> wt("daub10");

	wt.padtype=static_cast<kipl::base::ePadType>(p);

	wt.transform(img,n);

	std::ostringstream fname;
	fname<<"spec_N"<<n<<"_"<<p<<".tif";

	wt.SaveSpectrum(fname.str());
	//kipl::io::WriteTIFF32(spec,fname.str().c_str());

	img=wt.synthesize();


	fname.str("");
	fname<<"result_N"<<n<<"_"<<p<<".tif";
	kipl::io::WriteTIFF32(img,fname.str().c_str());
}

void testpad(int cnt)
{

	kipl::wavelets::WaveletTransform<float> wt("daub10");

	float src[256];
	float dst[256];

	int N=cnt;
	for (int i=0; i<N; i++)
		src[i]=static_cast<float>(i);

	int M=5;
	wt.pad_mirror(src, N, dst, M);

	ostringstream fname;
	fname<<"padded_"<<N<<".txt";
	ofstream of(fname.str().c_str());
	for (int i=0; i<(N+2*M+(N & 1)); i++)
		of<<dst[i]<<endl;
}

int main(int argc, char *argv[])
{
//	testpad(20);
//	testpad(21);


	if (argc!=1) {
		int n=atoi(argv[1]);
		int p= argc<3 ? 0 : atoi(argv[2]);
		testkipl(n,p);
	}
	else

		testkipl(3,0);
/*
   test<float>("float",640,480);
   test<float>("float",1024,1024);
   test<double>("double",640,480);
   test<double>("double",1024,1024);
*/
   return 0;
}
