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


#include <iostream>
#include <sstream>
#include <string>

#include "fwt2d_padded.hpp"
//#include "fwt2d.hpp"
#include "vec1d.hpp"
#include "wavelet_utils.hpp"

#include <base/timage.h>
#include <io/io_tiff.h>


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
//      wt.transform(data,3,0);
      wt.transform(data,3);
      wt.synthesize(data);
   }
   time.stop();
   std::cout << type_name<< "[" << width << "," << height <<"] Rate: " << (1.0 * frame_count) / time.time() << "fps" << std::endl;
   delete [] data;
}

void testkipl(int n, int p)
{
	kipl::base::TImage<float,2> img,spec;
	//kipl::io::ReadTIFF(img,"tablet_0006.tif");
	kipl::io::ReadTIFF(img,"sino_0010.tif");
	//kipl::io::ReadTIFF(img,"sine.tif");

	spec.Resize(img.Dims());

	fwt2d<float> wt("filters/daub10.flt");
	wt.init(img.Size(0),img.Size(1));
	wt.padtype=static_cast<ePadType>(p);

	wt.transform(img.GetDataPtr(),n);

	memcpy(spec.GetDataPtr(),wt.spectrum(),spec.Size()*sizeof(float));
        std::ostringstream fname;
	fname<<"spec_N"<<n<<".tif";
	kipl::io::WriteTIFF32(spec,fname.str().c_str());

	wt.synthesize(img.GetDataPtr());


	fname.str("");
	fname<<"result_N"<<n<<".tif";
	kipl::io::WriteTIFF32(img,fname.str().c_str());
}

void testpad()
{
/*
	fwt2d<float> wt("filters/daub10.flt");

	float src[256];
	float dst[256];

	int N=20;
	for (int i=0; i<N; i++)
		src[i]=sin(2*3.14f*i/N);

	int M=5;
	wt.pad_periodic(src, N, dst, M);
	wt.save_array(dst, N+2*M, "arr.txt");
*/
}

int main(int argc, char *argv[])
{
//	testpad();


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
