//
// AdaptiveFilter.cpp
//
//  Created on: May 25, 2011
//      Author: anders
//
//  Revision information
//    Checked in by $author$
//    Check-in date $date$
//    svn Revision  $revision$
//
// local smoothing with a filter function fDx of characteristics width  Dx such that this width
// is a function of the attenuation value p(x) that is currently beeing smoothed

//#include "stdafx.h"
#include "../include/AdaptiveFilter.h"

#include <ReconException.h>
#include <ParameterHandling.h>

#include <filters/filter.h>
#include <base/tprofile.h>
#include <math/mathfunctions.h>
#include <averageimage.h>


AdaptiveFilter::AdaptiveFilter() :
	PreprocModuleBase("AdaptiveFilter"),
	pLUT(NULL),
    m_nFilterSize(7),
//	m_fLambda(0.1f),
//    m_fSigma(0.01f), // doesn't seem to change very much from one to another value.. 0.01-0.05
    m_fEccentricityMin(0.3f),
    m_fEccentricityMax(0.7f), // original value 0.7
    m_fFilterStrength(1.0f),
    m_fFmax(0.10f)
{}

AdaptiveFilter::~AdaptiveFilter()
{
	if (pLUT!=NULL)
		delete pLUT;
}

int AdaptiveFilter::Configure(ReconConfig config, std::map<std::string, std::string> parameters)
{
	mConfig=config;
//    m_fLambda          = GetFloatParameter(parameters,"lambda");
//    m_fSigma           = GetFloatParameter(parameters,"sigma");
    m_nFilterSize      = GetIntParameter(parameters,"filtersize");
    m_fFilterStrength  = GetFloatParameter(parameters,"filterstrength");
    m_fFmax            = GetFloatParameter(parameters,"fmax");
//    m_fEccentricityMin = GetFloatParameter(parameters,"eccentricitymin");
//    m_fEccentricityMax = GetFloatParameter(parameters,"eccentricitymax");

    // mConfig.ProjectionInfo.fScanArc // gives me the first and last projections angles


//	if (pLUT!=NULL)
//		delete pLUT;
//    pLUT=new kipl::math::SigmoidLUT(1024, m_fLambda, m_fSigma); // compute sigmoid function
//    std::cout << pLUT[0] << std::endl;
//    std::cout<< m_fLambda << std::endl;
//    std::cout << m_fSigma << std::endl;
//    std::cout << m_nFilterSize << std::endl;

	return 1;
}

std::map<std::string, std::string> AdaptiveFilter::GetParameters()
{
    std::map<std::string, std::string> parameters;

//    parameters["lambda"] = kipl::strings::value2string(m_fLambda);
//    parameters["sigma"]  = kipl::strings::value2string(m_fSigma);
    parameters["filtersize"] = kipl::strings::value2string(m_nFilterSize);
    parameters["filterstrength"]  = kipl::strings::value2string(m_fFilterStrength);
    parameters["fmax"]            = kipl::strings::value2string(m_fFmax);
//    parameters["eccentricitymin"] = kipl::strings::value2string(m_fEccentricityMin);
//    parameters["eccentricitymax"] = kipl::strings::value2string(m_fEccentricityMax);

    return parameters;
}

int AdaptiveFilter::ProcessCore(kipl::base::TImage<float,3> &img, std::map<std::string,std::string> &parameters)
{
//	kipl::base::TImage<float,2> slice(img.Dims());

//	for (size_t i=0; i<img.Size(2); i++) {
//		memcpy(slice.GetDataPtr(),img.GetLinePtr(0,i),slice.Size()*sizeof(float));
//		ProcessCore(slice,parameters);
//		memcpy(img.GetLinePtr(0,i),slice.GetDataPtr(),slice.Size()*sizeof(float));
//	}

    kipl::base::TImage<float,2> sinogram;

    for (size_t i=0; i<img.Size(1); i++) {
        std::cout<<"Processing sinogram "<<i<<std::endl;
        ExtractSinogram(img,sinogram,i);
        ProcessSingle(sinogram, parameters);
        InsertSinogram(sinogram,img,i);
    }

	return 0;
}

int AdaptiveFilter::ProcessSingle(kipl::base::TImage<float,2> &img, std::map<std::string,std::string> &parameters)
{
    return SimpleFilter(img,parameters);
}

int AdaptiveFilter::SimpleFilter(kipl::base::TImage<float,2> &img, std::map<std::string,std::string> &parameters)
{

    // before anything, check if values of the sino are below 0 and in that case set to 0, to avoid errors in eccentricity
    // find min and translate all values

    float mymin = *std::min_element(img.GetLinePtr(0),img.GetLinePtr(0)+img.Size());
//    std::cout << "mymin: " << mymin << std::endl;

    if (mymin<0) {
        for (size_t x=0; x<img.Size(); x++){
            img[x] = img[x]-mymin;
        }
    }

//     ------------ Anders' version begin

    float k[1024];
    float w=1.0f/static_cast<float>(m_nFilterSize);
    for (int i=0; i<m_nFilterSize; i++)
        k[i]=w; // it creates a mean filter with size m_nFilterSize and weights 1/m_nFilterSize

    size_t dimx[2]={static_cast<size_t>(m_nFilterSize), 1UL};
//    size_t dimx[2]={static_cast<size_t>(m_nFilterSize)/2, static_cast<size_t>(m_nFilterSize)/2};

    kipl::filters::TFilter<float,2> filterx(k,dimx);
//    std::cout << "dimx: "<< dimx[0] << " " << dimx[1] << endl;


    kipl::base::TImage<float,2> smoothx;
//    kipl::base::TImage<float,2> weight;
//    kipl::base::TImage<float,2> thr_values;

    try {
        smoothx=filterx(img,kipl::filters::FilterBase::EdgeMirror); // applico il filtro medio con dimensione 7 in direzione righe! quindi prendo un range
        // di angoli +- 7*projangle ..
    }

    catch (kipl::base::KiplException &e)
    {
        throw ReconException(e.what(),__FILE__, __LINE__);
    }

//    float *profile=new float[img.Size(1)];


//    kipl::base::VerticalProjection2D(img.GetDataPtr(),img.Dims(),profile,true); //Extracts a vertical projection from the image, and compute mean (bool = true)

//    float *pImg    = img.GetDataPtr(); // immagine originale
//    float *pSmooth = smoothx.GetDataPtr(); // immagine smoothata


//    float ws,wo;
//        std::cout << "ciao"<< std::endl;
//        std::cout << img.Size(0)<< " " << img.Size(1)<< std::endl;

//    for (size_t i=0; i<img.Size(1); i++) { //img.Size()
//        ws=(*pLUT)(pSmooth[i]);
//        wo=1.0f-ws;

//        int count = 0;

//        for (size_t x=0; x<img.Size(0); x++) { //img.Size(0) // operate first on row i would say
//            pImg[x]=wo*pImg[x]+ws*pSmooth[x];
//                        if (ws!=1)
//                            count ++;

//        }

//        if (count!=0)
//         std::cout << count << std::endl; // ws is always equal to 1.. so no thresholding is done actually

//    }
    // ------------ Anders' version end



    // 1. compute smoothed max projection p(alfa) on sinograms, in pi/10 neighborhood-> that is not used later..
    // pi/10 of the neighborhood means 626 columns, each column is 0.58 degree, i want to threshold it on 18 degree, about 31 columns


//    std::cout << img.Size(0) << " " << img.Size(1) << endl; // image size are now correct

    std::cout << mConfig.ProjectionInfo.fScanArc[0] << " " << mConfig.ProjectionInfo.fScanArc[1] << std::endl;
    std::cout << mConfig.ProjectionInfo.fResolution[0] << " " << mConfig.ProjectionInfo.fResolution[1] << std::endl;
    std::cout << mConfig.ProjectionInfo.nFirstIndex << " " << mConfig.ProjectionInfo.nLastIndex << std::endl;
//    std::cout << mConfig.ProjectionInfo.nlSkipList. << std::endl;
    std::cout << mConfig.ProjectionInfo.nDims[0] << std::endl;// number of pixel
    std::cout << img.Size(0) << " " << img.Size(1) << std::endl;



    float *pm = new float[img.Size(1)];
    float *av_pm = new float[img.Size(1)];


    for (size_t i=0; i<img.Size(1); i++) { // for each sinogram row (i.e. for each angle) == iterate ALONG Y
        pm[i] = *std::max_element(img.GetLinePtr(i),img.GetLinePtr(i)+img.Size(0)); // compute max for each angle
//        std::cout << pm[i] << std::endl;
    }

    // then compute average on a window of pi/10 (or else..)
//    int win = 15; // window of 31 samples -> try with this
    int win = floor(img.Size(1)*18/mConfig.ProjectionInfo.fScanArc[1]/2);
//    std::cout << "win: " << win << std::endl;
    float sum;
//    int ind;

    // create pad array
    float *pad_pm = new float[img.Size(1)+2*win]; // +1 ?

    std::copy(pm+img.Size(1)-win, pm+img.Size(1), pad_pm);
    std::copy(pm, pm+img.Size(1), pad_pm+win);
    std::copy(pm, pm+win, pad_pm+win+img.Size(1));

//    std::cout << "padding done" << endl;
//    std::cout << pad_pm[0] << " " << pm[img.Size(1)-win] << endl;
//    std::cout << pad_pm[1] << " " << pm[img.Size(1)-win+1] << endl;
//    std::cout << pad_pm[win] << " " << pm[0] << endl;
//    std::cout << pad_pm[win+1] << " " << pm[1] << endl;
//    std::cout << pad_pm[win+img.Size(1)-1] << " " << pm[img.Size(1)-1] << endl; // seems correct.. also with Matlab check!



    // convolution filter

    for (size_t i=0; i<img.Size(1); i++) {

        sum=0;

        for(int j=-win; j<=win; j++)
        {
              sum+=pad_pm[i+j+win];
        }
           sum/=(2*win+1);
           av_pm[i]=sum;
//           std::cout << pm[i] << " " << av_pm[i] << endl;
    }


//    std::cout << "after convolution" << endl;




    // 2. compute p+(alfa) and p-(alfa) in +-pi/2 neighborhood, compute this time local minima and local maxima of p(alfa)
    // pi/2 on 625 projections are about 156 sample

    // second try without transpose but get column

    // third try: compute p +(alfa) and p-(alfa) from pad images

    // third try BEGIN

    float *pMax = new float[img.Size(1)];
    float *pMin = new float[img.Size(1)];
    float *ecc = new float[img.Size(1)];

    // padding
//    int win90 = 156;
    int win90 = floor(img.Size(1)*180/mConfig.ProjectionInfo.fScanArc[1]/2);
//    std::cout << "win90: " << win90 << std::endl;
    size_t pad_dims[2] = {img.Size(0),(img.Size(1)+2*win90)};
    kipl::base::TImage<float,2> padImg(pad_dims);

    // do pad

    std::copy(img.GetLinePtr(img.Size(1)-win90),img.GetLinePtr(img.Size(1))+img.Size(0), padImg.GetLinePtr(0));
    std::copy(img.GetLinePtr(0), img.GetLinePtr(img.Size(1))+img.Size(0), padImg.GetLinePtr(win90));
    std::copy(img.GetLinePtr(0), img.GetLinePtr(win90-1)+img.Size(0), padImg.GetLinePtr(win90+img.Size(1)));


    // compute pmax and pmin

    for (size_t i=0; i<img.Size(1); i++) {

            pMax[i] = *std::max_element(padImg.GetLinePtr(i), padImg.GetLinePtr(i)+padImg.Size(0)*2*win90);
            pMin[i] = *std::min_element(padImg.GetLinePtr(i), padImg.GetLinePtr(i)+padImg.Size(0)*2*win90);
            ecc[i] = 1-pMin[i]/pMax[i];

//            std::cout << pMax[i] << " " << pMin[i] << " ";
//            std::cout << ecc[i] << std::endl;
            ecc[i] = (ecc[i]-m_fEccentricityMin)/(m_fEccentricityMax-m_fEccentricityMin); // truncated eccentricity
            if (ecc[i]<0) { ecc[i]=0; }
            if (ecc[i]>1) { ecc[i]=1; }

//            std::cout << ecc[i] << std::endl;
    }



    // third try END






//// --------------------- second try BEGIN
////    size_t tdims[2]={img.Size(1), img.Size(0)}; // compute also transposed max and min? i would say yes
    size_t dims[2] = {img.Size(0), img.Size(1)};
//    kipl::base::TImage <float,2> tlMIN(dims);
//    kipl::base::TImage <float,2> tlMAX(dims);

//    std::cout << img.Size(0) << " " << img.Size(1) << endl;
////    kipl::base::TImage <float,2> timg (kipl::base::Transpose<float,2>()(img)); // probably not the smart way..
////    std::cout << timg.Size(0) << " " << timg.Size(1) << endl;

//    // pad array
//    int win90 = 156;
////    float *padImg = new float[timg.Size(0)*(timg.Size(1)+2*win90)]; // or better to construct as an image?
//    size_t pad_dims[2] = {img.Size(0),(img.Size(1)+2*win90)};
//    kipl::base::TImage<float,2> padImg(pad_dims);


//    // do pad

//    std::copy(img.GetLinePtr(img.Size(1)-win90),img.GetLinePtr(img.Size(1))+img.Size(0), padImg.GetLinePtr(0));
//    std::copy(img.GetLinePtr(0), img.GetLinePtr(img.Size(1))+img.Size(0), padImg.GetLinePtr(win90));
//    std::copy(img.GetLinePtr(0), img.GetLinePtr(win90-1)+img.Size(0), padImg.GetLinePtr(win90+img.Size(1)));


//    std::cout << padImg[0] << " " << padImg[1] << endl; // seems correct compared to Matlab

//    // transpose padded: to then compute min and max values in lines and not in column.. understand if there is a better way!!

////    kipl::base::TImage <float,2> timg (kipl::base::Transpose<float,2>()(padImg));
//// instead of padding i could create a getcolumn method like in imagingalgorithm::averageimage.. makes more sense!

//    std::cout << tlMIN.Size(0)<< " " << tlMIN.Size(1) << endl;
//    std::cout << padImg.Size(0)<< " " << padImg.Size(1) << endl;

//    float *mycolumn = new float[padImg.Size(1)];

//// compute local min and max, and i have to do it from one row to another, so column wise
//    for (size_t x=0; x<tlMIN.Size(0); x++) { // columns // x

//        GetColumn(padImg,x,mycolumn);
//        float *pMin = tlMIN.GetDataPtr()+x; // pointer to the first element of the column
//        float *pMax = tlMAX.GetDataPtr()+x;

//        for (size_t y=0; y<tlMIN.Size(1); y++){ //row, y -> iterate to each elemen of the line.. i think that is the correct way of doing it..

//            // compute max and min values in -pi/2 - pi/2 neighborhood on Pimg, then copy value in pMin and pMax
//                pMax[y*tlMIN.Size(0)] = *std::max_element(mycolumn+y, mycolumn+y+2*win90);
//                pMin[y*tlMIN.Size(0)] = *std::min_element(mycolumn+y, mycolumn+y+2*win90);


//        }
//    }

//    delete [] mycolumn;
//    std::cout << tlMIN[0] << " " << tlMIN[1] << endl;
//    std::cout << tlMAX[0] << " " << tlMAX[1] << endl;
//    std::cout << tlMIN[200] << " " << tlMIN[201] << endl;
//    std::cout << tlMAX[200] << " " << tlMAX[201] << endl;
//    // ---------------- second try END




//    kipl::base::TImage <float,2> eImg(dims);

//    std::cout << eImg.Size(0) << " " << eImg.Size(1) << std::endl;

//    float mine, maxe;

//    for (size_t y=0; y<dims[1]; y++) {

//        float *ptlMIN = tlMIN.GetLinePtr(y);
//        float *ptlMAX = tlMAX.GetLinePtr(y);
//        float *peImg = eImg.GetLinePtr(y);

//        for (size_t x=0; x<dims[0]; x++){

//            peImg[x] = 1-ptlMIN[x]/ptlMAX[x];
//            peImg[x] = (peImg[x]-m_fEccentricityMin)/(m_fEccentricityMax-m_fEccentricityMin); // truncated eccentricity

//            if (peImg[x]<0) { peImg[x]=0; }
//            if (peImg[x]>1) { peImg[x]=1; }

//        }

//    }




//    mine = *std::min_element(eImg.GetDataPtr(), eImg.GetDataPtr()+eImg.Size());
//    maxe = *std::max_element(eImg.GetDataPtr(), eImg.GetDataPtr()+eImg.Size());

//    std::cout << "----- min and max Eccentricity -----" << std::endl;
//    std::cout << mine << " "<< maxe << std::endl;


//        std::cout << eImg[90] << " " << eImg[0]<< " " << eImg[200] << std::endl;

//        // second try END




    // ------------------ 3. apply threshold ---------------------------

//    float kernel[]={1.0/9.0,1.0/9.0,1.0/9.0, 1.0/9.0,1.0/9.0,1.0/9.0, 1.0/9.0,1.0/9.0,1.0/9.0};
//    size_t fdims[]={9,1}; // seems more correct like this, filter lines in the sinograms, so values for the same angle value.
//    kipl::filters::TFilter<float,2> box(kernel,fdims);
//    kipl::base::TImage <float, 2> smoothx;
//    smoothx =  box(img, kipl::filters::FilterBase::EdgeMirror);

//        // first threshold computation BEGIN

//    int counts = 0;
//    float ws,wo;

//    for (size_t y=0; y<dims[1]; y++) {

//        float *peImg = eImg.GetLinePtr(y);
//        float *pImg = img.GetLinePtr(y);
//        float *pFilt = smoothx.GetLinePtr(y);

//        for (size_t x=0; x<dims[0]; x++){

//            if (pImg[x]>=av_pm[y]){ // not sure that has to be done.. to understand it better

//                if (peImg[x]>=0.4) { // this should be replaced with some adaptive searching of the threshold value.
//                    wo = 0;
//                    ws = 1;
//                    counts++;

//                } else {
//                    wo = 1;
//                    ws = 0;

//                }
//                pImg[x] = wo*pImg[x]+ws*pFilt[x];
////                pImg[x] = pImg[x];
//            }

//        }
//    }

//    std::cout << counts << std::endl;
//    std::cout << img.Size() << std::endl;

//    if (mymin<0) {
//        for (size_t x=0; x<img.Size(); x++){
//            img[x] = img[x]+mymin;
//        }
//    }
//    // first threshold computation END

    // second try BEGIN with adaptive computation of the threshold


        float ws, wo;
        float high, low, mid;
        float *f_alfa = new float[img.Size(1)]; // fraction of modified projections
        int counts = 0;


        for (size_t y=0; y<dims[1]; y++){ //for each projection angle

            high = pMax[y];
            low = pMin[y];
            float *pImg = img.GetLinePtr(y);
            float *pFilt = smoothx.GetLinePtr(y);
            f_alfa[y] = m_fFmax*ecc[y]*m_fFilterStrength;

//            std:: cout << "searching in line ................................" << y << std::endl;


            bool search = 1;

            while (search) {

//                std::cout <<"--- high value ---  " << high << std::endl;
//                std::cout <<"--- low value ----  " << low << std::endl;

                mid = low + (high-low)/2; // threshold binary search


//                std::cout<< "----mid value------" << mid << std::endl;


                for (size_t x=0; x<dims[0]; x++) {
                    if (pImg[x]>=mid) {
                        counts++;
                    }
                }
//                std::cout << counts << std::endl;
//                                return 0;

//                std::cout << "fraction of modified pixels: " << static_cast< float >(counts)/img.Size(0) << std::endl;
//                std::cout << "f_alfa: " << f_alfa[y] << std::endl;


                if ( (static_cast< float >(counts)/img.Size(0))<=f_alfa[y]+0.005 && (static_cast< float >(counts)/img.Size(0))>=f_alfa[y]-0.005) // se soddisfo la condizione
                {
                    for (size_t x=0; x<dims[0]; x++){
                        if (pImg[x]>=mid) {
                            wo = 0;
                            ws = 1;
                        }
                        else{
                            wo = 1;
                            ws = 0;
                        }
                        pImg[x] = wo*pImg[x]+ws*pFilt[x];
                    }

//                    std::cout << "---------IF-------" << std::endl;
//                    std::cout <<"--- high value ---  " << high << std::endl;
//                    std::cout <<"--- low value ----  " << low << std::endl;

                    search = 0;
                }
                else
                {
//                    std::cout << "---------ELSE-------" << std::endl;

                    if (static_cast< float >(counts)/img.Size(0)>f_alfa[y]+0.005) // devo alzare la soglia
                    {
                        low = mid;}

                    if (static_cast< float >(counts)/img.Size(0)<f_alfa[y]-0.005) // devo abbassare la soglia
                       {
                          high = mid;
                       }

//                    std::cout <<"--- high value ---  " << high << std::endl;
//                    std::cout <<"--- low value ----  " << low << std::endl;

                    if (high<=low+0.005) // con una certa tolleranza?
                    {
                        // se ho finito di cercare e cmq la mia frazione di pixel modificati è minore di quanto aspettato, applica il filtro
                        if ( (static_cast< float >(counts)/img.Size(0)) <= (f_alfa[y]+0.005) ){
//                            std::cout << "------------ filter --------------" << std::endl;

                            for (size_t x=0; x<dims[0]; x++){
                                if (pImg[x]>=mid) {
                                    wo = 0;
                                    ws = 1;
                                }
                                else{
                                    wo = 1;
                                    ws = 0;
                                }
                                pImg[x] = wo*pImg[x]+ws*pFilt[x];
                            }

                        }
                        search = 0;

                    }


                    counts = 0;

                }



            }

                }


//        std::cout << img.Size() << std::endl;

        // after everything, rescale all values back to the original ones

            if (mymin<0) {
                for (size_t x=0; x<img.Size(); x++){
                    img[x] = img[x]+mymin;
                }
            }



	return 0;
}

void AdaptiveFilter::MaxProfile(kipl::base::TImage<float,3> &img, kipl::base::TImage<float,2> &profile)
{
    size_t Nx=img.Size(0);
    size_t Ny=img.Size(1);
    size_t Nz=img.Size(2);

    size_t pdims[2]={Ny,Nz};

    profile.Resize(pdims);

    for (size_t z=0; z<Nz; z++) {
        float *pProfile=profile.GetLinePtr(z);

        for (size_t y=0; y<Ny; y++) {
            float *pImg=img.GetLinePtr(y,z);
            pProfile[y]=pImg[0];
            for (size_t x=1; x<Nx; x++) {
                if (pProfile[y]<pImg[x]) pProfile[y]=pImg[x];
            }
        }
    }

}

void AdaptiveFilter::MinProfile(kipl::base::TImage<float,3> &img, kipl::base::TImage<float,2> &profile)
{
    size_t Nx=img.Size(0);
    size_t Ny=img.Size(1);
    size_t Nz=img.Size(2);

    size_t pdims[2]={Ny,Nz};

    profile.Resize(pdims);

    for (size_t z=0; z<Nz; z++) {
        float *pProfile=profile.GetLinePtr(z);

        for (size_t y=0; y<Ny; y++) {
            float *pImg=img.GetLinePtr(y,z);
            pProfile[y]=pImg[0];
            for (size_t x=1; x<Nx; x++) {
                if (pProfile[y]>pImg[x]) pProfile[y]=pImg[x];
            }
        }
    }

}

void AdaptiveFilter::MinMaxProfile(kipl::base::TImage<float,2> &img, std::vector<float> &minprofile, std::vector<float> &maxprofile)
{
    size_t Nx=img.Size(0);
    size_t Ny=img.Size(1);

    size_t pdims[1]={Ny};

    minprofile.resize(Ny);
    maxprofile.resize(Ny);

    for (size_t y=0; y<Ny; y++) {
        float *pImg=img.GetLinePtr(y);
        minprofile[y]=pImg[0];
        maxprofile[y]=pImg[0];
        for (size_t x=1; x<Nx; x++) {
            if (maxprofile[y]<pImg[x]) maxprofile[y]=pImg[x]; // local max
            if (pImg[x]<minprofile[y]) minprofile[y]=pImg[x]; // local min -> shouldn't be computed on [-pi/2; pi/2] of the local view?
        }
    }
}

void AdaptiveFilter::Eccentricity(std::vector<float> &minprofile, std::vector<float> &maxprofile, std::vector<float> &eprofile)
{
    eprofile.resize(minprofile.size());

    for (size_t i=0; i<minprofile.size(); i++) {
        eprofile[i]=1-minprofile[i]/maxprofile[i];
        eprofile[i]=(eprofile[i]-m_fEccentricityMin)/(m_fEccentricityMax-m_fEccentricityMin);
        eprofile[i]=max(0.0f,eprofile[i]);
        eprofile[i]=max(1.0f,eprofile[i]);
    }
}

void AdaptiveFilter::GetColumn(kipl::base::TImage<float,2> &img, size_t idx, float *data)
{
    float *pImg=img.GetDataPtr()+idx;
    size_t N=img.Size(0);

    for (size_t i=0; i<img.Size(1); i++)
    {
        data[i]=pImg[i*N];
    }

}



