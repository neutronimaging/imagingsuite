//<LICENSE>

#include "../include/gammaclean.h"
#include <sstream>
#include <list>
#include <algorithm>
#include <array>
#include <vector>

#include <filters/laplacianofgaussian.h>
#include <filters/medianfilter.h>
#include <math/median.h>

#include "../include/ImagingException.h"

namespace ImagingAlgorithms {

        
//     void process(kipl::base::TImage<float,2> & img);
//     void process(kipl::base::TImage<float,3> & img);
    
//     void configure(float sigma, float th3, float th5, float th7, size_t medsize);

//     kipl::base::TImage<float,2> DetectionImage(kipl::base::TImage<float,2> &img);

//     void setConnectivity(kipl::base::eConnectivity conn = kipl::base::conn8);
//     kipl::base::eConnectivity connectivity();
        
//     void setLimits(bool bClamp, float fMin, float fMax);
//     std::vector<float> clampLimits();
//     bool clampActive();
    
//     void setCleanInfNan(bool activate);
//     bool cleanInfNan();
//     void useThreading(bool x);
//     bool isThreaded();
//     int  numberOfThreads();

// private:
//     void prepareNeighborhoods(const std::vector<size_t> &dims);
//     void medianNeighborhood(float *pImg, float *pRes, ptrdiff_t pos, ptrdiff_t *ng, size_t N);
//     void laplacianOfGaussian(float *pImg, float *pRes, ptrdiff_t pos, ptrdiff_t *ng, size_t N);

GammaClean::GammaClean( float sigma, 
                        float th3, 
                        float th5, 
                        float th7, 
                        size_t medsize, 
                        bool useThreads, 
                        kipl::interactors::InteractionBase *interactor) :
    m_logger("GammaClean"),
    m_pInteractor(interactor),
    m_threadPool(nullptr),
    m_fSigma(sigma),
    m_fThreshold3(th3),
    m_fThreshold5(th5),
    m_fThreshold7(th7),
    m_nMedianSize(medsize)
{
    if (useThreads) {
        m_threadPool = new kipl::utilities::ThreadPool(std::thread::hardware_concurrency());
    }   
}

void GammaClean::configure(float sigma, float th3, float th5, float th7, size_t medsize)
{
    m_fSigma       = sigma;
    m_fThreshold3  = th3;
    m_fThreshold5  = th5;
    m_fThreshold7  = th7;
    m_nMedianSize  = medsize;
}

void GammaClean::process(kipl::base::TImage<float,3> & /*img*/)
{
    throw ImagingException("3D processing not implemented yet",__FILE__,__LINE__);
}

void GammaClean::process(kipl::base::TImage<float,2> & img)
{
    std::stringstream msg;

    msg.str(""); msg<<"Starting GammaClean with parameters: sigma="<<m_fSigma<<", th3="<<m_fThreshold3<<", th5="<<m_fThreshold5<<", th7="<<m_fThreshold7<<", median size="<<m_nMedianSize;
    m_logger.message(msg.str());
    kipl::base::TImage<float,2> LoG=kipl::filters::LaplacianOfGaussian(img,m_fSigma);

    std::vector<size_t>  meddims={m_nMedianSize,m_nMedianSize};
    kipl::filters::TMedianFilter<float,2> med3(meddims);

    kipl::base::TImage<float,2> LoGm3=med3(LoG);

    m_diff.resize(img.dims()); 
    m_mask.resize(img.dims()); 

    // float *pLoG   = LoG.GetDataPtr();
    // float *pLoGm3 = LoGm3.GetDataPtr();
    // int   *pMask  = m_mask.GetDataPtr();

    std::vector<ptrdiff_t> m3;
    std::vector<ptrdiff_t> m5;
    std::vector<ptrdiff_t> m7;

    ptrdiff_t N=static_cast<ptrdiff_t>(img.Size());
    for (ptrdiff_t i=0; i<N; i++) 
    {
        m_diff[i]=LoG[i]-LoGm3[i];
        m_mask[i]=(m_fThreshold3<m_diff[i]) + ((m_fThreshold5<m_diff[i])*2) + ((m_fThreshold7<m_diff[i])*4);
        
        switch (static_cast<int>(m_mask[i])) 
        {
            case 0: break;
            case 1: m3.push_back(i); break;
            case 3: m5.push_back(i); break;
            case 7: m7.push_back(i); break;
            default:
                msg.str(""); msg<<"Strange mask code ("<<m_mask[i]<<") found at position"<<i;
                throw ImagingException(msg.str(),__FILE__,__LINE__);
        }
    }

    msg.str(""); msg<<"Gamma spots detected: "<<m3.size()<<" (3x3), "<<m5.size()<<" (5x5), "<<m7.size()<<" (7x7)";
    m_logger.message(msg.str());

    // AK: This dilation map is useless!!! It is a void operation.
    //    ; we found that some of the edge pixels are not removed. so we dilate the map7
    //    if total(imgthr7) gt 0 then begin
    //        s=[[1,1,1],[1,1,1],[1,1,1]]     ;the dilate structure
    //        single7=smooth(imgthr7*255,3) lt 30
    //        single7=single7 and imgthr7     ;those single pixels in threshold map7
    //        imgthr7=imgthr7 - single7
    //        imgthr7=dilate(imgthr7,s) + single7
    //    endif
    // The mask implemented mask coding renders this process obsolete.
    //    imgthr5=(imgthr5 or imgthr7) - imgthr7
    //    imgthr3=imgthr3-imgthr5

    // kipl::base::TImage<float,2> imgm3=med3(img);
    kipl::base::TImage<float,2> res=img;
    res.Clone();

    std::list<ptrdiff_t>::iterator it;
    float *pRes=res.GetDataPtr();
    float *pImg=img.GetDataPtr();


    for (const auto &pos : m3) 
    {
        medianNeighborhood(pImg,pRes,pos,m_nNG3);
    }

    for (const auto &pos : m5) {
        medianNeighborhood(pImg,pRes,pos,m_nNG5);
    }

    for (const auto &pos : m7) {
        medianNeighborhood(pImg,pRes,pos,m_nNG7);
    }

    img=res;
}

void GammaClean::prepareNeighborhoods(const std::vector<size_t> &dims)
{
    for (ptrdiff_t i=-1,idx=0; i<=1; i++)
        for (ptrdiff_t j=-1; j<=1; j++, idx++)
            m_nNG3[idx]=i*dims[0]+j;

    for (ptrdiff_t i=-2,idx=0; i<=2; i++)
        for (ptrdiff_t j=-2; j<=2; j++, idx++)
            m_nNG5[idx]=i*dims[0]+j;

    for (ptrdiff_t i=-3,idx=0; i<=3; i++)
        for (ptrdiff_t j=-3; j<=3; j++, idx++)
            m_nNG7[idx]=i*dims[0]+j;

}

void GammaClean::medianNeighborhood(float *pImg, float *pRes, ptrdiff_t pos, const std::vector<ptrdiff_t> &ng)
{
    std::vector<float> medvec;
    medvec.reserve(ng.size());

    for (const auto &i : ng) 
    {
        ptrdiff_t p=pos+i;

        if ((0<=p) && (p<m_nData)) 
        {
            medvec.push_back(pImg[p]);
        }
    }

    pRes[pos]=kipl::math::median_STL(medvec);
}

kipl::base::TImage<float,2> GammaClean::detectionImage() 
{
    return m_diff;
}

kipl::base::TImage<unsigned short,2> GammaClean::spotMask() 
{
    return m_mask;
}

}

// The original IDL implementation
//; gamma spots removal for Neutron radiography and tomography images
//; using 'find and replace' strategy, this is a discriminative method,
//; better than unique threshold substitution
//;
//;Find the gamma spots by laplacian or LOG operation, which are usually utilized for
//;edge finding. Those greatly changing area will produce high level in the resultant image. This can work very well for those high level gamma spots.
//;For those relatively low level gamma spots, a lower threshold is needed. But,
//;some steep edges also have very high values. This make it a hard choice: whether filter out those
//;low level noise at some sacrifice of the edge info, or keep the edge intact as well as those low level noise.
//;
//; adptive thresholding : med3(log)+ thr
//;based on the fact that: the edges of the obj usually have a width of more than 5 pixels,
//;while the gamma spots usually involve less then 3 pixels in width. By filtering the
//;resultant image from LOG(laplacian of gaussian) filtering with a 3by3 median kernel, the edges
//;will survive, while most of the gamma pixels lost their magnitude drastically, some are wiped out.
//; Adaptive size of median filter:
//;   3 by 3 for small spots, 5 by 5 for medium ones, 7 by 7 for high level ones.

//;written by Hongyun Li, visiting physicist at FRM2,TUM,Germany, Feb 09, 2006
//;   Contact info:
//;     hongyunlee@yahoo.com, or lihongyun03@mails.tsinghua.edu.cn
//;     Northwest Institute of Nuclear Technology, China
//;
//;Version: fast . Do median5 and median7 when necessary.
//;
//;   The test info:
//;       5.4060001 seconds used for D:\NR\Norm_0001.tif
//;Gamma rem:     1332941.  pixels substituted.      31.7798% of total pixels.
//;M3:     1004177,      23.9414%.  M5:      284016,      6.77147%. M7:       44748,      1.06688%


//function gam_rem_adp_log, img, thr3=thr3,thr5=thr5,thr7=thr7, sig_log=sig_log
//    ;check the parameters
//    if n_elements(img) le 100 then return, img
//    if n_elements(sig_log) le 0    then sig_log=0.8 ; the default values. work very well for our images
//    if n_elements(thr3) le 0   then thr=25
//    if n_elements(thr5) le 0   then thr=100
//    if n_elements(thr7) le 0   then thr=400

//    f_log=-log(9,sig_log)      ;create the kernel of LOG filter

//    img_log=convol(float(img),f_log)  ;do the LOG filter
//    img_logm3=median(img_log,3)      ;median the LOG edge enhanced image, 3 by 3 is good enough

//    ;----------substitute only those pixels whose values are greater than adaptive threshold
//    ;----------  ,which is set to median(log(img))+thr, where thr is a predetermined constant chosed
//    ;----------  to be best fitted for specific noise charateristics by user.
//    ;Adaptive filter size:
//    ;"Opening" operator:
//    imgthr3=img_log gt img_logm3+thr3
//    imgthr5=img_log gt img_logm3+thr5
//    imgthr7=img_log gt img_logm3+thr7

//    ; we found that some of the edge pixels are not removed. so we dilate the map7
//    if total(imgthr7) gt 0 then begin
//        s=[[1,1,1],[1,1,1],[1,1,1]]     ;the dilate structure
//        single7=smooth(imgthr7*255,3) lt 30
//        single7=single7 and imgthr7     ;those single pixels in threshold map7
//        imgthr7=imgthr7 - single7
//        imgthr7=dilate(imgthr7,s) + single7
//    endif
//    imgthr5=(imgthr5 or imgthr7) - imgthr7

//    imgthr3=imgthr3-imgthr5

//    sz=size(img)
//    m=sz[1] &   n=sz[2]
//    ; clean the border of map5 and map7, otherwise there might be out of range error
//    ; when doing the replacement
//    imgthr7[0:2,*]=0 & imgthr7[*,0:2]=0 & imgthr7[*,n-3:n-1]=0 & imgthr7[m-3:m-1,*]=0

//    imgthr5[0:1,*]=0 & imgthr5[*,0:1]=0 & imgthr5[*,n-2:n-1]=0 & imgthr5[m-2:m-1,*]=0

//    imgm3=median(img,3)        ;3 by 3 median filtering, as substitution image
//    img_adp=img
//    img_adp[where(imgthr3,n3)]=imgm3[where(imgthr3)]

//    index=where(imgthr5,n5)

//    if index[0] ge 0 then  begin
//        coor=ARRAY_INDICES(img,index)

//        for i=0L, n5-1 do begin
//            img_adp[index[i]]=median(img[coor[0,i]-2:coor[0,i]+2, coor[1,i]-2:coor[1,i]+2])
//        endfor
//    endif

//    index=where(imgthr7,n7)
//    if index[0] ge 0 then  begin
//        coor=ARRAY_INDICES(img,index)
//        for i=0L, n7-1 do begin
//            img_adp[index[i]]=median(img[coor[0,i]-3:coor[0,i]+3, coor[1,i]-3:coor[1,i]+3])
//        endfor
//    endif
//    n=n3+n5+n7  ;total(imgthr3)+total(imgthr5)+total(imgthr7)

//    print,'Gamma rem:',n,'.  pixels substituted.', n*100.0/n_elements(img), '% of total pixels.'
//    print,'M3:',n3,',',n3*100.0/n_elements(img),'%.  M5:', n5,',', n5*100.0/n_elements(img),'%. M7:',n7,',',n7*100.0/n_elements(img),'%'

//    ; adaptive size

//    return, img_adp
//end
