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


AdaptiveFilter::AdaptiveFilter(kipl::interactors::InteractionBase *interactor) :
    PreprocModuleBase("AdaptiveFilter",interactor),
	pLUT(NULL),
    m_nFilterSize(7),
    m_fEccentricityMin(0.3f),
    m_fEccentricityMax(0.7f), // original value 0.7
    m_fFilterStrength(1.0f),
    m_fFmax(0.10f),
    bNegative(false)
{}

AdaptiveFilter::~AdaptiveFilter()
{
	if (pLUT!=NULL)
		delete pLUT;
}

int AdaptiveFilter::Configure(ReconConfig config, std::map<std::string, std::string> parameters)
{
	mConfig=config;

    m_nFilterSize      = GetIntParameter(parameters,"filtersize");
    m_fFilterStrength  = GetFloatParameter(parameters,"filterstrength");
    m_fFmax            = GetFloatParameter(parameters,"fmax");
    bNegative          = kipl::strings::string2bool(GetStringParameter(parameters,"NegSinograms"));
//    m_fEccentricityMin = GetFloatParameter(parameters,"eccentricitymin");
//    m_fEccentricityMax = GetFloatParameter(parameters,"eccentricitymax");


	return 1;
}

std::map<std::string, std::string> AdaptiveFilter::GetParameters()
{
    std::map<std::string, std::string> parameters;

    parameters["filtersize"] = kipl::strings::value2string(m_nFilterSize);
    parameters["filterstrength"]  = kipl::strings::value2string(m_fFilterStrength);
    parameters["fmax"]            = kipl::strings::value2string(m_fFmax);
    parameters["NegSinograms"]  = kipl::strings::bool2string(bNegative);
//    parameters["eccentricitymin"] = kipl::strings::value2string(m_fEccentricityMin);
//    parameters["eccentricitymax"] = kipl::strings::value2string(m_fEccentricityMax);

    return parameters;
}

int AdaptiveFilter::ProcessCore(kipl::base::TImage<float,3> &img, std::map<std::string,std::string> &parameters)
{

    kipl::base::TImage<float,2> sinogram;

    const size_t N=img.Size(1);
    for (size_t i=0; (i<N) && (UpdateStatus(float(i)/N,m_sModuleName)==false); i++) {
     //   std::cout<<"Processing sinogram "<<i<<std::endl;
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

        float mymax = *std::max_element(img.GetLinePtr(0), img.GetLinePtr(0)+img.Size()); // find the max value

    if (bNegative){

        for (size_t x=0; x<img.Size(); ++x){
            img[x] = mymax-img[x]; // compute the negative img
        }

    }
    // before anything, check if values of the sino are below 0 and in that case set to 0, to avoid errors in eccentricity
    // find min and translate all values

    float mymin = *std::min_element(img.GetLinePtr(0),img.GetLinePtr(0)+img.Size());
//    std::cout << "mymin: " << mymin << std::endl;

    if (mymin<0) {
        for (size_t x=0; x<img.Size(); x++){
            img[x] = img[x]-mymin;
        }
    }

    float k[1024];
    float w=1.0f/static_cast<float>(m_nFilterSize);
    for (int i=0; i<m_nFilterSize; i++)
        k[i]=w; // it creates a mean filter with size m_nFilterSize and weights 1/m_nFilterSize

    size_t dimx[2]={static_cast<size_t>(m_nFilterSize), 1UL};

    kipl::filters::TFilter<float,2> filterx(k,dimx);
//    std::cout << "dimx: "<< dimx[0] << " " << dimx[1] << endl;


    kipl::base::TImage<float,2> smoothx;
//    kipl::base::TImage<float,2> weight;
//    kipl::base::TImage<float,2> thr_values;

    try {
        smoothx=filterx(img,kipl::filters::FilterBase::EdgeMirror); // apply average filter with size 7 in the row direction applico il filtro medio con dimensione 7 in direzione righe! quindi prendo un range
        // di angoli +- 7*projangle ..
    }

    catch (kipl::base::KiplException &e)
    {
        throw ReconException(e.what(),__FILE__, __LINE__);
    }



    // 1. compute smoothed max projection p(alfa) on sinograms, in pi/10 neighborhood-> that is not used later..
    // pi/10 of the neighborhood means 626 columns, each column is 0.58 degree, i want to threshold it on 18 degree, about 31 columns


//    std::cout << img.Size(0) << " " << img.Size(1) << endl; // image size are now correct

//    std::cout << mConfig.ProjectionInfo.fScanArc[0] << " " << mConfig.ProjectionInfo.fScanArc[1] << std::endl;
//    std::cout << mConfig.ProjectionInfo.fResolution[0] << " " << mConfig.ProjectionInfo.fResolution[1] << std::endl;
//    std::cout << mConfig.ProjectionInfo.nFirstIndex << " " << mConfig.ProjectionInfo.nLastIndex << std::endl;
//    std::cout << mConfig.ProjectionInfo.nlSkipList. << std::endl;
//    std::cout << mConfig.ProjectionInfo.nDims[0] << std::endl;// number of pixel
//    std::cout << img.Size(0) << " " << img.Size(1) << std::endl;



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
    // todo: replace by TFilter => Much faster - still missing
//    size_t dims[]={100,100};
//    kipl::base::TImage<float,2> img(dims),res;
//    float kernel[]={1,1,1, 1,1,1, 1,1,1};
//    size_t fdims[]={3,3};
//    kipl::filters::TFilter<float,2> box(kernel,fdims);
//    res=box(img);

    // that does not work I guess because of the absence of imgs... but array
//    float kernel[win];
//    for (size_t i=0; i<win; ++i){
//        kernel[i]=1;
//    }

//    size_t fdims[] = {win,1};
//    kipl::filters::TFilter<float,1>box(kernel,fdims);
//    av_pm = box(pad_pm);

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

    // compute p +(alfa) and p-(alfa) from pad images


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
    float den = 1/(m_fEccentricityMax-m_fEccentricityMin);

    for (size_t i=0; i<img.Size(1); i++) {

            pMax[i] = *std::max_element(padImg.GetLinePtr(i), padImg.GetLinePtr(i)+padImg.Size(0)*2*win90);
            pMin[i] = *std::min_element(padImg.GetLinePtr(i), padImg.GetLinePtr(i)+padImg.Size(0)*2*win90);
            ecc[i] = 1-pMin[i]/pMax[i];

//            std::cout << pMax[i] << " " << pMin[i] << " ";
//            std::cout << ecc[i] << std::endl;
            ecc[i] = (ecc[i]-m_fEccentricityMin)*den; // truncated eccentricity
                                                                                          // todo: replace by multiplication by precomputed reciprocal - OK
            if (ecc[i]<0) { ecc[i]=0; }
            if (ecc[i]>1) { ecc[i]=1; }

//            std::cout << ecc[i] << std::endl;
    }





    size_t dims[2] = {img.Size(0), img.Size(1)};





        float ws, wo;
        float high, low, mid;
        float *f_alfa = new float[img.Size(1)]; // fraction of modified projections
        float counts = 0.0f; // Todo: consider declare counts as float. You are anyway casting it several times - OK


        for (size_t y=0; y<dims[1]; y++){ //for each projection angle

            high = pMax[y];
            low = pMin[y];
            float *pImg = img.GetLinePtr(y);
            float *pFilt = smoothx.GetLinePtr(y);
            f_alfa[y] = m_fFmax*ecc[y]*m_fFilterStrength;

//            std:: cout << "searching in line ................................" << y << std::endl;


            bool search = true; // todo: use true and false instead of 0 and 1 - OK

            do{ // todo: as you always do at least one run the do {} while loop is the appropriate choice - OK

//                std::cout <<"--- high value ---  " << high << std::endl;
//                std::cout <<"--- low value ----  " << low << std::endl;

                mid = low + (high-low)/2; // threshold binary search


//                std::cout<< "----mid value------" << mid << std::endl;


                for (size_t x=0; x<dims[0]; x++) {
                    if (pImg[x]>=mid) {
                        counts += 1.0f;
                    }
                }
//                std::cout << counts << std::endl;
//                                return 0;

//                std::cout << "fraction of modified pixels: " << static_cast< float >(counts)/img.Size(0) << std::endl;
//                std::cout << "f_alfa: " << f_alfa[y] << std::endl;


//                if ( (static_cast< float >(counts/img.Size(0))<=f_alfa[y]+0.005 && (static_cast< float >(counts)/img.Size(0))>=f_alfa[y]-0.005) // se soddisfo la condizione
                float normalized_counts = counts/img.Size(0);

                if (abs(normalized_counts-f_alfa[y])<0.005)
                    // todo: Try abs(counts/size-f_alpha[y])<0.005 - OK
                    // todo: "static_cast< float >(counts)/img.Size(0))" is used several times compute it once as a constant - OK
                {
                    for (size_t x=0; x<dims[0]; x++){
                        // Todo: can be implemented without branch - OK
                         ws=static_cast<float>(pImg[x]>=mid);
                         wo=1.0f-ws;
//                        if (pImg[x]>=mid) {
//                            wo = 0;
//                            ws = 1;
//                        }
//                        else{
//                            wo = 1;
//                            ws = 0;
//                        }
                        pImg[x] = wo*pImg[x]+ws*pFilt[x];
                    }

//                    std::cout << "---------IF-------" << std::endl;
//                    std::cout <<"--- high value ---  " << high << std::endl;
//                    std::cout <<"--- low value ----  " << low << std::endl;

                    search = false;
                }
                else
                {
//                    std::cout << "---------ELSE-------" << std::endl;

                    if (normalized_counts>f_alfa[y]+0.005) // devo alzare la soglia
                    {
                        low = mid;}

                    if (normalized_counts<f_alfa[y]-0.005) // devo abbassare la soglia
                       {
                          high = mid;
                       }

//                    std::cout <<"--- high value ---  " << high << std::endl;
//                    std::cout <<"--- low value ----  " << low << std::endl;

                    if (high<=low+0.005) // con una certa tolleranza?
                    {
                        // se ho finito di cercare e cmq la mia frazione di pixel modificati è minore di quanto aspettato, applica il filtro
                        if ( (normalized_counts) <= (f_alfa[y]+0.005) ){
//                            std::cout << "------------ filter --------------" << std::endl;

                            for (size_t x=0; x<dims[0]; x++){
                                // todo: again try with branchless implementation - OK
                                ws=static_cast<float>(pImg[x]>=mid);
                                wo=1.0f-ws;
//                                if (pImg[x]>=mid) {
//                                    wo = 0;
//                                    ws = 1;
//                                }
//                                else{
//                                    wo = 1;
//                                    ws = 0;
//                                }
                                pImg[x] = wo*pImg[x]+ws*pFilt[x];
                            }

                        }
                        search = false;

                    }


                    counts = 0.0f;

                }



            }while (search);

                }


//        std::cout << img.Size() << std::endl;

        // after everything, rescale all values back to the original ones

            if (mymin<0) {
                for (size_t x=0; x<img.Size(); x++){
                    img[x] = img[x]+mymin;
                }
            }

            if (bNegative){
                for (size_t x=0; x<img.Size(); ++x){
                    img[x]= mymax-img[x]; // and go back into the non imverse values
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



