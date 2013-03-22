/*
 * TestFFT.cpp
 *
 *  Created on: Mar 21, 2013
 *      Author: kaestner
 */
#include <base/timage.h>
#include <io/io_tiff.h>
#include <complex>
#include <fft/fftbase.h>
#include <fft/zeropadding.h>

void FilterVerticalStripes(kipl::base::TImage<float,2> &img);
void GetVerticalLine(float *pSrc, float *pLine, size_t pos, size_t stride, size_t len);
void SetVerticalLine(float *pLine, float *pDest, size_t pos, size_t stride, size_t len);

using namespace std;

int TestFFT(int argc, char *argv[])
{
	cout<<"Testing FFT."<<endl;
	int nlevels=2;

	string fname, outname;
	switch (argc) {
		case 1: cout<<"Missing input\n"; return -1;
		case 2: fname = argv[1]; break;
		case 3: fname = argv[1]; nlevels=atoi(argv[2]);break;
	}

	kipl::base::TImage<float,2> img, result;
	kipl::io::ReadTIFF(img,fname.c_str());

	result=img;
	result.Clone();

	FilterVerticalStripes(result);

	kipl::io::WriteTIFF(result,"result.tif",0.0f, 65535.0f);
	result-=img;

	kipl::io::WriteTIFF32(result,"diff.tif");

	return 0;
}


void FilterVerticalStripes(kipl::base::TImage<float,2> &img)
{

	kipl::math::fft::FFTBaseFloat *fft;
	const size_t nLines=img.Size(0);
	const size_t nFFTlength=kipl::math::fft::NextPower2(1.25*img.Size(1));

	size_t N=2*nFFTlength;
	fft=new kipl::math::fft::FFTBaseFloat(&N,1);

	float *pData=img.GetDataPtr();
	float *pLine=new float[2*nFFTlength];
	std::complex<float> *pCLine= new std::complex<float>[2*nFFTlength];

	cout<<"nLine="<<nLines<<endl;
	for (size_t line=0; line<nLines; line++) {
		memset(pLine,0,sizeof(float)*2*nFFTlength);
		GetVerticalLine(pData,pLine,line,nLines,img.Size(1));

		(*fft)(pLine,pCLine);

		for (size_t i=0; i< 2*nFFTlength; i++) {
//			pCLine[i]*=m_pDamping[i];
			pCLine[i]=static_cast<float>(i)/(2.0f*nFFTlength);
//			pCLine[i]/=(2.0f*nFFTlength);
		}

		(*fft)(pCLine, pLine);
		SetVerticalLine(pLine,pData,line,nLines,img.Size(1));
	}

	delete [] pLine;
	delete [] pCLine;
	delete fft;
}

void GetVerticalLine(float *pSrc, float *pLine, size_t pos, size_t stride, size_t len)
{
	float *pData=pSrc+pos;

	for (size_t i=0; i<len; i++, pData+=stride) {
		pLine[i]=*pData;
	}
}

void SetVerticalLine(float *pLine, float *pDest, size_t pos, size_t stride, size_t len)
{
	float *pData=pDest+pos;

	for (size_t i=0; i<len; i++, pData+=stride) {
		*pData=pLine[i];
	}
}


