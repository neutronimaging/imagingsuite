#ifndef __IMAGEINFO_H
#define __IMAGEINFO_H
#include "../kipl_global.h"
#include <string>
#include <iostream>

namespace kipl { namespace base {

    KIPLSHARED_EXPORT class ImageInfo {
	public: 
		ImageInfo() : 
			sSoftware("KIPL image processing library"),
			sArtist("Anders Kaestner"),
			sCopyright("Anders Kaestner"),
			sDescription(""),
			nBitsPerSample(16),
			nSamplesPerPixel(1),
			fResolutionX(1.0f),
			fResolutionY(1.0f)
		{}
		
		ImageInfo(const ImageInfo & info) :
					sSoftware(info.sSoftware),
					sArtist(info.sArtist),
					sCopyright(info.sCopyright),
					sDescription(info.sDescription),
					nBitsPerSample(info.nBitsPerSample),
					nSamplesPerPixel(info.nSamplesPerPixel),
					fResolutionX(info.fResolutionX),
					fResolutionY(info.fResolutionY)
				{}

		const ImageInfo & operator=(const ImageInfo & info)
		{
			sSoftware=info.sSoftware;
			sArtist=info.sArtist;
			sCopyright=info.sCopyright;
			sDescription=info.sDescription;
			nBitsPerSample=info.nBitsPerSample;
			nSamplesPerPixel=info.nSamplesPerPixel;
			fResolutionX=info.fResolutionX;
			fResolutionY=info.fResolutionY;

			return *this;
		}

		void SetMetricX(float res) { fResolutionX=res; }
		float GetMetricX() {return fResolutionX;}
		
		void SetMetricY(float res) { fResolutionY=res; }
		float GetMetricY() {return fResolutionY;}

		void SetDPCMX(float res) { fResolutionX=10.0f/res; }
		float GetDPCMX() {return 10.0f/fResolutionX;}
		
		void SetDPCMY(float res) { fResolutionY=10.0f/res; }
		float GetDPCMY() {return 10.f/fResolutionY;}

		void SetDPIX(float dpi) { fResolutionX=25.4f/dpi; }
		float GetDPIX() {return 25.4f/fResolutionX;}
		
		void SetDPIY(float dpi) { fResolutionY=25.4f/dpi; }
		float GetDPIY() {return 25.4f/fResolutionY;}
		
		std::string sSoftware;    // TIFF tag 305
		std::string sArtist;	  // TIFF tag 315
		std::string sCopyright;   // TIFF tag 33432
		std::string sDescription; // TIFF tag 270	
		unsigned short nBitsPerSample;
		unsigned short nSamplesPerPixel;
		unsigned short nSampleFormat;


	private:
		float fResolutionX; // unit mm/pixel
		float fResolutionY; // unit mm/pixel

	};

}}

KIPLSHARED_EXPORT std::ostream & operator<<(std::ostream &s, kipl::base::ImageInfo &info);
#endif

