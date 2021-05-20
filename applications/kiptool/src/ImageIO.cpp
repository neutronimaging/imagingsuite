//<LICENSE>

#include <KiplProcessConfig.h>
#include <KiplFrameworkException.h>
#include <base/KiplException.h>
#include <sstream>
#include <string>
#include <base/timage.h>
#include <imagereader.h>
#include <interactors/interactionbase.h>

kipl::base::TImage<float,3> LoadVolumeImage(KiplProcessConfig & config, kipl::interactors::InteractionBase *interactor)
{

	std::ostringstream msg;
    std::string fname=config.mImageInformation.sSourceFileMask;

    kipl::base::TImage<float,3> img;
    ImageReader reader(interactor);
    try
    {
       if (config.mImageInformation.bUseROI==true)
       {
            img=reader.Read(fname,
                config.mImageInformation.nFirstFileIndex,
                config.mImageInformation.nLastFileIndex,
                config.mImageInformation.nStepFileIndex,
                config.mImageInformation.eFlip,
                config.mImageInformation.eRotate,
                1.0f,
                config.mImageInformation.nROI);
        }
        else
       {
            img=reader.Read(fname,
                config.mImageInformation.nFirstFileIndex,
                config.mImageInformation.nLastFileIndex,
                config.mImageInformation.nStepFileIndex,
                config.mImageInformation.eFlip,
                config.mImageInformation.eRotate,
                1.0f,
                {});
        }

    }
    catch (kipl::base::KiplException &e) {
            msg<<"KiplException with message: "<<e.what();
            throw KiplFrameworkException(msg.str(),__FILE__,__LINE__);
    }
    catch (std::exception &e) {
            msg<<"STL Exception with message: "<<e.what();
            throw KiplFrameworkException(msg.str(),__FILE__,__LINE__);
    }
    catch (...) {
            msg<<"Unknown exception thrown while reading image";
            throw KiplFrameworkException(msg.str(),__FILE__,__LINE__);
    }

	return img;
}
