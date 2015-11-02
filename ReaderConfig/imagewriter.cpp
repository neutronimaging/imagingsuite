#include <strings/filenames.h>
#include <io/io_tiff.h>
#include "imagewriter.h"
#include "readerexception.h"


ImageWriter::ImageWriter()
{

}

ImageWriter::~ImageWriter()
{

}

void ImageWriter::write(kipl::base::TImage<float,2> &img, std::string fname)
{
//    kipl::io::eExtensionTypes exttype=kipl::io::GetFileExtensionType(fname);

//    try {
//        switch (exttype) {
//        case kipl::io::ExtensionDMP :
//        case kipl::io::ExtensionDAT :
//        case kipl::io::ExtensionRAW : throw ReaderException("Saving raw data is not supported",__FILE__,__LINE__);
//                break;
//        case kipl::io::ExtensionFITS: throw ReaderException("Saving image as FITS is not supported",__FILE__,__LINE__);
//        case kipl::io::ExtensionTIFF: kipl::io::WriTIFF(img,);  break;
//        case kipl::io::ExtensionTXT : throw ReaderException("Saving image as txt is not supported",__FILE__,__LINE__);
//        case kipl::io::ExtensionXML : throw ReaderException("Saving image as xml is not supported",__FILE__,__LINE__);
//        case kipl::io::ExtensionPNG : throw ReaderException("Saving image as png is not supported",__FILE__,__LINE__);
//        case kipl::io::ExtensionMAT : throw ReaderException("Saving image as mat is not supported",__FILE__,__LINE__);
//        case kipl::io::ExtensionHDF : throw ReaderException("Saving image as hdf is not supported",__FILE__,__LINE__);
//        default : throw kipl::base::KiplException("The chosen file type is not implemented",__FILE__,__LINE__);
//        }
//    }

}
