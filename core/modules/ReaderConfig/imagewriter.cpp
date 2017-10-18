//<LICENSE>

#include <strings/filenames.h>
#include <io/io_tiff.h>
#include <io/io_fits.h>
#include <io/analyzefileext.h>
#include "imagewriter.h"
#include "readerexception.h"


ImageWriter::ImageWriter() :
    logger("ImageWriter")
{
    logger(logger.LogMessage,"Writer started");
}

ImageWriter::~ImageWriter()
{
    logger(logger.LogMessage,"Writer ended");
}

void ImageWriter::write(kipl::base::TImage<float,2> &img, std::string fname)
{

    kipl::io::eExtensionTypes exttype=kipl::io::GetFileExtensionType(fname);
    std::ostringstream msg;
    try {
        switch (exttype) {
            case kipl::io::ExtensionDMP :
            case kipl::io::ExtensionDAT :
            case kipl::io::ExtensionRAW : throw ReaderException("Saving raw data is not supported",__FILE__,__LINE__);
                    break;
            case kipl::io::ExtensionFITS:
                kipl::io::WriteFITS(img,fname.c_str());
                msg.str("");
                msg<<"Wrote "<<fname<<" using WriteFITS.";
                logger(logger.LogDebug,msg.str());
                break;
            case kipl::io::ExtensionTIFF:
                kipl::io::WriteTIFF(img,fname.c_str());
                msg.str("");
                msg<<"Wrote "<<fname<<" using WriteTIFF.";
                logger(logger.LogDebug,msg.str());
                break;
            case kipl::io::ExtensionTXT : throw ReaderException("Saving image as txt is not supported",__FILE__,__LINE__);
            case kipl::io::ExtensionXML : throw ReaderException("Saving image as xml is not supported",__FILE__,__LINE__);
            case kipl::io::ExtensionPNG : throw ReaderException("Saving image as png is not supported",__FILE__,__LINE__);
            case kipl::io::ExtensionMAT : throw ReaderException("Saving image as mat is not supported",__FILE__,__LINE__);
            case kipl::io::ExtensionHDF : throw ReaderException("Saving image as hdf is not supported",__FILE__,__LINE__);
            default : throw kipl::base::KiplException("The chosen file type is not implemented",__FILE__,__LINE__);
        }
    }
    catch (ReaderException &e) {
        msg.str("");
        msg<<"ImageWriter failed to save the image (ReaderException):"<<std::endl<<e.what();
        throw ReaderException(msg.str(),__FILE__,__LINE__);
    }
    catch (kipl::base::KiplException &e) {
        msg.str("");
        msg<<"ImageWriter failed to save the image (KiplException):"<<std::endl<<e.what();
        throw ReaderException(msg.str(),__FILE__,__LINE__);
    }
    catch (std::exception &e) {
        msg.str("");
        msg<<"ImageWriter failed to save the image (STLException):"<<std::endl<<e.what();
        throw ReaderException(msg.str(),__FILE__,__LINE__);
    }
    catch (...) {
        msg.str("");
        msg<<"ImageWriter failed to save the image (Unknown exception):";
        throw ReaderException(msg.str(),__FILE__,__LINE__);
    }
}
