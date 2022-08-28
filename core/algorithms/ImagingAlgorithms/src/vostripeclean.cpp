//<LICENSE>
#include "../include/vostripeclean.h"
#include <base/timage.h>
#include <list>
#include <vector>
#include <numeric>

#include <base/tpermuteimage.h>
#include <filters/filter.h>
#include <filters/medianfilter.h>
#include <base/tprofile.h>
#include <math/mathfunctions.h>
#include <math/linfit.h>
#include <stltools/stlvecmath.h>
#include <stltools/stlmorphology.h>
#include <stltools/zerocrossings.h>
#include <io/io_serializecontainers.h>
#include <io/io_tiff.h>

namespace ImagingAlgorithms
{


VoStripeClean::VoStripeClean() :
    logger("VoStripeClean"),
    saveSteps(true)
{

}

kipl::base::TImage<float,2> VoStripeClean::removeStripeBasedSorting(kipl::base::TImage<float,2> &sinogram, size_t size, bool doTranspose)
{
//    """
//    Algorithm 3 in the paper. Remove stripes using the sorting technique.
//    Work particularly well for removing partial stripes.
//    Angular direction is along the axis 0
//    ---------
//    Parameters: - sinogram: 2D array.
//                - size: window size of the median filter.
//    ---------
//    Return:     - stripe-removed sinogram.
//    """
//    sinogram = np.transpose(sinogram)
    kipl::base::TImage<float,2> tsinogram = transpose(sinogram,true);

    //    sinosorted = np.sort(sinogram, axis=0)

    for (size_t i=0; i<tsinogram.Size(1); ++i)
    {
        auto pLine = tsinogram.GetLinePtr(i);
        std::multimap<float,size_t> m;
        for (size_t j=0; j<tsinogram.Size(0); ++j)
            m.insert(make_pair(pLine[j],j));
        std::vector<float> v;
        for (const auto & item: m)
            v.push_back(item.first);

        auto vm=medianFilter(v,size);
        size_t k=0;
        for (const auto & item: m)
        {
            pLine[item.second]=vm[k];
            ++k;
        }
    }

    //    sinosmoothed = median_filter(sinosorted, (1, size))



//    (nrow, ncol) = sinogram.shape
//    listindex = np.arange(0.0, ncol, 1.0)
//    matindex = np.tile(listindex,(nrow,1))
//    matcomb = np.asarray(np.dstack((matindex, sinogram)))
//    matsort = np.asarray(
//        [row[row[:, 1].argsort()] for row in matcomb])
//    matsort[:, :, 1] = median_filter(matsort[:, :, 1],(size,1))
//    matsortback = np.asarray(
//        [row[row[:, 0].argsort()] for row in matsort])
//    sino_corrected = matsortback[:, :, 1]
//    return np.transpose(sino_corrected)

   return transpose(tsinogram,true);
}

kipl::base::TImage<float,2> VoStripeClean::removeStripeBasedFiltering(kipl::base::TImage<float,2> &sinogram, float sigma, size_t size, bool doTranspose)
{
//    """
//    Algorithm 2 in the paper. Remove stripes using the filtering technique.
//    Angular direction is along the axis 0
//    ---------
//    Parameters: - sinogram: 2D array.
//                - sigma: sigma of the Gaussian window which is used to separate
//                        the low-pass and high-pass components of the intensity
//                        profiles of each column.
//                - size: window size of the median filter.
//    ---------
//    Return:     - stripe-removed sinogram.
//    """
//    pad = 150 # To reduce artifacts caused by FFT
//    sinogram = np.transpose(sinogram)
//    sinogram2 = np.pad(sinogram,((0, 0),(pad, pad)), mode = 'reflect')
//    (_, ncol) = sinogram2.shape
//    window = signal.gaussian(ncol, std = sigma)
//    listsign = np.power(-1.0,np.arange(ncol))
//    sinosmooth = np.zeros_like(sinogram)
//    for i,sinolist in enumerate(sinogram2):
//        #sinosmooth[i] = np.real(ifft(fft(sinolist*listsign)*window)*listsign)[pad:ncol-pad]
//        sinosmooth[i] = np.real(fft.ifft(fft.fft(sinolist*listsign)*window)*listsign)[pad:ncol-pad]
//    sinosharp = sinogram - sinosmooth
//    sinosmooth_cor = median_filter(sinosmooth,(size,1))
//    return np.transpose(sinosmooth_cor + sinosharp)
    auto res = transpose(sinogram,doTranspose);

    return transpose(res,doTranspose);
}


kipl::base::TImage<float,2> VoStripeClean::_2dWindowEllipse(size_t height, size_t width, float sigmax, float sigmay)
{
//    """
//    Create a 2D Gaussian window for Fourier filtering.
//    ---------
//    Parameters: - height, width: shape of the window.
//                - sigmax, sigmay: sigmas of the window.
//    ---------
//    Return:     - 2D window.
//    """
//    centerx = (width-1.0)/2.0
//    centery = (height-1.0)/2.0
//    y,x = np.ogrid[-centery:height-centery,-centerx:width-centerx]
//    numx = 2.0*sigmax*sigmax
//    numy = 2.0*sigmay*sigmay
//    win2d = np.exp(-(x*x/numx+y*y/numy))
//    return win2d
//    std::vector<size_t> dims={static_cast<size_t>(ceil(sigmax*2.5)*2+1),
//                              static_cast<size_t>(ceil(sigmay*2.5)*2+1)};

    std::vector<size_t> dims={width,
                              height};
    kipl::base::TImage<float,2> win2d(dims);
    float centerx   = dims[0]/2.0f;
    float centery   = dims[1]/2.0f;

    float sigmax2 = 2.0f * sigmax * sigmax ;
    float sigmay2 = 2.0f * sigmay * sigmay ;

    for (size_t y = 0; y<dims[1]; ++y)
        for (size_t x = 0 ; x<dims[0]; ++x)
        {
            float xx = x-centerx;
            float yy = y-centery;
            win2d(x,y) = exp(-( xx*xx/sigmax2 + yy*yy/sigmay2 ));
        }

    return win2d;
}

kipl::base::TImage<float,2> VoStripeClean::_2dFilter(kipl::base::TImage<float,2> mat, float sigmax, float sigmay, size_t pad)
{
//    """
//    Filtering an image using 2D Gaussian window.
//    ---------
//    Parameters: - mat: 2D array.
//                - sigmax, sigmay: sigmas of the window.
//                - pad: padding for FFT
//    ---------
//    Return:     - filtered image.
//    """
//    matpad = np.pad(mat,((0, 0), (pad, pad)), mode = 'edge')
//    matpad = np.pad(matpad,((pad, pad), (0, 0)), mode = 'mean')
//    (nrow, ncol) = matpad.shape
//    win2d = _2d_window_ellipse(nrow, ncol, sigmax, sigmay)
//    listx = np.arange(0, ncol)
//    listy = np.arange(0, nrow)
//    x,y = np.meshgrid(listx, listy)
//    matsign = np.power(-1.0,x+y)
//    #matfilter = np.real(ifft2(fft2(matpad*matsign)*win2d)*matsign)
//    matfilter = np.real(fft.ifft2(fft.fft2(matpad*matsign)*win2d)*matsign)
//    return matfilter[pad:nrow-pad,pad:ncol-pad]
    kipl::base::TImage<float,2> res;

    return res;
}

kipl::base::TImage<float, 2> VoStripeClean::transpose(kipl::base::TImage<float,2> &img,bool doIt)
{

    kipl::base::TImage<float,2> res;

    if (doIt)
    {
        kipl::base::Transpose<float> tr;

        res = tr(img);
    }
    else
        res = img;

    return res;
}


kipl::base::TImage<float,2> VoStripeClean::removeStripeBasedFitting(kipl::base::TImage<float, 2> &sinogram, int order, float sigmax, float sigmay, bool doTranspose)
{
//    Algorithm 1 in the paper. Remove stripes using the fitting technique.
//    Angular direction is along the axis 0

//    (nrow, _) = sinogram.shape
//    nrow1 = nrow
//    if nrow1%2==0:
//        nrow1 = nrow1 - 1
//    sinofit = savgol_filter(sinogram, nrow1, order, axis=0,mode = 'mirror')
//    sinofitsmooth = _2d_filter(sinofit, sigmax, sigmay, 50)
//    num1 = np.mean(sinofit)
//    num2 = np.mean(sinofitsmooth)
//    sinofitsmooth = num1*sinofitsmooth/num2
//    return sinogram/sinofit*sinofitsmooth
    auto res = transpose(sinogram,doTranspose);

    return res;
}

std::vector<float> VoStripeClean::detect_stripe(std::vector<float> &listdata, float snr)
{
//    """
//    Algorithm 4 in the paper. Used to locate stripe positions.
//    ---------
//    Parameters: - listdata: 1D normalized array.
//                - snr: ratio used to discriminate between useful
//                    information and noise.
//    ---------
//    Return:     - 1D binary mask.
//    """
    std::ostringstream msg;

    size_t numdata = listdata.size();

    //    listsorted = np.sort(listdata)[::-1]
    std::vector<float> listsorted(listdata.begin(),listdata.end());
    std::sort(listsorted.begin(),listsorted.end(), greater<float>());
    kipl::io::serializeContainer(listsorted.begin(),listsorted.end(),"Vo_sorted.txt");
    //    xlist = np.arange(0, numdata, 1.0)
    auto xlist = std::vector<float>(numdata,0.0f);
    std::iota(xlist.begin(),xlist.end(),0.0f);

    //    ndrop = np.int16(0.25 * numdata)
    size_t ndrop = numdata / 4;

    //    (_slope, _intercept) = np.polyfit(
    //        xlist[ndrop:-ndrop-1], listsorted[ndrop:-ndrop - 1], 1)
    std::vector<float> xx(xlist.begin()+ndrop,xlist.begin()+numdata-2*ndrop);
    std::vector<float> ll(listsorted.begin()+ndrop,listsorted.begin()+numdata-2*ndrop);
    auto coefs = kipl::math::polyFit(xx,ll,1);

    //    numt1 = _intercept + _slope * xlist[-1]
    //    noiselevel = np.abs(numt1 - _intercept)
    //    val1 = np.abs(listsorted[0] - _intercept) / noiselevel
    //    val2 = np.abs(listsorted[-1] - numt1) / noiselevel
    float numt1      = coefs[0]+coefs[1] * xlist.back();
    float noiselevel = fabs(numt1 - coefs[0]);
    float val1       = fabs(ll[0] - coefs[0]) / noiselevel;
    float val2       = fabs(ll.back() - numt1) / noiselevel;
    float lower_thresh = numt1 - noiselevel * snr * 0.5;
    float upper_thresh = coefs[0] + noiselevel * snr * 0.5;

    msg.str("");
    msg<<"Variables\n listsorted[0]: "<<listsorted[0]
      <<"\n coefs: ("<<coefs[0]<<","<<coefs[1]
      <<")\n numt1: "<<numt1
      <<"\n noiselevel: "<<noiselevel
      <<"\n val1: "<<val1<<", val2: "<<val2
      <<"\n thresholds: ("<<lower_thresh<<", "<<upper_thresh<<")";

    logger.message(msg.str());
    //    listmask = np.zeros_like(listdata)
    std::vector<float> listmask(listdata.size(),0.0f);
    //    if (val1 >= snr):
    //        upper_thresh = _intercept + noiselevel * snr * 0.5
    //        listmask[listdata > upper_thresh] = 1.0
    if (val1 >=snr)
    {

        //        listmask[listdata > upper_thresh] = 1.0

        for (size_t i=0; i<listdata.size(); ++i)
            if (listdata[i] > upper_thresh)
                listmask[i] = 1.0;
    }

    //    if (val2 >= snr):
    //        lower_thresh = numt1 - noiselevel * snr * 0.5
    //        listmask[listdata <= lower_thresh] = 1.0
    if (val2 >=snr)
    {
        for (size_t i=0; i<listdata.size(); ++i)
            if (listdata[i] <= lower_thresh)
                listmask[i] = 1.0;
    }

    return listmask;
}

void VoStripeClean::interpolationFill(kipl::base::TImage<float, 2> &img, std::vector<float> &mask)
{
    if (mask.size()!=img.Size(0))
    {
        throw kipl::base::DimsException("Mask doesn't match image width in interpolationFill",__FILE__,__LINE__);
    }

    auto interpolationBlocks = findContinuousBlocks(mask);

    std::ostringstream msg;
    msg<<"Found "<<interpolationBlocks.size()<<" blocks to interpolate.";
    logger.message(msg.str());

    if ( !interpolationBlocks.empty())
    {
        for (size_t y=0; y<img.Size(1); ++y)
        {
            float *pLine = img.GetLinePtr(y);
            for (auto & block : interpolationBlocks)
            {
                float k = (pLine[block.second]-pLine[block.first-1])/(block.second - block.first + 1);
                float m = pLine[block.first-1] - k * (block.first-1);
                for (ptrdiff_t x = block.first; x< block.second; ++x)
                {
                    pLine[x] = k*x +m;
                }
            }
        }
    }
    else
    {
        logger.message("interpolationFill didn't find any blocks to fill");
        return ;
    }

}

void VoStripeClean::writeImage(kipl::base::TImage<float, 2> &img, const std::string & fname)
{
    if (saveSteps)
        kipl::io::WriteTIFF(img,fname,kipl::base::Float32);
}

void VoStripeClean::writeVector(std::vector<float> &v, const string &fname)
{
    if (saveSteps)
        kipl::io::serializeContainer(v.begin(),v.end(),fname);
}

kipl::base::TImage<float,2> VoStripeClean::removeLargeStripe(kipl::base::TImage<float,2> &sinogram, float snr, size_t size, bool doTranspose)
{
//    Algorithm 5 in the paper. Remove large stripes by: locating stripes,
//    normalizing to remove full stripes, using the sorting technique to
//    remove partial stripes.
//    Angular direction is along the axis 0.
    std::ostringstream msg;

    float  badpixelratio = 0.05f;
    size_t ndrop         = static_cast<size_t>(badpixelratio * sinogram.Size(1));

    auto res=sinogram; res.Clone();

    kipl::base::TImage<float,2> tsinogram = transpose(sinogram,true);

    //    sinosorted = np.sort(sinogram, axis=0)
    kipl::base::TImage<float,2> sinosorted(tsinogram);
    sinosorted.Clone();

    for (size_t y=0; y<tsinogram.Size(1); ++y)
        std::sort(tsinogram.GetLinePtr(y),tsinogram.GetLinePtr(y)+tsinogram.Size(0));

    //    sinosmoothed = median_filter(sinosorted, (1, size))
    //kipl::filters::TMedianFilter<float,2> medfilt({size,3});
    kipl::filters::TMedianFilter<float,2> medfilt({3,size});
    auto sinosmoothed = medfilt(sinosorted,kipl::filters::FilterBase::EdgeMirror);

    //    list1 = np.mean(sinosorted[ndrop:nrow - ndrop], axis=0)
    auto list1 = kipl::base::projection2D(sinosorted.GetDataPtr(),sinosorted.dims(),0,true,{ndrop,sinosorted.Size(0)-2*ndrop});
    writeVector(list1,"Vo_large_list1.txt");

    //    list2 = np.mean(sinosmoothed[ndrop:nrow - ndrop], axis=0)
    auto list2 = kipl::base::projection2D(sinosmoothed.GetDataPtr(),sinosmoothed.dims(),0,true,{ndrop,sinosmoothed.Size(0)-2*ndrop});
    writeVector(list2,"Vo_large_list2.txt");

    //    listfact = list1 / list2
    std::vector<float> listfact(list1.size(),0.0f);
    for (size_t i = 0; i < listfact.size(); ++i)
        listfact[i]=list1[i]/list2[i];

    writeVector(listfact,"Vo_large_listfact.txt");

    //    listmask = detect_stripe(listfact, snr)
    auto listmask = detect_stripe(listfact, snr);
    writeVector(listmask,"Vo_large_listmask.txt");

//    listmask = binary_dilation(listmask, iterations=1).astype(listmask.dtype)
    listmask = binaryDilation(listmask,1);
    writeVector(listmask,"Vo_large_listmask2.txt");

//    matfact = np.tile(listfact,(nrow,1))
//    sinogram = sinogram / matfact
     msg.str("");
     msg<<"res.size: ("<<res.Size(0)<<", "<<res.Size(1)<<"), listmask.size:"<<listmask.size();
     logger.message(msg.str());
    for (size_t i=0; i<res.Size(1); ++i )
    {
        auto pLine = res.GetLinePtr(i);
        for (size_t j=0; j<res.Size(0); ++j)
            pLine[j] = pLine[j]/listfact[j];
    }
    writeImage(res,"Vo_large_sinoscaled.tif");

    tsinogram = transpose(res,true);
    int cnt=0;
    for (size_t i=0; i<listmask.size(); ++i)
    {
        if (0.0f<listmask[i])
        {
            cnt++;
            auto pLine  = tsinogram.GetLinePtr(i);
            auto pSLine = sinosmoothed.GetLinePtr(i);

            std::multimap<float,size_t> sorted_vector;
            for (size_t j=0; j<tsinogram.Size(0); ++j)
                sorted_vector.insert(std::make_pair(pLine[j],j));


            for (const auto & item : sorted_vector)
            {
                pLine[item.second]=pSLine[item.second];
            }
        }
    }

    std::cout<<"cnt="<<cnt<<"\n";
    res = transpose(tsinogram,true);


//    sinogram1 = np.transpose(sinogram)
//    listindex = np.arange(0.0, nrow, 1.0)
//    matindex = np.tile(listindex,(ncol,1))
//    matcombine = np.asarray(np.dstack((matindex, sinogram1)))
//    matsort = np.asarray(
//        [row[row[:, 1].argsort()] for row in matcombine])
//    matsort[:, :, 1] = np.transpose(sinosmoothed)
//    matsortback = np.asarray(
//        [row[row[:, 0].argsort()] for row in matsort])
//    sino_corrected = np.transpose(matsortback[:, :, 1])
//    listxmiss = np.where(listmask > 0.0)[0]
//    sinogram[:, listxmiss] = sino_corrected[:, listxmiss]
//    return sinogram

    return res;
}

kipl::base::TImage<float,2> VoStripeClean::removeUnresponsiveAndFluctuatingStripe(kipl::base::TImage<float,2> &sinogram, float snr, size_t size)
{
    // Algorithm 6 in the paper
    kipl::base::TImage<float,2> res;
    res.Clone(sinogram);

    //    sinosmoothed = np.apply_along_axis(uniform_filter1d, 0, sinogram, 10)
    std::vector<float>  kernel(10,1.0f/10.0f);
    std::vector<size_t> kDims={1,10};
    kipl::filters::TFilter<float,2> vsmooth(kernel,kDims);

    auto sinosmoothed = vsmooth(res,kipl::filters::FilterBase::EdgeMirror); //OK
    writeImage(sinosmoothed,"Vo_ssino.tif");

//    //    listdiff = np.sum(np.abs(sinogram - sinosmoothed), axis=0)
    auto absdiff  = kipl::math::absDiff(res,sinosmoothed); // OK
    writeImage(absdiff,"Vo_absdiff.tif");

    auto listdiff = kipl::base::projection2D(absdiff.GetDataPtr(),absdiff.dims(),1,false);
    writeVector(listdiff,"Vo_listdiff.txt");
    //    nmean = np.mean(listdiff)
    float nmean = std::accumulate(listdiff.begin(),listdiff.end(),0.0f)/listdiff.size();

    //    listdiffbck = median_filter(listdiff, size)
    auto listdiffbck = medianFilter(listdiff,size);


    //    listdiffbck[listdiffbck == 0.0] = nmean
    for (auto & item : listdiffbck)
        if (item==0.0)
            item = nmean;
    writeVector(listdiffbck,"Vo_listdiffbck.txt");

    //    listfact = listdiff / listdiffbck
    std::vector<float> listfact=listdiff;

    for (size_t i=0; i<listfact.size(); ++i)
        listfact[i]=listfact[i]/listdiffbck[i];
    writeVector(listfact,"Vo_listfact.txt");

    //    listmask = detect_stripe(listfact, snr)
    auto listmask = detect_stripe(listfact, snr);
    writeVector(listmask,"Vo_listmask.txt");

    //    listmask = binary_dilation(listmask, iterations=1).astype(listmask.dtype)
    listmask = binaryDilation(listmask, 1); 
    std::fill(listmask.begin(),listmask.begin()+2,0.0f);
    std::fill(listmask.rbegin(),listmask.rbegin()+2,0.0f);
    writeVector(listmask,"Vo_listmask2.txt");
    //    listx = np.where(listmask < 1.0)[0]
    //    listy = np.arange(nrow)
    //    matz = sinogram[:, listx]
    //    finter = interpolate.interp2d(listx, listy, matz, kind='linear')
    //    listxmiss = np.where(listmask > 0.0)[0]
    //    if len(listxmiss) > 0:
    //        matzmiss = finter(listxmiss, listy)
    //        sinogram[:, listxmiss] = matzmiss


    interpolationFill(res,listmask);
    writeImage(res,"Vo_uResult.tif");

    return res;
}

kipl::base::TImage<float,2> VoStripeClean::removeAllStripes(kipl::base::TImage<float,2> &sinogram, float snr, size_t la_size, size_t sm_size, bool doTranspose)
{
    auto res = transpose(sinogram,doTranspose);

    res = removeUnresponsiveAndFluctuatingStripe(sinogram, snr, la_size);
    res = removeLargeStripe(res, snr, la_size,false);
    res = removeStripeBasedSorting(res, sm_size,false);

    return transpose(res,doTranspose);
}

//# ----------------------------------------------------------------------------
//# Example of use:
//# sinogram = remove_stripe_based_sorting(sinogram, 31)
//# sinogram = remove_stripe_based_filtering(sinogram, 3, 31)
//# sinogram = remove_stripe_based_fitting(sinogram, 1, 5, 20)
//# sinogram = remove_unresponsive_and_fluctuating_stripe(sinogram1, 3, 81)
//# sinogram = remove_large_stripe(sinogram1, 3, 81)
//# sinogram = remove_all_stripe(sinogram, 3, 81, 31)
}
