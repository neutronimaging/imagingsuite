//<LICENSE>

#include "SegmentBB.h"
#include <base/KiplException.h>
#include <ImagingException.h>
#include <strings/string2array.h>
#include <filters/medianfilter.h>
#include <morphology/morphfilters.h>
#include <morphology/label.h>
#include <segmentation/thresholds.h>
#include <base/thistogram.h>
#include <io/io_tiff.h>

SegmentBB::SegmentBB() :
    logger("SegmentBB"),
    m_fRadius(5.0f),
    m_areaLimits({10,1000}),
    m_segmentationMethod(method_otsu),
    m_fThreshold(0.0f)
{

}

SegmentBB::SegmentBB(const std::map<std::string, std::string> &params ) :
    logger("SegmentBB")
{
    setParameters(params);
}

void SegmentBB::setDotRadius(float radius)
{
    if (radius<1)
        kipl::base::KiplException("Radius less than 1",__FILE__,__LINE__);

    m_fRadius = radius;
}

void SegmentBB::setDotAreaLimits(const std::vector<size_t> & limits)
{
    if (limits.size() !=2)
        ImagingException("Area limits vector must have exactly two elements",__FILE__,__LINE__);

    m_areaLimits = limits;

    if (m_areaLimits[1]<m_areaLimits[0])
        swap(m_areaLimits[1],m_areaLimits[0]);
}

void SegmentBB::setSegmentationMethod(eSegmentationMethod method, float threshold)
{
    m_segmentationMethod = method;
    m_fThreshold = threshold;
}


std::map<std::string, std::string> SegmentBB::parameters()
{
    std::map<std::string, std::string> params;

    params["maskradius"]         = std::to_string(m_fRadius);
    params["arealimits"]         = kipl::strings::Vector2String(m_areaLimits);
    params["segmentationmethod"] = enum2string(m_segmentationMethod);
    params["threshold"]          = std::to_string(m_fThreshold);

    return params;
}

void SegmentBB::setParameters(const std::map<std::string, std::string> &params)
{
    std::ignore = params;
}

void SegmentBB::exec( const kipl::base::TImage<float,2> & bb, 
                            kipl::base::TImage<float,2> & mask,
                      const std::vector<size_t> & ROI)
{   
    // 1. Enhance/flatten: subtract median filtered image from ImagingException
    // 2. Get histogram
    // 3. Find threshold  
    // 4. Apply threshold 
    // 5. Morph open and close 
    segment(bb, mask);
    // kipl::io::WriteTIFF(mask,"segmented_mask.tif");

    // 6. Label image 
    // 7. Get region properties
    // 8. Filter on size, position, and intensity 
    identifyBBs(bb, mask, ROI);

    // 9. Create dot mask
    prepareMask(bb.dims());
    // kipl::io::WriteTIFF(m_mask,"prepared_mask.tif");

}

void SegmentBB::segment(const kipl::base::TImage<float,2> &bb,
                              kipl::base::TImage<float,2> & mask)
{
    std::ostringstream msg;
    if ((m_segmentationMethod == method_usermask) && 
        (bb.Size(0)==mask.Size(0)) && 
        (bb.Size(0)==mask.Size(1)))
    {   
        // Return user defined mask if it has the same size as the bb image
        // Otherwise, segment using default segmentation method
        return;
    }

    // 2.a compute histogram
    
    kipl::filters::TMedianFilter<float,2> medh({71,1});
    kipl::filters::TMedianFilter<float,2> medv({1,71});

    auto fltv = medv(bb, kipl::filters::FilterBase::EdgeZero);
    auto flat = medh(fltv,kipl::filters::FilterBase::EdgeZero);

    flat-=bb; 
    // kipl::io::WriteTIFF(flat,"flat_img.tif");

    std::vector<size_t> hist;
    std::vector<float>  axis;
    int ndims = 256;
    kipl::base::Histogram(flat.GetDataPtr(), flat.Size(), ndims, hist, axis, 0.0f,0.0f, true);

    // 3. Find threshold  
    // 4. Apply threshold 
    int idx = 0;
    mask.resize(bb.dims());

    switch (m_segmentationMethod)
    {
        case SegmentBB::method_usermask :
            logger.warning("Provided user mask size didn't match bb image size. Using Otsu to segement BB.");

        case SegmentBB::method_otsu :  
            idx = kipl::segmentation::Threshold_Otsu(hist);
            msg.str(""); msg<<"Otsu threshold at idx:"<<idx<<", level:"<<axis[idx];
            logger.message(msg.str());
            kipl::segmentation::Threshold( bb.GetDataPtr(), 
                                mask.GetDataPtr(), 
                                bb.Size(), 
                                axis[idx],
                                kipl::segmentation::cmp_less);
            break;
        case SegmentBB::method_rosin :
            idx = kipl::segmentation::Threshold_Rosin(hist);
            kipl::segmentation::Threshold( bb.GetDataPtr(), 
                                           mask.GetDataPtr(), 
                                           bb.Size(), 
                                           axis[idx],
                                           kipl::segmentation::cmp_less);

            break;
        case SegmentBB::method_manual :
            kipl::segmentation::Threshold( bb.GetDataPtr(), 
                                           mask.GetDataPtr(), 
                                           bb.Size(), 
                                           m_fThreshold,
                                           kipl::segmentation::cmp_less);

            break;


        // case ImagingAlgorithms::ReferenceImageCorrection::referenceFreeMask :
        //     break;
        default :
            throw ImagingException("Inappropriate mask choice in SegmentBlackBody, you shouldn't reach this point.",__FILE__,__LINE__);
    }

    // 5. Morph open and close 
    std::vector<float> kernel(25,1.0f);
    kipl::morphology::TDilate<float,2> D(kernel,{5,5});
    kipl::morphology::TErode<float,2>  E(kernel,{5,5});

    mask = E(mask,kipl::filters::FilterBase::EdgeMirror);
    mask = D(mask,kipl::filters::FilterBase::EdgeMirror);

    mask = D(mask,kipl::filters::FilterBase::EdgeMirror);
    mask = E(mask,kipl::filters::FilterBase::EdgeMirror);
}

void SegmentBB::identifyBBs(const kipl::base::TImage<float,2> &bb,
                            const kipl::base::TImage<float,2> &mask,
                            const std::vector<size_t> & ROI)
{
    // 6. Label image 
    kipl::base::TImage<int,2> lbl(mask.dims());
    
    size_t num_obj = 0UL;

    try {
         num_obj = kipl::morphology::LabelImage(mask,lbl);

         std::ostringstream msg;
         msg << "Labeling found " << num_obj << "objects in the mask.";
         logger.debug(msg.str());
     }
     catch (ImagingException &e) {
         logger.error(e.what());
         throw ImagingException("kipl::morphology::LabelImage failed", __FILE__, __LINE__);
     }
     catch(kipl::base::KiplException &e){
         logger.error(e.what());
         throw kipl::base::KiplException("kipl::morphology::LabelImage failed", __FILE__, __LINE__);
     }
     catch (std::exception &e)
     {
         logger.error(e.what());
         throw ImagingException("kipl::morphology::LabelImage failed", __FILE__, __LINE__);
     }

    if (num_obj<=2)
         throw ImagingException("SegmentBlackBodyNorm failed, Number of detected objects too little. Please try to change the threshold", __FILE__, __LINE__);
    // 7. Get region properties
    
    kipl::morphology::RegionProperties<int,float> rp(lbl,bb);

    rp.filter(kipl::morphology::regprop_area,{static_cast<float>(m_areaLimits[0]),static_cast<float>(m_areaLimits[1])});           // upper limit is the image size
    if (ROI.size() == 4) 
    {
        rp.filter(kipl::morphology::regprop_cogx,{static_cast<float>(ROI[0]),static_cast<float>(ROI[2])});
        rp.filter(kipl::morphology::regprop_cogy,{static_cast<float>(ROI[1]),static_cast<float>(ROI[3])});
    }
    
    m_regProps = rp;
}

void SegmentBB::prepareMask(const std::vector<size_t> & dims)
{
    m_mask.resize(dims);
    auto cogs = m_regProps.cog();

    for (const auto & cog : cogs)
    {
        auto x0 = cog.second[0];
        auto y0 = cog.second[1];

        for (float y=-m_fRadius; y<=m_fRadius; y++)
        {
            for (float x=-m_fRadius; x<=m_fRadius; x++)
            {
                float R = sqrt( x*x + y*y );

                if ( R<=m_fRadius )
                {
                    m_mask(static_cast<int>(x+x0+0.5),static_cast<int>(y+y0+0.5))=1.0f;
                }
            }
        }
    }
}

const kipl::base::TImage<float,2> & SegmentBB::mask()
{
    return m_mask;
}

std::tuple<std::vector<float>,std::vector<float> > SegmentBB::dotCoordinates()
{

}

std::pair<std::vector<size_t>, std::vector<float> >  SegmentBB::histogram()
{
    return std::make_pair(m_histBins,m_histAxis);
}


// Enum-string converters
std::string enum2string(SegmentBB::eSegmentationMethod sm)
{
    std::map<SegmentBB::eSegmentationMethod, std::string> table = {
        {SegmentBB::method_manual,  "segment_manual"},
        {SegmentBB::method_otsu,    "segment_otsu"},
        {SegmentBB::method_rosin,   "segment_rosin"},
        {SegmentBB::method_usermask,"segment_usermask"}
    };
    
    return table.at(sm);
}

void string2enum(const std::string & str, SegmentBB::eSegmentationMethod &sm)
{
    std::map<std::string, SegmentBB::eSegmentationMethod> table = {
        {"segment_manual",   SegmentBB::method_manual},
        {"segment_otsu",     SegmentBB::method_otsu},
        {"segment_rosin",    SegmentBB::method_rosin},
        {"segment_usermask", SegmentBB::method_usermask}
    };

    sm = table.at(str);
}

const std::ostream & operator<<(std::ostream &s, SegmentBB::eSegmentationMethod sm)
{
    s<<enum2string(sm);

    return s;
}