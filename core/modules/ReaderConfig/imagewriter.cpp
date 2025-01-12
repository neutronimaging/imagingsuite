//<LICENSE>

#include <strings/filenames.h>
#include <io/io_tiff.h>
#include <io/io_fits.h>
#include "analyzefileext.h"
#include "imagewriter.h"
#include "readerexception.h"


ImageWriter::ImageWriter() :
    logger("ImageWriter")
{
    logger.message("Writer started");
}

ImageWriter::~ImageWriter()
{
    logger.message("Writer ended");
}

void ImageWriter::write(kipl::base::TImage<float,2> &img, std::string fname,kipl::base::eDataType dt)
{

    readers::eExtensionTypes exttype=readers::GetFileExtensionType(fname);
    std::ostringstream msg;
    msg<<"File "<<fname<<" has ext "<<exttype;
    logger.message(msg.str());
    try {
        switch (exttype) {
            case readers::ExtensionDMP :
            case readers::ExtensionDAT :
            case readers::ExtensionRAW : throw ReaderException("Saving raw data is not supported",__FILE__,__LINE__);
                    break;
            case readers::ExtensionFITS:
                logger.message(msg.str());
                kipl::io::WriteFITS(img,fname.c_str());
                msg.str("");
                msg<<"Wrote image "<<img<<" as "<<fname<<" using WriteFITS ";
                logger(logger.LogDebug,msg.str());
                break;
            case readers::ExtensionTIFF:
                logger.message(msg.str());
                kipl::io::WriteTIFF(img,fname.c_str(),dt);
                msg.str("");
                msg<<"Wrote image "<<img<<" as "<<fname<<" using WriteTIFF ";
                logger(logger.LogDebug,msg.str());
                break;
            case readers::ExtensionTXT : throw ReaderException("Saving image as txt is not supported",__FILE__,__LINE__); break;
            case readers::ExtensionXML : throw ReaderException("Saving image as xml is not supported",__FILE__,__LINE__); break;
            case readers::ExtensionPNG : throw ReaderException("Saving image as png is not supported",__FILE__,__LINE__); break;
            case readers::ExtensionHDF : throw ReaderException("Saving image as hdf is not supported",__FILE__,__LINE__); break;
            default : throw ReaderException("The chosen file type is not implemented",__FILE__,__LINE__); break;
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
