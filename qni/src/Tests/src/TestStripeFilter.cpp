#include <base/timage.h>
#include <io/io_tiff.h>
#include <StripeFilter.h>

int TestStripeFilter(int argc, char *argv[])
{
	cout<<"Testing StripeFilter."<<endl;
	int nlevels=2;

	string fname, outname;
	switch (argc) {
		case 1: cout<<"Missing input\n"; return -1;
		case 2: fname = argv[1]; break;
		case 3: fname = argv[1]; nlevels=atoi(argv[2]);break;
	}

	kipl::base::TImage<float,2> img, result;
	kipl::io::ReadTIFF(img,fname.c_str());

	ImagingAlgorithms::StripeFilter sf(img.Dims(), "daub25", nlevels, 0.05f);

	result=img;
	result.Clone();
	sf.Process(result);

	result=img;
	result.Clone();
	sf.Process(result);

	kipl::io::WriteTIFF(result,"result.tif",0.0f, 65535.0f);
	result-=img;

	kipl::io::WriteTIFF32(result,"diff.tif");


	system("kipviewer result.tif &");
	system("kipviewer diff.tif &");
	return 0;
}
