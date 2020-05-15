#include "slice2vol.h"
#include <base/timage.h>
#include <strings/filenames.h>
#include <io/io_tiff.h>
#include <base/imagesamplers.h>
#include <iostream>

Slice2Vol::Slice2Vol()
{

}

void Slice2Vol::process(std::string filemask, size_t first, size_t last, size_t bin, std::string destname)
{
    std::cout<<filemask<<" "<<first<<" "<<last<<" "<<bin<<" "<<destname<<"\n";
    std::string fname,ext;
    kipl::strings::filenames::MakeFileName(filemask,static_cast<int>(first),fname,ext,'#','0');
    size_t dims[3];

    std::cout<<filemask<<"\n";
    kipl::io::GetTIFFDims(fname.c_str(),dims);
    dims[0]=dims[0]/bin;
    dims[1]=dims[1]/bin;
    dims[2]=(last-first+1)/bin;

    kipl::base::TImage<float,2> slice;
    kipl::base::TImage<float,2> binned;

    kipl::base::TImage<float,3> volume(dims);
    volume=0.0f;

    size_t bins[2]={bin,bin};

    std::cout<<"Processing "<<volume;
    for (size_t i=first,k=0; i<last; i+=bin,++k)
    {

        for (size_t j=0; j<bin; ++j)
        {
            if (i+j<=last)
            {
                kipl::strings::filenames::MakeFileName(filemask,i+j,fname,ext,'#');
                std::cout<<fname<<"\n";
                kipl::io::ReadTIFF(slice,fname.c_str());
                kipl::base::ReBin(slice,binned,bins);
                float *pVol    = volume.GetLinePtr(0,k);
                float *pBinned = binned.GetDataPtr();

                for (size_t n=0; n<binned.Size(); ++n)
                    pVol[n]+=pBinned[n];
            }
        }
    }

    if (1<bin)
    {
        float fbin=1.0f/bin;
        for (size_t i=0; i<volume.Size(); ++i)
            volume[i]*=fbin;
    }

    kipl::io::WriteTIFF(volume,destname.c_str());
}
