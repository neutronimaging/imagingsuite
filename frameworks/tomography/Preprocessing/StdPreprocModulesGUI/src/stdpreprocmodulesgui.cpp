//<LICENSE>


#include "StdPreprocModulesGUI_global.h"
#include "stdpreprocmodulesgui.h"
//#include <ConfiguratorDialogBase.h>
#include "SpotClean2Dlg.h"
//#include "ISSfilterDlg.h"
#include "FullLogNormDlg.h"
#include "medianmixringcleandlg.h"
#include "projectionfilterdlg.h"
#include "polynomialcorrectiondlg.h"
#include "WaveletRingCleanDlg.h"
#include "morphspotcleandlg.h"
#include "datascalerdlg.h"
#include "adaptivefilterdlg.h"
#include "generalfilterdlg.h"
#include "bblognormdlg.h"
#include "ReplaceUnderexposedDlg.h"
#include "detectorlagdlg.h"

class ConfiguratorDialogBase;

STDPREPROCMODULESGUISHARED_EXPORT void *GetGUIModule(const char *application, const char *name, void * /*interactor*/)
{
	if (strcmp(application,"muhrec")!=0)
        return nullptr;

    if (name!=nullptr) {
		std::string sName=name;

        if (sName=="FullLogNorm")
            return new FullLogNormDlg;


        if (sName=="BBLogNorm")
            return new BBLogNormDlg;

//        if (sName=="LogProjection")
//            return new LogProjection;

        if (sName=="ProjectionFilterSingle")
            return new ProjectionFilterDlg;

		//if (sName=="SpotClean")
		//	return new SpotClean;

        if (sName=="SpotClean2")
            return new SpotClean2Dlg;

        if (sName=="GeneralFilter")
            return new GeneralFilterDlg;

        if (sName=="AdaptiveFilter")
            return new AdaptiveFilterDlg;

		//if (sName=="BasicRingClean")
		//	return new BasicRingClean;

        if (sName=="MedianMixRingClean")
            return new MedianMixRingCleanDlg;

        if (sName=="DataScaler")
            return new DataScalerDlg;

//		if (sName=="ISSfilter")
//			return new ISSfilterDlg;

        if (sName=="WaveletRingClean")
            return new WaveletRingCleanDlg;

        if (sName=="PolynomialCorrection")
            return new PolynomialCorrectionDlg;

        if (sName=="MorphSpotClean")
            return new MorphSpotCleanDlg;

        if (sName=="ReplaceUnderexposed")
            return new ReplaceUnderexposedDlg;

        if (sName=="DetectorLag")
            return new DetectorLagDlg;

	}	
    return nullptr;
}

STDPREPROCMODULESGUISHARED_EXPORT int DestroyGUIModule(const char *application, void *obj)
{
	if (strcmp(application,"muhrec")!=0)
		return -1;

    if (obj!=nullptr) {
		ConfiguratorDialogBase *dlg=reinterpret_cast<ConfiguratorDialogBase *>(obj);
		delete dlg;
	}

	return 0;
}
