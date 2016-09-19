#ifndef __IMAGEINFO_H
#define __IMAGEINFO_H
#include "../kipl_global.h"
#include <string>
#include <iostream>

namespace kipl { namespace base {

/// \brief Meta data information struct
    class KIPLSHARED_EXPORT ImageInfo {
	public: 
        /// \brief c'tor that sets default values
		ImageInfo() : 
			sSoftware("KIPL image processing library"),
			sArtist("Anders Kaestner"),
			sCopyright("Anders Kaestner"),
			sDescription(""),
			nBitsPerSample(16),
			nSamplesPerPixel(1),
            nSampleFormat(0),
			fResolutionX(1.0f),
			fResolutionY(1.0f)
		{}
		
        /// \brief Copy c'tor
        /// \param info The instance to copy
		ImageInfo(const ImageInfo & info) :
					sSoftware(info.sSoftware),
					sArtist(info.sArtist),
					sCopyright(info.sCopyright),
					sDescription(info.sDescription),
					nBitsPerSample(info.nBitsPerSample),
					nSamplesPerPixel(info.nSamplesPerPixel),
                    nSampleFormat(info.nSampleFormat),
					fResolutionX(info.fResolutionX),
					fResolutionY(info.fResolutionY)
				{}

        /// \brief Assignment operator
        /// \param info The instance to copy
		const ImageInfo & operator=(const ImageInfo & info)
		{
            sSoftware        = info.sSoftware;
            sArtist          = info.sArtist;
            sCopyright       = info.sCopyright;
            sDescription     = info.sDescription;
            nBitsPerSample   = info.nBitsPerSample;
            nSamplesPerPixel = info.nSamplesPerPixel;
            nSampleFormat    = info.nSampleFormat;
            fResolutionX     = info.fResolutionX;
            fResolutionY     = info.fResolutionY;

			return *this;
		}

        /// \brief Sets the metric pixel size in the X-direction
        /// \param res pixel size in mm
		void SetMetricX(float res) { fResolutionX=res; }

        /// \brief Getter for the metric resolution in the X-direction
        /// \return the pixel size in mm
		float GetMetricX() {return fResolutionX;}

        /// \brief Sets the metric pixel size in the Y-direction
        /// \param res pixel size in mm
		void SetMetricY(float res) { fResolutionY=res; }

        /// \brief Getter for the metric resolution in the Y-direction
        /// \return the pixel size in mm
		float GetMetricY() {return fResolutionY;}

        /// \brief Set the metric resolution as pixels per cm in the X-direction
        /// \param the pixel size in mm
		void SetDPCMX(float res) { fResolutionX=10.0f/res; }

        /// \brief Getter for the metric resolution in the X-direction
        /// \return the resolution in pixels per cm
		float GetDPCMX() {return 10.0f/fResolutionX;}
		
        /// \brief Getter for the metric resolution in the Y-direction
        /// \return the resolution in pixels per cm
		void SetDPCMY(float res) { fResolutionY=10.0f/res; }

        /// \brief Getter for the metric resolution in the Y-direction
        /// \return the number of pixels per cm
        float GetDPCMY() {return 10.f/fResolutionY;}

        /// \brief Set for the metric resolution as pixels per cm in the X-direction
        /// \param The number of pixels per cm
		void SetDPIX(float dpi) { fResolutionX=25.4f/dpi; }

        /// \brief Getter for the imperial resolution in the X-direction
        /// \return the number of pixels per inch
		float GetDPIX() {return 25.4f/fResolutionX;}

        /// \brief Set for the metric resolution as pixels per cm in the X-direction
        /// \param The number of pixels per cm
		void SetDPIY(float dpi) { fResolutionY=25.4f/dpi; }

        /// \brief Getter for the imperial resolution in the Y-direction
        /// \return the number of pixels per inch
		float GetDPIY() {return 25.4f/fResolutionY;}
		
        std::string    sSoftware;        ///< TIFF tag 305
        std::string    sArtist;	         ///< TIFF tag 315
        std::string    sCopyright;       ///< TIFF tag 33432
        std::string    sDescription;     ///< TIFF tag 270
        unsigned short nBitsPerSample;   ///< Number of bits per sample as read from the image file
        unsigned short nSamplesPerPixel; ///< Number of samples per pixel in the image file. Mainly applies to color images
        unsigned short nSampleFormat;

	private:
        float          fResolutionX;     ///< unit mm/pixel
        float          fResolutionY;     ///< unit mm/pixel
	};

}}

std::ostream KIPLSHARED_EXPORT  & operator<<(std::ostream &s, kipl::base::ImageInfo &info);
#endif

