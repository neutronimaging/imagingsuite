//<LICENSE>

#include <map>
#include <cmath>
#include <sstream>
#include <list>
#include <algorithm>
#include <numeric>

#include <armadillo>

#include <math/median.h>
#include <io/io_tiff.h>
#include <segmentation/thresholds.h>
#include <base/thistogram.h>
#include <base/timage.h>
#include <base/tprofile.h>
#include <base/tsubimage.h>
#include <strings/miscstring.h>
#include <morphology/label.h>
#include <morphology/repairhole.h>
#include <morphology/morphextrema.h>
#include <morphology/regionproperties.h>


#include "../include/ReferenceImageCorrection.h"
#include "../include/ImagingException.h"
#include "filters/medianfilter.h"

//using namespace TNT;

namespace ImagingAlgorithms {

ReferenceImageCorrection::ReferenceImageCorrection(kipl::interactors::InteractionBase *interactor) :
    logger("ReferenceImageCorrection"),
    m_bHaveOpenBeam(false),
    m_bHaveDarkCurrent(false),
    m_bHaveBlackBody(false),
    m_bHaveExternalBlackBody(false),
    m_bComputeLogarithm(true),
    bSaveBG(false),
    m_fOpenBeamDose(1.0f),
    m_bHaveDoseROI(false),
    m_bHaveBBDoseROI(false),
    m_bHaveBlackBodyROI(false),
    bExtSingleFile(true),
    fdose_ext_slice(0.0f),
    m_MaskMethod(ImagingAlgorithms::ReferenceImageCorrection::otsuMask),
    m_AverageMethod(ImagingAlgorithms::AverageImage::ImageWeightedAverage),
    m_IntMeth_x(SecondOrder_x),
    m_IntMeth_y(SecondOrder_y),
    m_nDoseROI(4,0UL),
    m_nROI(4,0UL),
    m_nBlackBodyROI(4,0UL),
    m_diffBBroi(4,0),
    m_nDoseBBRoi(4,0UL),
    angles(4,0UL),
    m_nBBimages(0),
    m_nProj(0),
    radius(0),
    tau(1.0f),
    min_area(0),
    thresh(0.0f),
    m_Interactor(interactor)
{
    a = b = c = d = e = f = 1;
}

ReferenceImageCorrection::~ReferenceImageCorrection()
{

}

void ReferenceImageCorrection::SaveBG(bool value, std::string path, std::string obname, std::string filemask)
{
    bSaveBG = value;
    pathBG = path;
    flatname_BG = obname;
    filemask_BG = filemask;
}

void ReferenceImageCorrection::SetInteractor(kipl::interactors::InteractionBase *interactor){
    if (interactor!=nullptr){
        m_Interactor=interactor;
    }
    else{
        m_Interactor=nullptr;
    }

}

void ReferenceImageCorrection::SetReferenceImages(kipl::base::TImage<float,2> *ob,
        kipl::base::TImage<float,2> *dc,
        bool useBB, bool useExtBB, bool useSingleExtBB,
        float dose_OB,
        float dose_DC,
        bool normBB,
        const std::vector<size_t> & roi,
        const std::vector<size_t> &doseroi)
{

    if (ob!=nullptr)
    {
		m_bHaveOpenBeam=true;
        m_OpenBeam=*ob;
	}

    if (dc!=nullptr)
    {
		m_bHaveDarkCurrent=true;
        m_DarkCurrent=*dc;
	}

    if (!doseroi.empty())
    {
        m_nDoseROI = doseroi;
    }

    if (!roi.empty())
    {
        m_nROI = roi;
    }
    else
    {
        m_nROI = {0,0,ob->Size(0),ob->Size(1)};
    }

    if (dose_OB!=0)
    {
        m_bHaveDoseROI=true;
        m_fOpenBeamDose = dose_OB;
    }

    if (dose_DC!=0){
       m_fDarkDose = dose_DC;
    }

    if (normBB) {
        m_bHaveBlackBodyROI = true;
        m_bHaveBBDoseROI = true;
    }


    if (useBB)
    {

		m_bHaveBlackBody=true;


        switch (m_InterpMethod)
        {
            case (Polynomial):
            {
                m_OB_BB_Interpolated = InterpolateBlackBodyImage(ob_bb_parameters, m_nROI); // now rois are in absolute coordinates , richtig?
                m_DoseBBflat_image   = InterpolateBlackBodyImage(ob_bb_parameters, m_nDoseROI);
                break;
            }
            case(ThinPlateSplines):
            {
                m_OB_BB_Interpolated = InterpolateBlackBodyImagewithSplines(ob_bb_parameters, spline_ob_values, m_nROI); // now rois are in absolute coordinates , richtig?
                m_DoseBBflat_image   = InterpolateBlackBodyImagewithSplines(ob_bb_parameters, spline_ob_values, m_nDoseROI);

                break;
            }
        }


        // HERE SAVE OB_BB INTERPOLATED

        if (bSaveBG)
        {
                std::string fname=pathBG+"/"+flatname_BG;
                kipl::strings::filenames::CheckPathSlashes(fname,false);
                kipl::io::WriteTIFF(m_OB_BB_Interpolated,fname,kipl::base::Float32); // seem correct
        }

        PrepareReferencesBB();
	}

    if (useExtBB)
    {
        m_bHaveExternalBlackBody = true;
        bExtSingleFile = useSingleExtBB;
        PrepareReferencesExtBB();
    }

    if(!useBB && !useExtBB)
    {
        PrepareReferences(); // original way
    }


}

void ReferenceImageCorrection::SetMaskCreationMethod(ReferenceImageCorrection::eMaskCreationMethod eMaskMethod, kipl::base::TImage<float, 2> &mask)
{
    m_MaskMethod = eMaskMethod;
    m_MaskImage  = mask;
}

void ReferenceImageCorrection::SetInterpolationOrderX(eInterpOrderX eim_x){
    m_IntMeth_x = eim_x;

    switch(eim_x){

    case(SecondOrder_x):
    {   b=1;
        d=1;
        e=1;
        break;
    }
    case(FirstOrder_x): {
        b=1;
        d=1;
        e=0;
        break;
    }
    case(ZeroOrder_x): {
        b=0;
        d=0;
        e=0;
        break;
    }

    }

}

void ReferenceImageCorrection::SetInterpolationOrderY(eInterpOrderY eim_y){
    m_IntMeth_y = eim_y;

    switch(eim_y){

    case(SecondOrder_y):{
        c=1;
        a=1;
        f=1;
        break;
    }
    case(FirstOrder_y):{
        c=1;
        a=1;
        f=0;
        break;
    }
    case(ZeroOrder_y):{
        c=0;
        a=0;
        f=0;
        break;
    }

    }


}

kipl::base::TImage<float,2> ReferenceImageCorrection::Process(kipl::base::TImage<float,2> &img, float dose)
{
    if (m_bComputeLogarithm)
    {
        ComputeLogNorm(img,dose);
    }
    else
    {
		ComputeNorm(img,dose);
    }

    return img;

}

void ReferenceImageCorrection::Process(kipl::base::TImage<float,3> &img, float *dose)
{
    std::vector<size_t> slice_dims = {img.Size(0), img.Size(1)};
    kipl::base::TImage<float, 2> slice(slice_dims);

    for (size_t i=0; (i<img.Size(2) && (updateStatus(float(i)/img.Size(2),"BBLogNorm: Referencing iteration")==false)); i++)
    {
        if (m_bHaveBlackBody)
        {
            switch (m_InterpMethod)
            {
            case Polynomial:
            {
                std::vector<float> current_param(6,0.0f);
                std::copy_n(sample_bb_interp_parameters.begin()+i*6,6,current_param.begin());
//                memcpy(current_param, sample_bb_interp_parameters+i*6, sizeof(float)*6);
                m_BB_sample_Interpolated = InterpolateBlackBodyImage(current_param ,m_nROI);
                m_DoseBBsample_image     = InterpolateBlackBodyImage(current_param, m_nDoseROI);
//                delete[] current_param;
                break;
            }
            case ThinPlateSplines:
            {
                std::vector<float> current_param(spline_sample_values.size()+3,0.0f);

                std::copy_n(sample_bb_interp_parameters.begin()+i*(spline_sample_values.size()+3),
                            spline_sample_values.size()+3,
                            current_param.begin());
//                memcpy(current_param, sample_bb_interp_parameters+i*(spline_sample_values.size()+3), sizeof(float)*(spline_sample_values.size()+3));

                m_BB_sample_Interpolated = InterpolateBlackBodyImagewithSplines(current_param, spline_sample_values, m_nROI);
                m_DoseBBsample_image     = InterpolateBlackBodyImagewithSplines(current_param, spline_sample_values, m_nDoseROI);
                break;
            }
            default: throw ImagingException("Unknown m_InterpMethod in ReferenceImageCorrection::SetReferenceImages", __FILE__, __LINE__);
            }

            // here to save the BG

            if (bSaveBG)
            {
                std::string filename, ext;
                kipl::strings::filenames::MakeFileName(filemask_BG,static_cast<int>(i),filename,ext,'#','0');
                std::string fname=pathBG+"/"+filename;
                kipl::strings::filenames::CheckPathSlashes(fname,false);
                kipl::io::WriteTIFF(m_BB_sample_Interpolated,fname,kipl::base::Float32); // seem correct

            }


        }

        if (m_bHaveExternalBlackBody){

            m_BB_slice_ext.resize(m_OB_BB_ext.dims());

            if (bExtSingleFile)
            {

//                memcpy(m_BB_slice_ext.GetDataPtr(),m_BB_sample_ext.GetDataPtr(), sizeof(float)*m_BB_sample_ext.Size(0)*m_BB_sample_ext.Size(1));
                fdose_ext_slice = fdoseS_ext;
            }
            else {

                if (m_BB_sample_ext.Size(2)!=img.Size(2)){
                    throw ImagingException ("Number of externally processed BB images are not the same as Projection data",__FILE__,__LINE__);
                }
                memcpy(m_BB_slice_ext.GetDataPtr(),m_BB_sample_ext.GetLinePtr(0,i), sizeof(float)*m_BB_sample_ext.Size(0)*m_BB_sample_ext.Size(1));
                fdose_ext_slice = fdose_ext_list[i];

            }

        }


        memcpy(slice.GetDataPtr(),img.GetLinePtr(0,i), sizeof(float)*slice.Size());
        Process(slice,dose[i]);
        memcpy(img.GetLinePtr(0,i), slice.GetDataPtr(), sizeof(float)*slice.Size()); // and copy the result back
	}


}

void ReferenceImageCorrection::SegmentBlackBody(kipl::base::TImage<float, 2> &img, kipl::base::TImage<float, 2> &mask)
{
    // 1. Enhance/flatten: subtract median filtered image from ImagingException
    // 2. Get histogram
    // 3. Find threshold  
    // 4. Apply threshold 
    // 5. Morph open and close 
    // 6. Label image 
    // 7. Get region properties
    // 8. Filter on size, position, and intensity 
    // 9. 

    // 2. otsu threshold
    // 2.a compute histogram

    if ((img.Size(0)!=mask.Size(0)) && (img.Size(1)!=mask.Size(1)))
        mask.resize(img.dims());

    kipl::filters::TMedianFilter<float,2> medh({71,1});
    kipl::filters::TMedianFilter<float,2> medv({1,71});

    auto fltv = medv(img, kipl::filters::FilterBase::EdgeZero);
    auto flat = medh(fltv,kipl::filters::FilterBase::EdgeZero);

    for (int i= 0; i< img.Size(); ++i)
    {
        flat[i]-=img[i];
        // if (flat[i]<0) flat[i]=0.0f;
    }

    std::vector<size_t> hist;
    std::vector<float>  axis;
    int ndims = 256;
    kipl::base::Histogram(flat.GetDataPtr(), flat.Size(), ndims, hist, axis, 0.0f,0.0f, true);

    // 3. Find threshold  
    // 4. Apply threshold 
    int idx = 0;
    switch (m_MaskMethod)
    {
        case ImagingAlgorithms::ReferenceImageCorrection::otsuMask :  
            idx = kipl::segmentation::Threshold_Otsu(hist);
            kipl::segmentation::Threshold( img.GetDataPtr(), 
                                mask.GetDataPtr(), 
                                img.Size(), 
                                axis[idx],
                                kipl::segmentation::cmp_less);
            break;
        case ImagingAlgorithms::ReferenceImageCorrection::rosinMask :
            idx = kipl::segmentation::Threshold_Rosin(hist);
            kipl::segmentation::Threshold( img.GetDataPtr(), 
                                           mask.GetDataPtr(), 
                                           img.Size(), 
                                           axis[idx],
                                           kipl::segmentation::cmp_less);

            break;
        case ImagingAlgorithms::ReferenceImageCorrection::manuallyThresholdedMask :
            kipl::segmentation::Threshold(img.GetDataPtr(), mask.GetDataPtr(), img.Size(), thresh);

            break;

        case ImagingAlgorithms::ReferenceImageCorrection::userDefinedMask :
        case ImagingAlgorithms::ReferenceImageCorrection::referenceFreeMask :
        default :
            throw ImagingException("Inappropriate mask choice in SegmentBlackBody, you shouldn't reach this point.",__FILE__,__LINE__);
    }

    // 5. Morph open and close 
    std::vector<float> kernel(25,1.0f);
    kipl::morphology::TDilate<float,2> D(kernel,{5,5});
    kipl::morphology::TErode<float,2> E(kernel,{5,5});

    mask = E(mask,kipl::filters::FilterBase::EdgeMirror);
    mask = D(mask,kipl::filters::FilterBase::EdgeMirror);

    mask = D(mask,kipl::filters::FilterBase::EdgeMirror);
    mask = E(mask,kipl::filters::FilterBase::EdgeMirror);

    // 6. Label image 
    kipl::base::TImage<int,2> lbl(mask.dims());
    
    size_t num_obj = 0UL;

    try {
         num_obj = kipl::morphology::LabelImage(mask,lbl);

         std::ostringstream msg;
         msg << "number of objects: " << num_obj;
         logger.debug(msg.str());
     }
     catch (ImagingException &e) {
         logger.error(e.what());
         std::cerr<<"Error in the SegmentBlackBody function\n";
         throw ImagingException("kipl::morphology::LabelImage failed", __FILE__, __LINE__);
     }
     catch(kipl::base::KiplException &e){
         logger.error(e.what());
         std::cerr<<"Error in the SegmentBlackBody function\n";
         throw kipl::base::KiplException("kipl::morphology::LabelImage failed", __FILE__, __LINE__);
     }
     catch (std::exception &e)
     {
         logger.error(e.what());
         throw ImagingException("kipl::morphology::LabelImage failed", __FILE__, __LINE__);
     }

    if (num_obj<=2)
         throw ImagingException("SegmentBlackBodyNorm failed \n Number of detected objects too little \n Please try to change the threshold or select a bigger ROI containing at least 2 BBs", __FILE__, __LINE__);
    // 7. Get region properties
    
    kipl::morphology::RegionProperties rp(lbl,img);

    auto area     = rp.area();
    auto spherity = rp.spherity();
 }

void ReferenceImageCorrection::SegmentBlackBody_old(kipl::base::TImage<float, 2> &img, kipl::base::TImage<float, 2> &mask)
{

    // 2. otsu threshold
    // 2.a compute histogram


    kipl::base::TImage<float,2> norm(img.dims());
//    memcpy(norm.GetDataPtr(), img.GetDataPtr(), sizeof(float)*img.Size()); // copy to norm.. evalute if necessary
    norm = img;

    std::vector<pair<float, size_t>> histo;
    float min = *std::min_element(norm.GetLinePtr(0), norm.GetLinePtr(0)+norm.Size());
    float max = *std::max_element(norm.GetLinePtr(0), norm.GetLinePtr(0)+norm.Size());
    kipl::base::THistogram<float> myhist(256, min, max);
    histo =  myhist(norm);


    //2.b compute otsu threshold

    float ot;
    if (m_MaskMethod == manuallyThresholdedMask)
    {
          ot = thresh;
    }
    else
    {
        size_t * vec_hist = new size_t[256];
        for(int i=0; i<256; i++)
        {
            vec_hist[i] = histo.at(i).second;
        }
        int value = kipl::segmentation::Threshold_Otsu(vec_hist, 256);
        ot = static_cast<float>(histo.at(value).first);
   }



    //2.c threshold image

    kipl::base::TImage<float,2> maskOtsu(norm.dims());
    kipl::base::TImage<int,2>   labelImage(norm.dims());

 // now it works:
//    kipl::segmentation::Threshold(norm.GetDataPtr(), maskOtsu.GetDataPtr(), norm.Size(), ot);

    float *pImg = norm.GetDataPtr();
    float *res = maskOtsu.GetDataPtr();
    const int N=static_cast<int>(norm.Size());
    for (int i=0; i<N; ++i){
        if (pImg[i]>=ot)
        {
            res[i] = 1.0f;
        }
        else
        {
            res[i] = 0.0f;
        }
    }


  // here: fillPeaks, as at this point the BBs are black in the binary mask

//    kipl::io::WriteTIFF(maskOtsu,"mask_Otsu.tif",kipl::base::Float32);
    kipl::base::TImage<float,2> maskOtsuFilled(mask.dims());

    try {
        maskOtsuFilled = kipl::morphology::FillPeaks(maskOtsu,kipl::base::conn4); // from morphextrema
    }
    catch (ImagingException &e)
    {
        logger.error(e.what());
        std::cerr<<"Error in the SegmentBlackBody function\n";
        throw ImagingException("kipl::morphology::FillPeaks failed", __FILE__, __LINE__);
    }

//    kipl::io::WriteTIFF(maskOtsuFilled, "maskOtsuFilled_before.tif",kipl::base::Float32);

     float bg = 1.0f;
     size_t num_obj;

     vector< pair< size_t, size_t > > area;
     try {
         num_obj = kipl::morphology::LabelImage(maskOtsuFilled,labelImage, kipl::base::conn4, bg);
//         kipl::io::WriteTIFF(labelImage, "labelImage.tif");
         std::ostringstream msg;
         msg << "number of objects: " << num_obj;
         logger.debug(msg.str());
     }
     catch (ImagingException &e) {
         logger.error(e.what());
         std::cerr<<"Error in the SegmentBlackBody function\n";
         throw ImagingException("kipl::morphology::LabelImage failed", __FILE__, __LINE__);
     }
     catch(kipl::base::KiplException &e){
         logger.error(e.what());
         std::cerr<<"Error in the SegmentBlackBody function\n";
         throw kipl::base::KiplException("kipl::morphology::LabelImage failed", __FILE__, __LINE__);
     }
     catch (std::exception &e)
     {
         logger.error(e.what());
         throw ImagingException("kipl::morphology::LabelImage failed", __FILE__, __LINE__);
     }



     kipl::morphology::LabelArea(labelImage, num_obj, area);


     if (num_obj<=2)
         throw ImagingException("SegmentBlackBodyNorm failed \n Number of detected objects too little \n Please try to change the threshold or select a bigger ROI containing at least 2 BBs", __FILE__, __LINE__);

//     kipl::io::WriteTIFF(labelImage,"labelImage.tif");



     // remove objetcs with size lower the minum size:
     for (size_t i=0; i<area.size(); ++i)
     {
         if (area.at(i).first<=min_area){
             int *pLbl=labelImage.GetDataPtr();
             float *pOtsu = maskOtsuFilled.GetDataPtr();
             for (size_t p=0; p<=maskOtsuFilled.Size(); ++p)
             {
                 if(pLbl[p]==i)
                     pOtsu[p]=1.0f;
             }

         }
     }

//          kipl::io::WriteTIFF(maskOtsuFilled, "maskOtsuFilled.tif",kipl::base::Float32);


     if (num_obj==2 || area.at(1).first>=3*area.at(2).first) // this in principle assumes that there can not be only one BB
//     if (area.at(1).first>=3*area.at(2).first)
     {
         throw ImagingException("SegmentBlackBodyNorm failed \n Please try to change the threshold ", __FILE__, __LINE__);
     }
     else
         {


        // 3. Compute mask within Otsu
        // 3.a sum of rows and columns and location of rois



        float *vert_profile = new float[maskOtsuFilled.Size(1)];
        float *hor_profile = new float[maskOtsuFilled.Size(0)];


        kipl::base::VerticalProjection2D(maskOtsuFilled.GetDataPtr(), maskOtsuFilled.dims(), vert_profile, true); // sum of rows
        kipl::base::HorizontalProjection2D(maskOtsuFilled.GetDataPtr(), maskOtsuFilled.dims(), hor_profile, true); // sum of columns


        //3.b create binary Signal
        float *bin_VP = new float[maskOtsuFilled.Size(1)];
        float *bin_HP = new float[maskOtsuFilled.Size(0)];

        for (size_t i=0; i<maskOtsuFilled.Size(1); i++) {
            float max = *std::max_element(vert_profile, vert_profile+maskOtsuFilled.Size(1));
            if(vert_profile[i]<max) {
                bin_VP[i] = 1;
            }
            else {
                bin_VP[i] = 0;
            }

        }


        for (size_t i=0; i<maskOtsuFilled.Size(0); i++) {
            float max = *std::max_element(hor_profile, hor_profile+maskOtsuFilled.Size(0));
            if(hor_profile[i]<max) {
                bin_HP[i] = 1;
            }
            else {
                bin_HP[i] = 0;
            }
        }


        // 3.c compute edges - diff signal

        float *pos_left = new float[100];
        float *pos_right = new float[100];
        int index_left = 0;
        int index_right = 0;


         for (int i=0; i<maskOtsuFilled.Size(1)-1; i++) {
             float diff = bin_VP[i+1]-bin_VP[i];
             if (diff>=1) {
                 pos_left[index_left] = i;
                 index_left++;
             }
             if (diff<=-1) {
                 pos_right[index_right]=i+1; // to have all BB within the rois
                 index_right++;
             }
         }



         // the same on the horizontal profile:
         float *pos_left_2 = new float[100];
         float *pos_right_2 = new float[100];
         int index_left_2 = 0;
         int index_right_2 = 0;


          for (int i=0; i<maskOtsuFilled.Size(0)-1; i++) {
              float diff = bin_HP[i+1]-bin_HP[i];
              if (diff>=1) {
                  pos_left_2[index_left_2] = i;
                  index_left_2++;
              }
              if (diff<=-1) {
                  pos_right_2[index_right_2]=i+1;
                  index_right_2++;
              }
          }



          // from these define ROIs containing BBs --> i can probably delete them later on
          std::vector<pair <int,int>> right_edges((index_left)*(index_left_2));
          std::vector<pair <int,int>> left_edges((index_left)*(index_left_2));

          int pos = 0;
          for (int i=0; i<(index_left); i++) {

              for (int j=0; j<(index_left_2); j++) {

                    right_edges.at(pos).first = pos_right[i]; // right position on vertical profiles = sum of rows = y axis = y1
                    right_edges.at(pos).second = pos_right_2[j]; // right position on horizontal profiles = sum of columns = x axis = x1
                    left_edges.at(pos).first = pos_left[i]; // left position on vertical profiles = y0
                    left_edges.at(pos).second = pos_left_2[j]; // left position on horizontal profiles = x0

                    pos++;
              }

          }



          for (size_t bb_index=0; bb_index<pos; bb_index++){

              std::vector<size_t> roi_dim = { size_t(right_edges.at(bb_index).second-left_edges.at(bb_index).second),
                                              size_t(right_edges.at(bb_index).first-left_edges.at(bb_index).first)}; // Dx and Dy

              kipl::base::TImage<float,2> roi(roi_dim);
              kipl::base::TImage<float,2> roi_im(roi_dim);


              for (int i=0; i<roi_dim[1]; i++) {
                    memcpy(roi.GetLinePtr(i),maskOtsuFilled.GetLinePtr(left_edges.at(bb_index).first+i)+left_edges.at(bb_index).second, sizeof(float)*roi_dim[0]); // one could use tsubimage
                    memcpy(roi_im.GetLinePtr(i),norm.GetLinePtr(left_edges.at(bb_index).first+i)+left_edges.at(bb_index).second, sizeof(float)*roi_dim[0]);
              }

              float x_com= 0.0f;
              float y_com= 0.0f;
              int size = 0;

//              // old implementation with geometrical mean:
//              for (size_t x=0; x<roi.Size(0); x++) {
//                  for (size_t y=0; y<roi.Size(1); y++) {
//                      if(roi(x,y)==0) {
//                          x_com +=x;
//                          y_com +=y;
//                          size++;
//                      }

//                  }
//              }
//              x_com /=size;
//              y_com /=size;

              float sum_roi = 0.0f;

              // weighted center of mass:
              for (size_t y=0; y<roi.Size(1); y++) {
                for (size_t x=0; x<roi.Size(0); x++) {

                      if(roi(x,y)==0) {
                          x_com += (roi_im(x,y)*float(x));
                          y_com += (roi_im(x,y)*float(y));
                          sum_roi += roi_im(x,y);
                          size++;
                      }

                  }
              }


              // draw the circle with user-defined radius

              // check on BB dimensions: this is now done at the beginning to avoid crash
              if (size>=min_area) {


                  x_com /=sum_roi;
                  y_com /=sum_roi;

                      for (size_t y=0; y<roi.Size(1); y++) {
                            for (size_t x=0; x<roi.Size(0); x++) {

                              if ((sqrt(int(x-x_com+0.5)*int(x-x_com+0.5)+int(y-y_com+0.5)*int(y-y_com+0.5)))<=radius && roi(x,y)==0) {
    //                              roi(x,y)=1; // this one actually I don't need
                                  mask(x+left_edges.at(bb_index).second, y+left_edges.at(bb_index).first) = 1;
                              }
                          }
                      }

                      roi(int(x_com+0.5), int(y_com+0.5))=2;
              }

          }



//          kipl::io::WriteTIFF(mask,"mask.tif",kipl::base::Float32);
    }

}

void ReferenceImageCorrection::ComputeBlackBodyCentroids(kipl::base::TImage<float, 2> &img, kipl::base::TImage<float, 2> &mask, std::map<std::pair<int, int>, float> &values)
{
    // 3. Compute mask within Otsu
    // 3.a sum of rows and columns and location of rois
    kipl::base::TImage<float,2> maskOtsu(img.dims());
    maskOtsu= 0.0f;

    std::map<std::pair<int, int>, float> new_values;

    float *p_mask = mask.GetDataPtr();
    float *p_otsu = maskOtsu.GetDataPtr();
    for (int i=0; i<mask.Size(); i++)
    {
        if (p_mask[i]==0.0f) p_otsu[i]=1.0f;

    }

    float *vert_profile = new float[maskOtsu.Size(1)];
    float *hor_profile = new float[maskOtsu.Size(0)];

    kipl::base::VerticalProjection2D(maskOtsu.GetDataPtr(), maskOtsu.dims(), vert_profile, true); // sum of rows
    kipl::base::HorizontalProjection2D(maskOtsu.GetDataPtr(), maskOtsu.dims(), hor_profile, true); // sum of columns


    //3.b create binary Signal
    float *bin_VP = new float[maskOtsu.Size(1)];
    float *bin_HP = new float[maskOtsu.Size(0)];

    for (size_t i=0; i<mask.Size(1); i++) {
        float max = *std::max_element(vert_profile, vert_profile+maskOtsu.Size(1));
        if(vert_profile[i]<max) {
            bin_VP[i] = 1;
        }
        else {
            bin_VP[i] = 0;
        }

    }


    for (size_t i=0; i<mask.Size(0); i++) {
        float max = *std::max_element(hor_profile, hor_profile+maskOtsu.Size(0));
        if(hor_profile[i]<max) {
            bin_HP[i] = 1;
        }
        else {
            bin_HP[i] = 0;
        }
    }

    // 3.c compute edges - diff signal

    float *pos_left = new float[100];
    float *pos_right = new float[100];
    int index_left = 0;
    int index_right = 0;


     for (int i=0; i<mask.Size(1)-1; i++) {
         float diff = bin_VP[i+1]-bin_VP[i];
         if (diff>=1) {
             pos_left[index_left] = i;
             index_left++;
         }
         if (diff<=-1) {
             pos_right[index_right]=i+1; // to have all BB within the rois
             index_right++;
         }
     }

     // the same on the horizontal profile:
     float *pos_left_2 = new float[100];
     float *pos_right_2 = new float[100];
     int index_left_2 = 0;
     int index_right_2 = 0;


      for (int i=0; i<mask.Size(0)-1; i++) {
          float diff = bin_HP[i+1]-bin_HP[i];
          if (diff>=1) {
              pos_left_2[index_left_2] = i;
              index_left_2++;
          }
          if (diff<=-1) {
              pos_right_2[index_right_2]=i+1;
              index_right_2++;
          }
      }


      // from these define ROIs containing BBs --> i can probably delete them later on
      std::vector<pair <int,int>> right_edges((index_left)*(index_left_2));
      std::vector<pair <int,int>> left_edges((index_left)*(index_left_2));

      int pos = 0;
      for (int i=0; i<(index_left); i++) {

          for (int j=0; j<(index_left_2); j++) {

                right_edges.at(pos).first = pos_right[i]; // right position on vertical profiles = sum of rows = y axis = y1
                right_edges.at(pos).second = pos_right_2[j]; // right position on horizontal profiles = sum of columns = x axis = x1
                left_edges.at(pos).first = pos_left[i]; // left position on vertical profiles = y0
                left_edges.at(pos).second = pos_left_2[j]; // left position on horizontal profiles = x0
                pos++;
          }

      }


//      ostringstream msg;
//      msg.str(""); msg<<"pos= "<<pos;
//      logger(kipl::logging::Logger::LogMessage,msg.str());

//      std::map<std::pair<int, int>, float>::const_iterator it_values = values.begin();

      for (size_t bb_index=0; bb_index<pos; bb_index++){

//          msg.str(""); msg<<"bb_index= "<<bb_index;
//          logger(kipl::logging::Logger::LogMessage,msg.str());

          std::vector<size_t> roi_dim = { size_t(right_edges.at(bb_index).second-left_edges.at(bb_index).second),
                                          size_t(right_edges.at(bb_index).first-left_edges.at(bb_index).first)}; // Dx and Dy
          kipl::base::TImage<float,2> roi(roi_dim);
          kipl::base::TImage<float,2> roi_im(roi_dim);


          for (int i=0; i<roi_dim[1]; i++) {
                memcpy(roi.GetLinePtr(i),maskOtsu.GetLinePtr(left_edges.at(bb_index).first+i)+left_edges.at(bb_index).second, sizeof(float)*roi_dim[0]); // one could use tsubimage
                memcpy(roi_im.GetLinePtr(i),img.GetLinePtr(left_edges.at(bb_index).first+i)+left_edges.at(bb_index).second, sizeof(float)*roi_dim[0]);
          }

          float x_com = 0.0f;
          float y_com = 0.0f;
          int size = 0;

//          // old implementation with geometrical mean:
//          for (size_t y=0; y<roi.Size(1); y++) {
//            for (size_t x=0; x<roi.Size(0); x++) {
//                  if(roi(x,y)==0) {
//                      x_com +=x;
//                      y_com +=y;
//                      size++;
//                  }

//              }
//          }
//          x_com /=static_cast<float>(size);
//          y_com /=static_cast<float>(size);

          float sum_roi = 0.0f;

          // weighted center of mass:
          for (size_t x=0; x<roi.Size(0); x++) {
              for (size_t y=0; y<roi.Size(1); y++) {
                  if(roi(x,y)==0) {
                      x_com += (roi_im(x,y)*float(x));
                      y_com += (roi_im(x,y)*float(y));
                      sum_roi += roi_im(x,y);
                      size++;
                  }

              }
          }

          // draw the circle with user-defined radius

          // check on BB dimensions:
//          if (size>=min_area) { //this is now wrong

          if (size!=0) { // check on BB existance


              x_com /=sum_roi;
              y_com /=sum_roi;

              std::pair<int,int> temp;
              temp = std::make_pair(floor(x_com+0.5)+left_edges.at(bb_index).second+m_diffBBroi[0], floor(y_com+0.5)+left_edges.at(bb_index).first+m_diffBBroi[1]);

              float value = 0.0f;
              std::vector<float> grayvalues;
              float mean_value =0.0f;

                  for (size_t y=0; y<roi.Size(1); y++) {
                        for (size_t x=0; x<roi.Size(0); x++) {



                          if ((sqrt(int(x-x_com+0.5)*int(x-x_com+0.5)+int(y-y_com+0.5)*int(y-y_com+0.5)))<=radius && roi(x,y)==0) { // and also this one is useless
//                              roi(x,y)=1; // this one actually I don't need
//                              mask(x+left_edges.at(bb_index).second, y+left_edges.at(bb_index).first) = 1; // this is now useless
                              value = img(x+left_edges.at(bb_index).second, y+left_edges.at(bb_index).first); // have to compute the median value from it
//                              if (value!=0)
//                              {
                                grayvalues.push_back(value);
                                mean_value += value;
//                              }
//
                          }


                      }
                  }

                  mean_value /= static_cast<float>(grayvalues.size());


                      roi(int(x_com+0.5), int(y_com+0.5))=2;
                      const auto median_it1 = grayvalues.begin() + grayvalues.size() / 2 - 1;
                      const auto median_it2 = grayvalues.begin() + grayvalues.size() / 2;
                      std::nth_element(grayvalues.begin(), median_it1 , grayvalues.end()); // e1
                      std::nth_element(grayvalues.begin(), median_it2 , grayvalues.end()); // e2
                      float median = (grayvalues.size() % 2 == 0) ? (*median_it1 + *median_it2) / 2 : *median_it2;
                       float average = std::accumulate(grayvalues.begin(), grayvalues.end(), 0.0)/ grayvalues.size();

//                      new_values.insert(std::make_pair(temp,median));
                      std::pair<int,int> mypair;

                      for (std::map<std::pair<int, int>, float>::const_iterator it_values = values.begin(); it_values != values.end(); ++it_values)
                      {
                          float dist = (it_values->first.first-temp.first)*(it_values->first.first-temp.first)+(it_values->first.second-temp.second)*(it_values->first.second-temp.second);
                          if(dist<=100.0f)
                          {
                                mypair = std::make_pair(it_values->first.first, it_values->first.second);
                                mypair.first = it_values->first.first;
                                mypair.second = it_values->first.second;
                                values.at(mypair) = median;
                                break;
                          }
                      }

         }

      }


}

void ReferenceImageCorrection::SegmentBlackBody(kipl::base::TImage<float,2> &normimg, kipl::base::TImage<float,2> &img, kipl::base::TImage<float,2> &mask, std::map<std::pair<int, int>, float> &values)
{
    // 2. otsu threshold
    // 2.a compute histogram
    kipl::base::TImage<float,2> norm;
    norm.Clone(normimg);

    std::vector<pair<float, size_t>> histo;
    float min = *std::min_element(norm.GetLinePtr(0), norm.GetLinePtr(0)+norm.Size());
    float max = *std::max_element(norm.GetLinePtr(0), norm.GetLinePtr(0)+norm.Size());
    kipl::base::THistogram<float> myhist(256, min, max);
    histo =  myhist(norm);

    size_t * vec_hist = new size_t[256];
    for(int i=0; i<256; i++)
    {
        vec_hist[i] = histo.at(i).second;
    }


    //2.b compute otsu threshold
    float ot;

    if (m_MaskMethod == manuallyThresholdedMask)
        {
              ot = thresh;
        }
    else
        {
            int value = kipl::segmentation::Threshold_Otsu(vec_hist, 256);
            ot = static_cast<float>(histo.at(value).first);
            }

    //2.c threshold image

    kipl::base::TImage<float,2> maskOtsu(mask.dims());
    kipl::base::TImage<int,2> labelImage(mask.dims());

 // now it works:
//    kipl::segmentation::Threshold(norm.GetDataPtr(), maskOtsu.GetDataPtr(), norm.Size(), ot);

    float *pImg = norm.GetDataPtr();
    float *res  = maskOtsu.GetDataPtr();
    const int N = static_cast<int>(norm.Size());
    for (size_t i=0; i<norm.Size(); ++i)
    {
        if (pImg[i]>=ot)
        {
            res[i] = 1.0f;
        }
        else
        {
            res[i] = 0.0f;
        }
    }

    kipl::base::TImage<float,2> maskOtsuFilled(mask.dims());
    maskOtsuFilled = kipl::morphology::FillPeaks(maskOtsu,kipl::base::conn4); // from morphextrema

    float bg = 1.0f;
    int num_obj = kipl::morphology::LabelImage(maskOtsu,labelImage, kipl::base::conn4, bg);

    vector< pair< size_t, size_t > > area;
    kipl::morphology::LabelArea(labelImage, num_obj, area);

    // remove objetcs with size lower the minum size:
    for (size_t i=0; i<area.size(); ++i)
    {
        if (area.at(i).first<=min_area){
            int *pLbl=labelImage.GetDataPtr();
            float *pOtsu = maskOtsuFilled.GetDataPtr();
            for (size_t p=0; p<=maskOtsuFilled.Size(); ++p)
            {
                if(pLbl[p]==i)
                    pOtsu[p]=1.0f;
            }

        }
    }


         if (num_obj==2 || area.at(1).first>=3*area.at(2).first) // this in principle assumes that there can not be only one BB
//    if (area.at(1).first>=3*area.at(2).first)
    {
        throw ImagingException("SegmentBlackBodyNorm failed \n Please try to change the threshold ", __FILE__, __LINE__);
    }
    else
        {


            // 3. Compute mask within Otsu
            // 3.a sum of rows and columns and location of rois

            float *vert_profile = new float[maskOtsuFilled.Size(1)];
            float *hor_profile = new float[maskOtsuFilled.Size(0)];


            kipl::base::VerticalProjection2D(maskOtsuFilled.GetDataPtr(), maskOtsuFilled.dims(), vert_profile, true); // sum of rows
            kipl::base::HorizontalProjection2D(maskOtsuFilled.GetDataPtr(), maskOtsuFilled.dims(), hor_profile, true); // sum of columns


            //3.b create binary Signal
            float *bin_VP = new float[maskOtsuFilled.Size(1)];
            float *bin_HP = new float[maskOtsuFilled.Size(0)];

            for (size_t i=0; i<maskOtsuFilled.Size(1); i++) {
                float max = *std::max_element(vert_profile, vert_profile+maskOtsuFilled.Size(1));
                if(vert_profile[i]<max) {
                    bin_VP[i] = 1;
                }
                else {
                    bin_VP[i] = 0;
                }

            }


            for (size_t i=0; i<maskOtsuFilled.Size(0); i++) {
                float max = *std::max_element(hor_profile, hor_profile+maskOtsuFilled.Size(0));
                if(hor_profile[i]<max) {
                    bin_HP[i] = 1;
                }
                else {
                    bin_HP[i] = 0;
                }
            }


            // 3.c compute edges - diff signal

            float *pos_left = new float[100];
            float *pos_right = new float[100];
            int index_left = 0;
            int index_right = 0;


             for (int i=0; i<maskOtsuFilled.Size(1)-1; i++) {
                 float diff = bin_VP[i+1]-bin_VP[i];
                 if (diff>=1) {
                     pos_left[index_left] = i;
                     index_left++;
                 }
                 if (diff<=-1) {
                     pos_right[index_right]=i+1; // to have all BB within the rois
                     index_right++;
                 }
             }


             // the same on the horizontal profile:
             float *pos_left_2 = new float[100];
             float *pos_right_2 = new float[100];
             int index_left_2 = 0;
             int index_right_2 = 0;


              for (int i=0; i<maskOtsuFilled.Size(0)-1; i++) {
                  float diff = bin_HP[i+1]-bin_HP[i];
                  if (diff>=1) {
                      pos_left_2[index_left_2] = i;
                      index_left_2++;
                  }
                  if (diff<=-1) {
                      pos_right_2[index_right_2]=i+1;
                      index_right_2++;
                  }
              }


              // from these define ROIs containing BBs --> i can probably delete them later on
              std::vector<pair <int,int>> right_edges((index_left)*(index_left_2));
              std::vector<pair <int,int>> left_edges((index_left)*(index_left_2));

              int pos = 0;
              for (int i=0; i<(index_left); i++) {

                  for (int j=0; j<(index_left_2); j++) {

                        right_edges.at(pos).first = pos_right[i]; // right position on vertical profiles = sum of rows = y axis = y1
                        right_edges.at(pos).second = pos_right_2[j]; // right position on horizontal profiles = sum of columns = x axis = x1
                        left_edges.at(pos).first = pos_left[i]; // left position on vertical profiles = y0
                        left_edges.at(pos).second = pos_left_2[j]; // left position on horizontal profiles = x0
                        pos++;
                  }

              }



              for (size_t bb_index=0; bb_index<pos; bb_index++){

                  std::vector<size_t> roi_dim = { size_t(right_edges.at(bb_index).second-left_edges.at(bb_index).second),
                                                  size_t(right_edges.at(bb_index).first-left_edges.at(bb_index).first)}; // Dx and Dy
                  kipl::base::TImage<float,2> roi(roi_dim);
                  kipl::base::TImage<float,2> roi_im(roi_dim);


                  for (int i=0; i<roi_dim[1]; i++) {
                        memcpy(roi.GetLinePtr(i),maskOtsu.GetLinePtr(left_edges.at(bb_index).first+i)+left_edges.at(bb_index).second, sizeof(float)*roi_dim[0]); // one could use tsubimage
                        memcpy(roi_im.GetLinePtr(i),norm.GetLinePtr(left_edges.at(bb_index).first+i)+left_edges.at(bb_index).second, sizeof(float)*roi_dim[0]);// I am not sure this one is correct.
                  }

                  float x_com= 0.0f;
                  float y_com= 0.0f;
                  int size = 0;

//                  // old implementation with geometrical mean:
//                  for (size_t y=0; y<roi.Size(1); y++) {
//                    for (size_t x=0; x<roi.Size(0); x++) {
//                          if(roi(x,y)==0) {
//                              x_com +=x;
//                              y_com +=y;
//                              size++;
//                          }

//                      }
//                  }
//                  x_com /= static_cast<float>(size);
//                  y_com /= static_cast<float>(size);

                  float sum_roi = 0.0f;

                  // weighted center of mass:
                  for (size_t x=0; x<roi.Size(0); x++) {
                      for (size_t y=0; y<roi.Size(1); y++) {
                          if(roi(x,y)==0) {
                              x_com += (roi_im(x,y)*float(x));
                              y_com += (roi_im(x,y)*float(y));
                              sum_roi += roi_im(x,y);
                              size++;
                          }

                      }
                  }



                  // draw the circle with user-defined radius

                  // check on BB dimensions:
                  if (size>=min_area) {


                      x_com /=sum_roi;
                      y_com /=sum_roi;

                      std::pair<int,int> temp;
                      temp = std::make_pair(floor(x_com+0.5)+left_edges.at(bb_index).second+m_diffBBroi[0], floor(y_com+0.5)+left_edges.at(bb_index).first+m_diffBBroi[1]);

                      float value = 0.0f;
                      std::vector<float> grayvalues;
                      float mean_value =0.0f;

                         for (size_t y=0; y<roi.Size(1); y++) {
                              for (size_t x=0; x<roi.Size(0); x++) {

                                   if ((sqrt(int(x-x_com+0.5)*int(x-x_com+0.5)+int(y-y_com+0.5)*int(y-y_com+0.5)))<=radius && roi(x,y)==0) {
            //                              roi(x,y)=1; // this one actually I don't need
                                          mask(x+left_edges.at(bb_index).second, y+left_edges.at(bb_index).first) = 1;
                                          value = img(x+left_edges.at(bb_index).second, y+left_edges.at(bb_index).first); // have to compute the median value from it
//                                          if (value!=0)
//                                          {
                                            grayvalues.push_back(value);
                                            mean_value =+value;
//                                           }


                                  }
                              }
                          }

                          roi(int(x_com+0.5), int(y_com+0.5))=2;

                          mean_value /= static_cast<float>(grayvalues.size());


                          const auto median_it1 = grayvalues.begin() + grayvalues.size() / 2 - 1;
                          const auto median_it2 = grayvalues.begin() + grayvalues.size() / 2;
                          std::nth_element(grayvalues.begin(), median_it1 , grayvalues.end()); // e1
                          std::nth_element(grayvalues.begin(), median_it2 , grayvalues.end()); // e2
                          float median = (grayvalues.size() % 2 == 0) ? (*median_it1 + *median_it2) / 2 : *median_it2; // here I compute the median
                          float average = std::accumulate( grayvalues.begin(), grayvalues.end(), 0.0)/ grayvalues.size();
                          values.insert(std::make_pair(temp,median));
//                          values.insert(std::make_pair(temp,average));
                 }

              }

      }
}

void ReferenceImageCorrection::SegmentBlackBody_old(kipl::base::TImage<float,2> &normimg, kipl::base::TImage<float,2> &img, kipl::base::TImage<float,2> &mask, std::map<std::pair<int, int>, float> &values)
{
    // 2. otsu threshold
    // 2.a compute histogram
    kipl::base::TImage<float,2> norm;
    norm.Clone(normimg);

    std::vector<pair<float, size_t>> histo;
    float min = *std::min_element(norm.GetLinePtr(0), norm.GetLinePtr(0)+norm.Size());
    float max = *std::max_element(norm.GetLinePtr(0), norm.GetLinePtr(0)+norm.Size());
    kipl::base::THistogram<float> myhist(256, min, max);
    histo =  myhist(norm);

    size_t * vec_hist = new size_t[256];
    for(int i=0; i<256; i++)
    {
        vec_hist[i] = histo.at(i).second;
    }


    //2.b compute otsu threshold
    float ot;

    if (m_MaskMethod == manuallyThresholdedMask)
        {
              ot = thresh;
        }
    else
        {
            int value = kipl::segmentation::Threshold_Otsu(vec_hist, 256);
            ot = static_cast<float>(histo.at(value).first);
            }

    //2.c threshold image

    kipl::base::TImage<float,2> maskOtsu(mask.dims());
    kipl::base::TImage<int,2> labelImage(mask.dims());

 // now it works:
//    kipl::segmentation::Threshold(norm.GetDataPtr(), maskOtsu.GetDataPtr(), norm.Size(), ot);

    float *pImg = norm.GetDataPtr();
    float *res  = maskOtsu.GetDataPtr();
    const int N = static_cast<int>(norm.Size());
    for (size_t i=0; i<norm.Size(); ++i)
    {
        if (pImg[i]>=ot)
        {
            res[i] = 1.0f;
        }
        else
        {
            res[i] = 0.0f;
        }
    }

    kipl::base::TImage<float,2> maskOtsuFilled(mask.dims());
    maskOtsuFilled = kipl::morphology::FillPeaks(maskOtsu,kipl::base::conn4); // from morphextrema

    float bg = 1.0f;
    int num_obj = kipl::morphology::LabelImage(maskOtsu,labelImage, kipl::base::conn4, bg);

    vector< pair< size_t, size_t > > area;
    kipl::morphology::LabelArea(labelImage, num_obj, area);

    // remove objetcs with size lower the minum size:
    for (size_t i=0; i<area.size(); ++i)
    {
        if (area.at(i).first<=min_area){
            int *pLbl=labelImage.GetDataPtr();
            float *pOtsu = maskOtsuFilled.GetDataPtr();
            for (size_t p=0; p<=maskOtsuFilled.Size(); ++p)
            {
                if(pLbl[p]==i)
                    pOtsu[p]=1.0f;
            }

        }
    }


         if (num_obj==2 || area.at(1).first>=3*area.at(2).first) // this in principle assumes that there can not be only one BB
//    if (area.at(1).first>=3*area.at(2).first)
    {
        throw ImagingException("SegmentBlackBodyNorm failed \n Please try to change the threshold ", __FILE__, __LINE__);
    }
    else
        {


            // 3. Compute mask within Otsu
            // 3.a sum of rows and columns and location of rois

            float *vert_profile = new float[maskOtsuFilled.Size(1)];
            float *hor_profile = new float[maskOtsuFilled.Size(0)];


            kipl::base::VerticalProjection2D(maskOtsuFilled.GetDataPtr(), maskOtsuFilled.dims(), vert_profile, true); // sum of rows
            kipl::base::HorizontalProjection2D(maskOtsuFilled.GetDataPtr(), maskOtsuFilled.dims(), hor_profile, true); // sum of columns


            //3.b create binary Signal
            float *bin_VP = new float[maskOtsuFilled.Size(1)];
            float *bin_HP = new float[maskOtsuFilled.Size(0)];

            for (size_t i=0; i<maskOtsuFilled.Size(1); i++) {
                float max = *std::max_element(vert_profile, vert_profile+maskOtsuFilled.Size(1));
                if(vert_profile[i]<max) {
                    bin_VP[i] = 1;
                }
                else {
                    bin_VP[i] = 0;
                }

            }


            for (size_t i=0; i<maskOtsuFilled.Size(0); i++) {
                float max = *std::max_element(hor_profile, hor_profile+maskOtsuFilled.Size(0));
                if(hor_profile[i]<max) {
                    bin_HP[i] = 1;
                }
                else {
                    bin_HP[i] = 0;
                }
            }


            // 3.c compute edges - diff signal

            float *pos_left = new float[100];
            float *pos_right = new float[100];
            int index_left = 0;
            int index_right = 0;


             for (int i=0; i<maskOtsuFilled.Size(1)-1; i++) {
                 float diff = bin_VP[i+1]-bin_VP[i];
                 if (diff>=1) {
                     pos_left[index_left] = i;
                     index_left++;
                 }
                 if (diff<=-1) {
                     pos_right[index_right]=i+1; // to have all BB within the rois
                     index_right++;
                 }
             }


             // the same on the horizontal profile:
             float *pos_left_2 = new float[100];
             float *pos_right_2 = new float[100];
             int index_left_2 = 0;
             int index_right_2 = 0;


              for (int i=0; i<maskOtsuFilled.Size(0)-1; i++) {
                  float diff = bin_HP[i+1]-bin_HP[i];
                  if (diff>=1) {
                      pos_left_2[index_left_2] = i;
                      index_left_2++;
                  }
                  if (diff<=-1) {
                      pos_right_2[index_right_2]=i+1;
                      index_right_2++;
                  }
              }


              // from these define ROIs containing BBs --> i can probably delete them later on
              std::vector<pair <int,int>> right_edges((index_left)*(index_left_2));
              std::vector<pair <int,int>> left_edges((index_left)*(index_left_2));

              int pos = 0;
              for (int i=0; i<(index_left); i++) {

                  for (int j=0; j<(index_left_2); j++) {

                        right_edges.at(pos).first = pos_right[i]; // right position on vertical profiles = sum of rows = y axis = y1
                        right_edges.at(pos).second = pos_right_2[j]; // right position on horizontal profiles = sum of columns = x axis = x1
                        left_edges.at(pos).first = pos_left[i]; // left position on vertical profiles = y0
                        left_edges.at(pos).second = pos_left_2[j]; // left position on horizontal profiles = x0
                        pos++;
                  }

              }



              for (size_t bb_index=0; bb_index<pos; bb_index++){

                  std::vector<size_t> roi_dim = { size_t(right_edges.at(bb_index).second-left_edges.at(bb_index).second),
                                                  size_t(right_edges.at(bb_index).first-left_edges.at(bb_index).first)}; // Dx and Dy
                  kipl::base::TImage<float,2> roi(roi_dim);
                  kipl::base::TImage<float,2> roi_im(roi_dim);


                  for (int i=0; i<roi_dim[1]; i++) {
                        memcpy(roi.GetLinePtr(i),maskOtsu.GetLinePtr(left_edges.at(bb_index).first+i)+left_edges.at(bb_index).second, sizeof(float)*roi_dim[0]); // one could use tsubimage
                        memcpy(roi_im.GetLinePtr(i),norm.GetLinePtr(left_edges.at(bb_index).first+i)+left_edges.at(bb_index).second, sizeof(float)*roi_dim[0]);// I am not sure this one is correct.
                  }

                  float x_com= 0.0f;
                  float y_com= 0.0f;
                  int size = 0;

//                  // old implementation with geometrical mean:
//                  for (size_t y=0; y<roi.Size(1); y++) {
//                    for (size_t x=0; x<roi.Size(0); x++) {
//                          if(roi(x,y)==0) {
//                              x_com +=x;
//                              y_com +=y;
//                              size++;
//                          }

//                      }
//                  }
//                  x_com /= static_cast<float>(size);
//                  y_com /= static_cast<float>(size);

                  float sum_roi = 0.0f;

                  // weighted center of mass:
                  for (size_t x=0; x<roi.Size(0); x++) {
                      for (size_t y=0; y<roi.Size(1); y++) {
                          if(roi(x,y)==0) {
                              x_com += (roi_im(x,y)*float(x));
                              y_com += (roi_im(x,y)*float(y));
                              sum_roi += roi_im(x,y);
                              size++;
                          }

                      }
                  }



                  // draw the circle with user-defined radius

                  // check on BB dimensions:
                  if (size>=min_area) {


                      x_com /=sum_roi;
                      y_com /=sum_roi;

                      std::pair<int,int> temp;
                      temp = std::make_pair(floor(x_com+0.5)+left_edges.at(bb_index).second+m_diffBBroi[0], floor(y_com+0.5)+left_edges.at(bb_index).first+m_diffBBroi[1]);

                      float value = 0.0f;
                      std::vector<float> grayvalues;
                      float mean_value =0.0f;

                         for (size_t y=0; y<roi.Size(1); y++) {
                              for (size_t x=0; x<roi.Size(0); x++) {

                                   if ((sqrt(int(x-x_com+0.5)*int(x-x_com+0.5)+int(y-y_com+0.5)*int(y-y_com+0.5)))<=radius && roi(x,y)==0) {
            //                              roi(x,y)=1; // this one actually I don't need
                                          mask(x+left_edges.at(bb_index).second, y+left_edges.at(bb_index).first) = 1;
                                          value = img(x+left_edges.at(bb_index).second, y+left_edges.at(bb_index).first); // have to compute the median value from it
//                                          if (value!=0)
//                                          {
                                            grayvalues.push_back(value);
                                            mean_value =+value;
//                                           }


                                  }
                              }
                          }

                          roi(int(x_com+0.5), int(y_com+0.5))=2;

                          mean_value /= static_cast<float>(grayvalues.size());


                          const auto median_it1 = grayvalues.begin() + grayvalues.size() / 2 - 1;
                          const auto median_it2 = grayvalues.begin() + grayvalues.size() / 2;
                          std::nth_element(grayvalues.begin(), median_it1 , grayvalues.end()); // e1
                          std::nth_element(grayvalues.begin(), median_it2 , grayvalues.end()); // e2
                          float median = (grayvalues.size() % 2 == 0) ? (*median_it1 + *median_it2) / 2 : *median_it2; // here I compute the median
                          float average = std::accumulate( grayvalues.begin(), grayvalues.end(), 0.0)/ grayvalues.size();
                          values.insert(std::make_pair(temp,median));
//                          values.insert(std::make_pair(temp,average));
                 }

              }

      }
}

void ReferenceImageCorrection::SetBBInterpRoi(const std::vector<size_t> &roi)
{
    m_nBlackBodyROI = roi;
}

void ReferenceImageCorrection::SetBBInterpDoseRoi(const std::vector<size_t> &roi)
{
    m_nDoseBBRoi = roi;
}

std::vector<float> ReferenceImageCorrection::ComputeInterpolationParameters(kipl::base::TImage<float,2>&mask, kipl::base::TImage<float,2>&img)
{
    std::map<std::pair<int,int>, float> values;

    float mean_value = 0.0f;
    int value_size = 0;

    for (size_t y=0; y<mask.Size(1); ++y)
    {
        for (size_t x=0; x<mask.Size(0); ++x)
        {
            if (mask(x,y)==1){
                std::pair<int, int> temp;
                temp = std::make_pair(x+m_diffBBroi[0],y+m_diffBBroi[1]);// m_diffBBroi compensates for the relative position of BBroi in the images 
//                if (img(x,y)!=0)
//                {
                    values.insert(std::make_pair(temp,img(x,y)));
                    mean_value +=img(x,y);
                    value_size++;
//                }

            }

        }

    }

//    mean_value/=values.size();
      mean_value/=static_cast<float>(value_size);

    // remove outliers if values to be interpolated are above mean +2std

    float std_dev = 0.0f;

    for (std::map<std::pair<int,int>, float>::const_iterator it = values.begin();
                it != values.end(); ++it) {
        std_dev += (it->second-mean_value)*(it->second-mean_value);
    }

    std_dev=sqrt(std_dev/values.size());



    // this should be the correct way of removing element (STL book, page 205)

    for (std::map<std::pair<int,int>, float>::const_iterator it = values.begin(); it!= values.end(); ){
        if(it->second >= (mean_value+2*std_dev)){
            values.erase(it++);
        }
        else{
            ++it;
        }
    }


    float error;
    auto myparam = SolveLinearEquation(values, error);

    return myparam;
}

std::vector<float> ReferenceImageCorrection::ComputeInterpolationParameters(kipl::base::TImage<float,2>&mask, kipl::base::TImage<float,2>&img, float &error)
{

    // IMPORTANT! WHEN THE A SPECIFIC ROI IS SELECTED FOR THE BBs, THEN THE INTERPOLATION PARAMETERS NEED TO BE COMPUTED ON THE BIGGER IMAGE GRID
    std::map<std::pair<int,int>, float> values;
    float mean_value = 0.0f;

    for (size_t y=0; y<mask.Size(1); ++y)
    {
        for (size_t x=0; x<mask.Size(0); ++x)
        {
            if (mask(x,y)==1.0f)
            {
                std::pair<int,int> temp;
                temp = std::make_pair(x+m_diffBBroi[0],y+m_diffBBroi[1]);// m_diffBBroi compensates for the relative position of BBroi in the images. now it should be in absolute coordinates
                values.insert(std::make_pair(temp,img(x,y)));
                mean_value +=img(x,y);

            }
        }
    }

    mean_value/=values.size();

    // remove outliers if values to be interpolated are above mean +2std

    float std_dev = 0.0f;

    for (std::map<std::pair<int,int>, float>::const_iterator it = values.begin();
                it != values.end(); ++it) {
        std_dev += (it->second-mean_value)*(it->second-mean_value);
    }

    std_dev=sqrt(std_dev/values.size());


    for (std::map<std::pair<int,int>, float>::const_iterator it = values.begin(); it!= values.end(); ){
        if(it->second >= (mean_value+2*std_dev)){
            values.erase(it++);
        }
        else{
            ++it;
        }
    }

    float my_error = 0.0f;
    auto myparam = SolveLinearEquation(values, my_error);
    error = my_error;

    return myparam;
}

std::vector<float> ReferenceImageCorrection::SolveLinearEquation(std::map<std::pair<int, int>, float> &values, float &error)
{
    int matrix_size = 1 + ((d && a)+b+c+f+e);

    arma::mat my_matrix(values.size(),matrix_size);
    arma::vec I(values.size());
    arma::vec param(matrix_size);
    std::map<std::pair<int, int>, float>::const_iterator it = values.begin();

    for (arma::uword x=0; x<my_matrix.n_rows; x++)
    {

        if (a && b && c && d && e && f)
        { // second order X, second order Y
              my_matrix.at(x,0) = 1;
              my_matrix.at(x,1) = it->first.first;
              my_matrix.at(x,2) = it->first.first*it->first.first;
              my_matrix.at(x,3) = it->first.first*it->first.second;
              my_matrix.at(x,4) = it->first.second;
              my_matrix.at(x,5) = it->first.second*it->first.second;
        }
        if ((b && d && a && c) && (e==0) && (f==0))
        { // first order X, first order Y
            my_matrix.at(x,0) = 1;
            my_matrix.at(x,1) = it->first.first;
            my_matrix.at(x,2) = it->first.second;
            my_matrix.at(x,3) = it->first.first*it->first.second;
        }
        if (b==0 && a==0 && c==0 && d==0 && e==0 && f==0) { // zero order X, zero order Y
            my_matrix.at(x,0) = 1;
        }

        if ((b && d && e && c && a) && f==0 ) { // second order X, first order Y
            my_matrix.at(x,0) = 1;
            my_matrix.at(x,1) = it->first.first;
            my_matrix.at(x,2) = it->first.first*it->first.first;
            my_matrix.at(x,3) = it->first.first*it->first.second;
            my_matrix.at(x,4) = it->first.second;
        }
        if ((b && d && e) && c==0 && a==0 && f==0 ) { // second order X, zero order Y
            my_matrix.at(x,0) = 1;
            my_matrix.at(x,1) = it->first.first;
            my_matrix.at(x,2) = it->first.first*it->first.first;
        }

        if ((b && c && d && a && f) && e==0){ // first order X, second order Y
            my_matrix.at(x,0) = 1;
            my_matrix.at(x,1) = it->first.first;
            my_matrix.at(x,2) = it->first.first*it->first.second;
            my_matrix.at(x,3) = it->first.second;
            my_matrix.at(x,4) = it->first.second*it->first.second;
        }

        if( (b && d) && a==0 && c==0 &&  e==0 && f==0){ //first order X, zero order Y
            my_matrix.at(x,0) = 1;
            my_matrix.at(x,1) = it->first.first;
        }

        if (b==0 && d==0 && e==0 && (c && a && f)){ // zero order X, second order Y
            my_matrix.at(x,0) = 1;
//            my_matrix[x][1] = it->first.first;
//            my_matrix[x][2] = it->first.first*it->first.first;
//            my_matrix[x][3] = it->first.first*it->first.second;
            my_matrix.at(x,1) = it->first.second;
            my_matrix.at(x,2) = it->first.second*it->first.second;
        }

        if ((c && a) && b==0 && d==0 && e==0 && f==0){ // zero order X, first order Y
            my_matrix.at(x,0) = 1;
            my_matrix.at(x,1) = it->first.second;

        }

              I[x] = static_cast<double>(it->second);
              it++;

    }

    param = arma::solve(my_matrix,I);

    std::vector<float> myparam(6,0.0f);

    arma::vec I_int(values.size());

    // here my param must be in the same order...


    it = values.begin();
  for (int i=0; i<static_cast<int>(values.size()); i++){

      if (a && b && c && d && e && f) {
//            I_int[i] = static_cast<float>(param[0]) + static_cast<float>(param[1])*static_cast<float>(it->first.first)+static_cast<float>(param[2])*static_cast<float>(it->first.first*it->first.first)+static_cast<float>(param[3])*static_cast<float>(it->first.first)*static_cast<float>(it->first.second)+static_cast<float>(param[4])*static_cast<float>(it->first.second)+static_cast<float>(param[5])*static_cast<float>(it->first.second*it->first.second);
           // img(x,y) = static_cast<float>(param[0]) + static_cast<float>(param[1])*static_cast<float>(x)+static_cast<float>(param[2])*static_cast<float>(x)*static_cast<float>(x)+static_cast<float>(param[3])*static_cast<float>(x)*static_cast<float>(y)+static_cast<float>(param[4])*static_cast<float>(y)+static_cast<float>(param[5])*static_cast<float>(y)*static_cast<float>(y);
           // fill parameters in the right order:

            myparam[0] = static_cast<float>(param[0]); // 0
            myparam[1] = static_cast<float>(param[1]); // x
            myparam[2] = static_cast<float>(param[2]); // x^2
            myparam[3] = static_cast<float>(param[3]); // xy
            myparam[4] = static_cast<float>(param[4]); // y
            myparam[5] = static_cast<float>(param[5]); // y^2

//            I_int[i] = myparam[0] + myparam[1]*static_cast<float>(it->first.first)+myparam[2]*static_cast<float>(it->first.first*it->first.first)+myparam[3]*static_cast<float>(it->first.first)*static_cast<float>(it->first.second)+myparam[4]*static_cast<float>(it->first.second)+myparam[5]*static_cast<float>(it->first.second*it->first.second);
      }
      if ((b && d && a && c) && (e==0) && (f==0)){
//          I_int[i] = static_cast<float>(param[0]) + static_cast<float>(param[1])*static_cast<float>(it->first.first)+static_cast<float>(param[2])*static_cast<float>(it->first.second)+static_cast<float>(param[3])*static_cast<float>(it->first.first)*static_cast<float>(it->first.second);

          myparam[0] = static_cast<float>(param[0]); // 0
          myparam[1] = static_cast<float>(param[1]); // x
//          myparam[2] = static_cast<float>(param[2]); // x^2
          myparam[3] = static_cast<float>(param[3]); // xy
          myparam[4] = static_cast<float>(param[2]); // y
//          myparam[5] = static_cast<float>(param[5]); // y^2
      }
      if (b==0 && a==0 && c==0 && d==0 && e==0 && f==0) {
//          I_int[i] = static_cast<float>(param[0]);
          myparam[0] = static_cast<float>(param[0]); // 0
      }
      if ((b && d && e && c && a) && f==0 ) {
//          I_int[i] = static_cast<float>(param[0]) + static_cast<float>(param[1])*static_cast<float>(it->first.first)+static_cast<float>(param[2])*static_cast<float>(it->first.first*it->first.first)+static_cast<float>(param[3])*static_cast<float>(it->first.first)*static_cast<float>(it->first.second)+static_cast<float>(param[4])*static_cast<float>(it->first.second);
          myparam[0] = static_cast<float>(param[0]); // 0
          myparam[1] = static_cast<float>(param[1]); // x
          myparam[2] = static_cast<float>(param[2]); // x^2
          myparam[3] = static_cast<float>(param[3]); // xy
          myparam[4] = static_cast<float>(param[4]); // y
      }
      if ((b && d && e) && c==0 && a==0 && f==0 ) {
//          I_int[i] = static_cast<float>(param[0]) + static_cast<float>(param[1])*static_cast<float>(it->first.first)+static_cast<float>(param[2])*static_cast<float>(it->first.first*it->first.first);
          myparam[0] = static_cast<float>(param[0]); // 0
          myparam[1] = static_cast<float>(param[1]); // x
          myparam[2] = static_cast<float>(param[2]); // x^2
      }

      if((b && c && d && a && f) && e==0){
//          I_int[i] = static_cast<float>(param[0]) + static_cast<float>(param[1])*static_cast<float>(it->first.first)+static_cast<float>(param[2])*static_cast<float>(it->first.first*it->first.first)+static_cast<float>(param[3])*static_cast<float>(it->first.first)*static_cast<float>(it->first.second)+static_cast<float>(param[4])*static_cast<float>(it->first.second)+static_cast<float>(param[5])*static_cast<float>(it->first.second*it->first.second);
         // img(x,y) = static_cast<float>(param[0]) + static_cast<float>(param[1])*static_cast<float>(x)+static_cast<float>(param[2])*static_cast<float>(x)*static_cast<float>(x)+static_cast<float>(param[3])*static_cast<float>(x)*static_cast<float>(y)+static_cast<float>(param[4])*static_cast<float>(y)+static_cast<float>(param[5])*static_cast<float>(y)*static_cast<float>(y);
         // fill parameters in the right order:

          myparam[0] = static_cast<float>(param[0]); // 0
          myparam[1] = static_cast<float>(param[1]); // x
//          myparam[2] = static_cast<float>(param[2]); // x^2
          myparam[3] = static_cast<float>(param[2]); // xy
          myparam[4] = static_cast<float>(param[3]); // y
          myparam[5] = static_cast<float>(param[4]); // y^2
      }

      if( (b && d) && a==0 && c==0 &&  e==0 && f==0){
          myparam[0] = static_cast<float>(param[0]); // 0
          myparam[1] = static_cast<float>(param[1]); // x
      }

      if (b==0 && d==0 && e==0 && (c && a && f)) {
          myparam[0] = static_cast<float>(param[0]); // 0
//          myparam[1] = static_cast<float>(param[1]); // x
//          myparam[2] = static_cast<float>(param[2]); // x^2
//          myparam[3] = static_cast<float>(param[2]); // xy
          myparam[4] = static_cast<float>(param[1]); // y
          myparam[5] = static_cast<float>(param[2]); // y^2

      }

      if ((c && a) && b==0 && d==0 && e==0 && f==0) {
          myparam[0] = static_cast<float>(param[0]); // 0
          myparam[4] = static_cast<float>(param[1]); // y

      }



      I_int[i] = myparam[0] + myparam[1]*static_cast<float>(it->first.first)+myparam[2]*static_cast<float>(it->first.first*it->first.first)+myparam[3]*static_cast<float>(it->first.first)*static_cast<float>(it->first.second)+myparam[4]*static_cast<float>(it->first.second)+myparam[5]*static_cast<float>(it->first.second*it->first.second);
      ++it;
  }


    error = 0.0f;

    for (int i=0; i<values.size(); i++){
          error += (((I_int[i]-I[i])/I[i])*((I_int[i]-I[i])/I[i]));
    }

    error = sqrt(1/float(values.size())*error);

    return myparam;


}

std::vector<float> ReferenceImageCorrection::SolveThinPlateSplines(std::map<std::pair<int,int>,float> &values) {



    //1. create system matrix, dimension of the matrix depends on the number of BB found


    arma::mat my_matrix(values.size()+3,values.size()+3);
    arma::vec b_vector(values.size()+3);
    arma::vec qr_param(values.size()+3);


    std::map<std::pair<int,int>, float>::const_iterator it_i = values.begin();
    for (size_t i=0; i<values.size(); ++i) {
            std::map<std::pair<int,int>, float>::const_iterator it_j = values.begin();
        for (size_t j=0; j<values.size(); ++j) {

            if (i==j){
                my_matrix.at(i,j) =  0.0f;
            }
            else {
                float ii_1 = static_cast<float>(it_i->first.first);
                float ii_2 = static_cast<float>(it_i->first.second);
                float jj_1 = static_cast<float>(it_j->first.first);
                float jj_2 = static_cast<float>(it_j->first.second);
                float dist = sqrt((ii_1-jj_1)*(ii_1-jj_1)+(ii_2-jj_2)*(ii_2-jj_2));
  //              float dist = sqrt((it_i->first.first-it_j->first.first)*(it_i->first.first-it_j->first.first)+(it_i->first.second-it_j->first.second)*(it_i->first.second-it_j->first.second));
//                float dist = sqrt((values.at(i).first.first-values.at(j).first.first)*(values.at(i).first.first-values.at(j).first.first)+(values.at(i).first.second-values.at(j).first.second)*(values.at(i).first.second-values.at(j).first.second)); // does not work like this...
                my_matrix.at(i,j) = dist*dist*log(dist);

            }
            ++it_j;
        }
        ++it_i;
    }

    it_i = values.begin();
    for (int i=0; i<values.size(); ++i){
        my_matrix.at(i,values.size()) = 1.0;
        my_matrix.at(i,values.size()+1) = it_i->first.second; // y
        my_matrix.at(i,values.size()+2) = it_i->first.first; // x
        ++it_i;
    }

    it_i = values.begin();
    for (int i=0; i<values.size(); ++i){
        my_matrix.at(values.size(),i) = 1.0;
        my_matrix.at(values.size()+1,i) = it_i->first.second; // y
        my_matrix.at(values.size()+2,i) = it_i->first.first; // x
        ++it_i;
    }

    int i=0;
    for (it_i = values.begin(); it_i!=values.end(); ++it_i){
        b_vector.at(i) = it_i->second;
        ++i;
    }

    qr_param = arma::solve(my_matrix,b_vector);

    std::vector<float> param(values.size()+3,0.0f);
    for (i=0; i<values.size()+3; ++i)
    {
        param[i] = static_cast<float>(qr_param[i]);
    }


    return param;

}

kipl::base::TImage<float,2> ReferenceImageCorrection::InterpolateBlackBodyImagewithSplines(const std::vector<float> &parameters, std::map<std::pair<int, int>, float> &values, const std::vector<size_t> &roi){



    size_t dimx = roi[2]-roi[0];
    size_t dimy = roi[3]-roi[1];


    std::vector<size_t> dims = {dimx,dimy};
    kipl::base::TImage <float,2> interpolated_img(dims);
    interpolated_img = 0.0f;

    float sumdist;
    std::map<std::pair<int,int>, float>::const_iterator it_i;

    for (size_t x=0; x<dimx; ++x) {
        for (size_t y=0; y<dimy; ++y){
             it_i = values.begin();

                sumdist = 0.0;
                float *dist = new float[values.size()];

            for (size_t ind=0; ind<values.size(); ++ind) {
                dist[ind] = ((it_i->first.first-(x+roi[0]))*(it_i->first.first-(x+roi[0]))+(it_i->first.second-(y+roi[1]))*(it_i->first.second-(y+roi[1])));
                if (dist[ind]!=0){
                    sumdist += (0.5*dist[ind]*log(dist[ind])*parameters[ind]); // correct?                }
                }

                ++it_i;
            }

                delete [] dist;

            interpolated_img(x,y) = sumdist+parameters[values.size()]+parameters[values.size()+1]*(y+roi[1])+parameters[values.size()+2]*(x+roi[0]);
        }
    }

    return interpolated_img;




}

kipl::base::TImage<float,2> ReferenceImageCorrection::InterpolateBlackBodyImage(const std::vector<float> &parameters, const std::vector<size_t> &roi) {


    size_t dimx = roi[2]-roi[0];
    size_t dimy = roi[3]-roi[1];

    std::vector<size_t> dims = {dimx,dimy};
    kipl::base::TImage <float,2> interpolated_img(dims);
    interpolated_img = 0.0f;

    for (size_t x=0; x<dimx; x++) {
        for (size_t y=0; y<dimy; y++){
            interpolated_img(x,y) = parameters[0] + parameters[1]*static_cast<float>(x+roi[0])+parameters[2]*static_cast<float>(x+roi[0])*static_cast<float>(x+roi[0])+parameters[3]*static_cast<float>(x+roi[0])*static_cast<float>(y+roi[1])+parameters[4]*static_cast<float>(y+roi[1])+parameters[5]*static_cast<float>(y+roi[1])*static_cast<float>(y+roi[1]);
        }
    }

    return interpolated_img;

}

float ReferenceImageCorrection::ComputeInterpolationError(kipl::base::TImage<float,2>&interpolated_img, kipl::base::TImage<float,2>&mask, kipl::base::TImage<float,2>&img) {

    float error = 0.0f;
    int  nValues = 0;

      for(size_t ind=0; ind<static_cast<int>(img.Size()); ind++)
          {
              if (mask(ind)!=0) {
                        error += ((interpolated_img(ind)-img(ind))/interpolated_img(ind))*((interpolated_img(ind)-img(ind))/interpolated_img(ind));
                       nValues++;
              }
          }

  error = sqrt(1/float(nValues)*error);

  return error;

}

std::vector<float> ReferenceImageCorrection::PrepareBlackBodyImage(kipl::base::TImage<float, 2> &flat,
                                                        kipl::base::TImage<float, 2> &dark,
                                                        kipl::base::TImage<float, 2> &bb,
                                                        kipl::base::TImage<float,2>  &mask)
{
    if (!kipl::base::checkEqualSize(flat,dark))
        throw kipl::base::DimsException("Size mismatch: flat != dark",__FILE__,__LINE__);

    if (!kipl::base::checkEqualSize(flat,bb))
        throw kipl::base::DimsException("Size mismatch: flat != bb",__FILE__,__LINE__);

    // 1. normalize image

    kipl::base::TImage<float, 2> norm(bb.dims());
    kipl::base::TImage<float, 2> normdc(bb.dims());

    norm.Clone(bb);
    normdc.Clone(bb);
//    memcpy(norm.GetDataPtr(),bb.GetDataPtr(), sizeof(float)*bb.Size());
//    memcpy(normdc.GetDataPtr(),bb.GetDataPtr(), sizeof(float)*bb.Size());

    normdc -= dark;
    norm   -= dark;
    flat   -= dark;
    norm   /= flat;

    try{
        SegmentBlackBody(norm, mask);
//        SegmentBlackBody(norm, normdc, mask, values); // try for thin plates spline
    }
    catch (...) {
        throw ImagingException("SegmentBlackBodyNorm failed", __FILE__, __LINE__);
    }

//    kipl::io::WriteTIFF(mask,"mask.tif",kipl::base::Float32);

    kipl::base::TImage<float,2> BB_DC(bb.dims());
    memcpy(BB_DC.GetDataPtr(), bb.GetDataPtr(), sizeof(float)*bb.Size());
    BB_DC-=dark;

    kipl::base::TImage<float, 2> interpolated_BB(bb.dims());
    interpolated_BB = 0.0f;

    auto param = ComputeInterpolationParameters(mask,BB_DC);

    return param;

}

std::vector<float> ReferenceImageCorrection::PrepareBlackBodyImagewithSplines(kipl::base::TImage<float, 2> &flat, kipl::base::TImage<float, 2> &dark, kipl::base::TImage<float, 2> &bb, kipl::base::TImage<float, 2> &mask, std::map<std::pair<int, int>, float> &values)
{
    // 1. normalize image
    kipl::base::TImage<float, 2> norm(bb.dims());
    kipl::base::TImage<float, 2> normdc(bb.dims());
    memcpy(norm.GetDataPtr(),bb.GetDataPtr(), sizeof(float)*bb.Size());
    memcpy(normdc.GetDataPtr(),bb.GetDataPtr(), sizeof(float)*bb.Size());

    normdc -=dark;
    norm -=dark;
    norm /= (flat-=dark);

    try{
        SegmentBlackBody(norm, normdc, mask, values);
//         kipl::io::WriteTIFF(mask, "maskSpline.tif",kipl::base::Float32);
    }
    catch (...) {
        throw ImagingException("SegmentBlackBodyNorm failed", __FILE__, __LINE__);
    }

    //float *tps_param = new float[values.size()+3];
    auto tps_param = SolveThinPlateSplines(values);

    return tps_param;
}

std::vector<float> ReferenceImageCorrection::PrepareBlackBodyImagewithSplinesAndMask(kipl::base::TImage<float, 2> &dark, kipl::base::TImage<float, 2> &bb, kipl::base::TImage<float, 2> &mask, std::map<std::pair<int, int>, float> &values)
{

    kipl::base::TImage<float,2> BB_DC(bb.dims());
    memcpy(BB_DC.GetDataPtr(), bb.GetDataPtr(), sizeof(float)*bb.Size());
    BB_DC-=dark;
    ComputeBlackBodyCentroids(BB_DC,mask, values);

//    float *tps_param = new float[values.size()+3];
    auto tps_param = SolveThinPlateSplines(values);

    return tps_param;
}


std::vector<float> ReferenceImageCorrection::PrepareBlackBodyImage(kipl::base::TImage<float, 2> &flat, kipl::base::TImage<float, 2> &dark, kipl::base::TImage<float, 2> &bb, kipl::base::TImage<float,2> &mask, float &error)
{
    // 1. normalize image

    kipl::base::TImage<float, 2> norm(bb.dims());
    kipl::base::TImage<float, 2> normdc(bb.dims());
    memcpy(norm.GetDataPtr(),bb.GetDataPtr(), sizeof(float)*bb.Size());
    memcpy(normdc.GetDataPtr(),bb.GetDataPtr(), sizeof(float)*bb.Size());

    normdc -= dark;
    norm   -= dark;
    norm   /= (flat-=dark); // TODO: is this a good idea?

    try
    {
        SegmentBlackBody(norm, mask);
    }
    catch (kipl::base::KiplException &e)
    {
        logger.error(e.what());
        std::cerr<<"Error in the SegmentBlackBody function\n";
        throw ImagingException("SegmentBlackBodyNorm failed", __FILE__, __LINE__);
    }

    kipl::base::TImage<float,2> BB_DC(bb.dims());
    memcpy(BB_DC.GetDataPtr(), bb.GetDataPtr(), sizeof(float)*bb.Size());
    BB_DC-=dark;

    kipl::base::TImage<float, 2> interpolated_BB(bb.dims());
    interpolated_BB = 0.0f;

    //float * param = new float[6];
    float myerror;

    auto param = ComputeInterpolationParameters(mask,BB_DC,myerror);
    error = myerror;

    return param;
}

std::vector<float> ReferenceImageCorrection::PrepareBlackBodyImagewithMask(kipl::base::TImage<float, 2> &dark, kipl::base::TImage<float, 2> &bb, kipl::base::TImage<float, 2> &mask)
{
    kipl::base::TImage<float,2> BB_DC(bb.dims());
    memcpy(BB_DC.GetDataPtr(), bb.GetDataPtr(), sizeof(float)*bb.Size());
    BB_DC-=dark;

//    float * param = new float[6];
    auto param = ComputeInterpolationParameters(mask,BB_DC);

    return param;
}

std::vector<float> ReferenceImageCorrection::PrepareBlackBodyImagewithMask(kipl::base::TImage<float, 2> &dark, kipl::base::TImage<float, 2> &bb, kipl::base::TImage<float, 2> &mask, float &error)
{
    kipl::base::TImage<float,2> BB_DC(bb.dims());
//    float * param = new float[6];
    float myerror;

    memcpy(BB_DC.GetDataPtr(), bb.GetDataPtr(), sizeof(float)*bb.Size());
    BB_DC-=dark;
    auto param = ComputeInterpolationParameters(mask,BB_DC,myerror);
    error = myerror;

    return param;
}

void ReferenceImageCorrection::SetAngles(float *ang, size_t nProj, size_t nBB)
{
    angles[0] = ang[0];
    angles[1] = ang[1];
    angles[2] = ang[2];
    angles[3] = ang[3];

    m_nProj     = nProj; // to set eventually around where it is used
    m_nBBimages = nBB;
}

void ReferenceImageCorrection::SetDoseList(const std::vector<float> & doselist)
{
    if (m_nProj!=0)
    {
        dosesamplelist = doselist;
    }
    else
    {
        throw ImagingException("m_nProj was not set before calling ReferenceImageCorrection::SetDoseList",__FILE__,__LINE__);
    }
}

void ReferenceImageCorrection::SetSplinesParameters(const std::vector<float> &ob_parameter,
                                                    std::vector<float> &sample_parameter,
                                                    size_t nBBSampleCount,
                                                    size_t nProj,
                                                    eBBOptions ebo,
                                                    int nBBs)
{
    m_nProj = nProj; // not very smartly defined
    ob_bb_parameters.resize(nBBs+3);
//    ob_bb_parameters = new float[nBBs+3];
//    sample_bb_interp_parameters = new float[(nBBs+3)*m_nProj];
    sample_bb_interp_parameters.resize((nBBs+3)*m_nProj);

    m_nBBimages = nBBSampleCount; // store how many images with BB and sample i do have
//    memcpy(ob_bb_parameters, ob_parameter, sizeof(float)*(nBBs+3));
    std::copy_n(ob_parameter.begin(),nBBs+3,ob_bb_parameters.begin());
//    for (size_t i=0; i<nBBs+3; ++i){
//        ob_bb_parameters[i]=0.0f;
//    }

    switch(ebo) {
    case(Interpolate) : {
        sample_bb_parameters.resize((nBBs+3)*m_nBBimages);
//        sample_bb_parameters = new float[(nBBs+3)*m_nBBimages];
//        memcpy(sample_bb_parameters, sample_parameter, sizeof(float)*(nBBs+3)*m_nBBimages);
        std::copy_n(sample_parameter.begin(),(nBBs+3)*m_nBBimages, sample_bb_parameters.begin());

        sample_bb_interp_parameters = InterpolateSplineGeneric(sample_bb_parameters, nBBs);

        for (size_t i=0; i<m_nProj; i++)
        {
            for (size_t j=0; j<(nBBs+3); j++)
            {
                sample_bb_interp_parameters[j+i*(nBBs+3)] *= (dosesamplelist[i]/tau);
            }
        }

        break;
    }
    case(OneToOne) : {


        if (m_nBBimages!=m_nProj){
            throw ImagingException("The number of BB images is not the same as the number of Projection images",__FILE__, __LINE__);
        }



        for (size_t i=0; i<m_nProj; i++){

            for (size_t j=0; j<(nBBs+3); j++){          
                sample_parameter[j+i*(nBBs+3)] *= (dosesamplelist[i]/tau);
            }
        }

//        memcpy(sample_bb_interp_parameters, sample_parameter, sizeof(float)*(nBBs+3)*m_nProj);
        std::copy_n(sample_parameter.begin(),(nBBs+3)*m_nProj,sample_bb_interp_parameters.begin());

        break;
    }
    case(Average) : {

        sample_bb_parameters.resize(nBBs+3);
        std::vector<float> temp_parameters(nBBs+3);
        std::copy_n(sample_parameter.begin(),nBBs+3,temp_parameters.begin());
//        memcpy(temp_parameters, sample_parameter, sizeof(float)*(nBBs+3));

//        sample_bb_interp_parameters = ReplicateParameters(sample_bb_parameters, nProj);

        for (size_t i=0; i<m_nProj; ++i)
        {

//            memcpy(sample_bb_parameters, temp_parameters, sizeof(float)*(nBBs+3));
            std::copy_n(temp_parameters.begin(),nBBs+3,sample_bb_parameters.begin());
            for (ptrdiff_t j=0; j<(nBBs+3); ++j)
            {
                sample_bb_parameters[j] *= (dosesamplelist[i]/tau);
            }

            std::copy_n(sample_bb_parameters.begin(),
                        (nBBs+3),
                        sample_bb_interp_parameters.begin()+i*(nBBs+3));
        }

        break;
    }
    }
}


void ReferenceImageCorrection::SetInterpParameters(const std::vector<float> &ob_parameter,
                                                   std::vector<float> &sample_parameter,
                                                   size_t nBBSampleCount,
                                                   size_t nProj,
                                                   eBBOptions ebo)
{

    m_nProj = nProj; // not very smartly defined
    ob_bb_parameters.resize(6,0.0f);
    sample_bb_interp_parameters.resize(6*m_nProj,0.0f);

    m_nBBimages = nBBSampleCount; // store how many images with BB and sample i do have
    std::copy_n(ob_parameter.begin(),6,ob_bb_parameters.begin());
//    memcpy(ob_bb_parameters, ob_parameter, sizeof(float)*6);
//    ob_bb_parameters[0] = 0.0f;
//    ob_bb_parameters[1] = 0.0f;
//    ob_bb_parameters[2] = 0.0f;
//    ob_bb_parameters[3] = 0.0f;
//    ob_bb_parameters[4] = 0.0f;
//    ob_bb_parameters[5] = 0.0f;

    switch(ebo) {
    case(Interpolate) : {

        sample_bb_parameters.resize(6*m_nBBimages);
//        memcpy(sample_bb_parameters, sample_parameter, sizeof(float)*6*m_nBBimages);
        std::copy_n(sample_parameter.begin(),6*m_nBBimages,sample_bb_parameters.begin());
        sample_bb_interp_parameters = InterpolateParametersGeneric(sample_bb_parameters);

        for (size_t i=0; i<m_nProj; i++){
            for (size_t j=0; j<6; j++){
                sample_bb_interp_parameters[j+i*6] *= (dosesamplelist[i]/tau);
            }
        }

        break;
    }
    case(OneToOne) : {

        if (m_nBBimages!=m_nProj){
            throw ImagingException("The number of BB images is not the same as the number of Projection images",__FILE__, __LINE__);
        }



        for (size_t i=0; i<m_nProj; i++){
            for (size_t j=0; j<6; j++){
                sample_parameter[j+i*6] *= (dosesamplelist[i]/tau);
            }
        }

//       memcpy(sample_bb_interp_parameters, sample_parameter, sizeof(float)*6*m_nProj);
        std::copy_n(sample_parameter.begin(),6*m_nProj,sample_bb_interp_parameters.begin());

        break;
    }
    case(Average) : {

        sample_bb_parameters.resize(6);
        std::vector<float> temp_parameters(6,0.0f);
        std::copy_n(sample_parameter.begin(),6,temp_parameters.begin());
//        memcpy(temp_parameters, sample_parameter, sizeof(float)*6);

//        sample_bb_interp_parameters = ReplicateParameters(sample_bb_parameters, nProj);

        for (size_t i=0; i<m_nProj; ++i){

            sample_bb_parameters=temp_parameters;
            for (size_t j=0; j<6; ++j){
                sample_bb_parameters[j] *= (dosesamplelist[i]/tau);
            }

            std::copy_n(sample_bb_parameters.begin(),6,sample_bb_interp_parameters.begin()+i*6);

        }

        break;
    }
    }


}

std::vector<float> ReferenceImageCorrection::InterpolateSplineGeneric(const std::vector<float> &param, int nBBs)
{
    // it is assumed that angles are in order
    std::vector<float> interpolated_param((nBBs+3)*m_nProj,0.0f);
    float curr_angle;
    float *small_a;
    float *big_a;
    float *bb_angles = new float[m_nBBimages+1];
    float step;
    int   index_1=0;
    int   index_2=0;

    if(angles[0]<angles[2]) {

        bb_angles[0] = angles[3]-angles[1];

        for (size_t i=0; i<m_nBBimages; i++)
        {
            bb_angles[i+1]= angles[2]+(angles[3]-angles[2])/(m_nBBimages-1)*i; // this is now more generic starting at angle angles[2]
        }

         index_1=m_nBBimages*(nBBs+3)-(nBBs+3);
         index_2=0;

    }
    else {

       for (size_t i=0; i<m_nBBimages; i++)
       {
           bb_angles[i]= angles[2]+(angles[3]-angles[2])/(m_nBBimages-1)*i; // this is now more generic starting at angle angles[2]
           index_1=0;
           index_2=(nBBs+3);
       }
    }

    small_a = &bb_angles[0];
    big_a = &bb_angles[1];

    for (size_t i=0; i<m_nProj; i++)
    {
    //1. compute current angle for projection

        curr_angle = angles[0]+(angles[1]-angles[0])/(m_nProj-1)*i; // that is the current projection angle


    //2. compute step for linear interpolation

         step = (curr_angle-*small_a)/(*big_a-*small_a);

     // 3. interpolate for missing angles
        std::vector<float> temp_param(nBBs+3);

        for (ptrdiff_t k=0; k<(nBBs+3);k++)
        {
            temp_param[k] = param[index_1+k]+(step)*(param[index_2+k]-param[index_1+k]);
        }

//        memcpy(interpolated_param+i*(nBBs+3),temp_param,sizeof(float)*(nBBs+3));
        std::copy_n(temp_param.begin(),nBBs+3,interpolated_param.begin()+i*(nBBs+3));

        if (curr_angle>=*big_a){ // se esco dal range incremento di 1

            if(curr_angle>=angles[3]) {

                if (*big_a==angles[3]) { // if pointers have not been modified yes, change the index, otherwise keep them as they are
                    index_1=index_2;
                    index_2 =0;
                }
                small_a = &angles[3];
                *big_a = (angles[1]+angles[2]);


            }
            else {
                index_1 =index_2;
                index_2 +=(nBBs+3);
                small_a++;
                big_a++;
            }
        }




    }

    delete [] bb_angles;

    return interpolated_param;

}

std::vector<float> ReferenceImageCorrection::InterpolateParametersGeneric(const std::vector<float> & param)
{

    // it is assumed that angles are in order
    std::vector<float> interpolated_param(6*m_nProj);
    float curr_angle;
    float *small_a;
    float *big_a;
    float *bb_angles;
    float step;
    int index_1;
    int index_2;



    if(angles[0]<angles[2])
    {
        bb_angles = new float[m_nBBimages+1];
        bb_angles[0] = angles[3]-angles[1];

        for (size_t i=0; i<m_nBBimages; i++)
        {
            bb_angles[i+1]= angles[2]+(angles[3]-angles[2])/(m_nBBimages-1)*i; // this is now more generic starting at angle angles[2]
        }

         index_1=m_nBBimages*6-6;
         index_2=0;

    }
    else
    {

        bb_angles = new float[m_nBBimages];

       for (size_t i=0; i<m_nBBimages; i++)
       {
           bb_angles[i]= angles[2]+(angles[3]-angles[2])/(m_nBBimages-1)*i; // this is now more generic starting at angle angles[2]
           index_1=0;
           index_2=6;
       }
    }

    small_a = &bb_angles[0];
    big_a = &bb_angles[1];

    for (size_t i=0; i<m_nProj; i++)
    {
    //1. compute current angle for projection

        curr_angle = angles[0]+(angles[1]-angles[0])/(m_nProj-1)*i; // that is the current projection angle


    //2. compute step for linear interpolation

         step = (curr_angle-*small_a)/(*big_a-*small_a);

     // 3. interpolate for missing angles
        std::vector<float> temp_param(6);

        for (size_t k=0; k<6;k++)
        {
            temp_param[k] = param[index_1+k]+(step)*(param[index_2+k]-param[index_1+k]);
        }

//        memcpy(interpolated_param+i*6,temp_param,sizeof(float)*6);
        std::copy_n(temp_param.begin(),6,interpolated_param.begin()+6*i);

        if (curr_angle>=*big_a)
        { // se esco dal range incremento di 1

            if(curr_angle>=angles[3])
            {

                if (*big_a==angles[3])
                { // if pointers have not been modified yes, change the index, otherwise keep them as they are
                    index_1=index_2;
                    index_2 =0;
                }
                small_a = &angles[3];
                *big_a = (angles[1]+angles[2]);


            }
            else
            {
                index_1 =index_2;
                index_2 +=6;
                small_a++;
                big_a++;
            }
        }
    }

    delete [] bb_angles;

    return interpolated_param;


}

std::vector<float> ReferenceImageCorrection::InterpolateParameters(const std::vector<float> &param, size_t n, size_t step){

    std::vector<float> interpolated_param(6*(n+1));

   // first copy the one that i already have
    size_t index = 0;
    for (size_t i=0; i<6*n; i+=step*6)
    {
//        memcpy(interpolated_param+i, param+index, sizeof(float)*6);
        std::copy_n(param.begin()+index,6,interpolated_param.begin()+i);
        index +=6;
    }

//    memcpy(interpolated_param+6*n, param, sizeof(float)*6); // copio i primi in coda
    std::copy_n(param.begin(),6,interpolated_param.begin()+6*n);

    for (size_t i=0; i<6*n; i+=step*6){
            index=i;

        for (size_t j=1; j<step; j++){
            std::vector<float> temp_param(6);

            for (size_t k=0; k<6; k++) {
                   temp_param[k] = interpolated_param[i+k]+(j)*(interpolated_param[i+k+step*6]-interpolated_param[i+k])/(step);
            }

            index +=6;
//            memcpy(interpolated_param+index,temp_param,sizeof(float)*6);
            std::copy_n(temp_param.begin(),6,interpolated_param.begin()+index);
        }
    }


    return interpolated_param;



}

float *ReferenceImageCorrection::ReplicateSplineParameters(float *param, size_t n, int nBBs)
{
    float *interpolated_param = new float[(nBBs+3)*(n+1)];
    int index=0;

        for (int i=0; i<=n; i++){
            memcpy(interpolated_param+index,param,sizeof(float)*(nBBs+3));
            index+=(nBBs+3);
        }

    return interpolated_param;
}


float * ReferenceImageCorrection::ReplicateParameters(float *param, size_t n)
{
    float *interpolated_param = new float[6*(n+1)];
    int index=0;

        for (int i=0; i<=n; i++){
            memcpy(interpolated_param+index,param,sizeof(float)*6);
            index+=6;
        }


    return interpolated_param;
}

// not used.
int* ReferenceImageCorrection::repeat_matrix(int* source, int count, int expand)
{
    int i, j;
    int* target = reinterpret_cast<int*>(malloc(sizeof(int)*count * expand));
    for (i = 0; i < count; ++i) {
        for (j = 0; j < expand; ++j) {
            target[i * expand + j] = source[i];
        }
    }
    return target;
}

void ReferenceImageCorrection::PrepareReferences()
{
    float dose=1.0f/(m_fOpenBeamDose-m_fDarkDose);

//    if (dose!=dose)
//        throw ReconException("The reference dose is a NaN",__FILE__,__LINE__);

    const int N=static_cast<int>(m_OpenBeam.Size());
    float *pFlat=m_OpenBeam.GetDataPtr();
    float *pDark=m_DarkCurrent.GetDataPtr();

//            kipl::io::WriteTIFF(m_OpenBeam,"prepare_ob.tif",kipl::base::Float32);
//            kipl::io::WriteTIFF(m_DarkCurrent,"prepare_dc.tif",kipl::base::Float32);


    if (!m_bHaveBlackBody) {

            if (m_bHaveDarkCurrent) {
                #pragma omp parallel for
                for (int i=0; i<N; i++) {
                    float fProjPixel=pFlat[i]-pDark[i];
                    if (fProjPixel<=0)
                        pFlat[i]=0;
                    else {
                        if (m_bComputeLogarithm)
                        {
                            pFlat[i]=log(fProjPixel)+log(dose);
                        }
                        else
                        {
                            pFlat[i]=fProjPixel*dose;
                        }
                    }

                }
            }
            else {
                #pragma omp parallel for
                for (int i=0; i<N; i++) {
                    float fProjPixel=pFlat[i];
                    if (fProjPixel<=0)
                        pFlat[i]=0;
                    else {
                        if (m_bComputeLogarithm)
                        {
                            pFlat[i]=log(fProjPixel)+log(dose);
                        }
                        else
                        {
                            pFlat[i] = fProjPixel*dose;
                        }
                        }
                }
            }
            }

//    kipl::io::WriteTIFF(m_OpenBeam,"log_ob.tif",kipl::base::Float32);

}

void ReferenceImageCorrection::PrepareReferencesBB()
{

    if (m_bHaveBlackBody) {

    const int N=static_cast<int>(m_OpenBeam.Size());
    float *pFlat=m_OpenBeam.GetDataPtr();
    float *pDark=m_DarkCurrent.GetDataPtr();

    float dose;
    float Pdose = 0.0f;

    float *pFlatBB = m_OB_BB_Interpolated.GetDataPtr();

        // set here the PB variante...
        if(bPBvariante){

            Pdose = computedose(m_DoseBBflat_image);
            dose = 1.0f/(m_fOpenBeamDose-m_fDarkDose-Pdose);

        }
        else {
            dose = 1.0f/(m_fOpenBeamDose-m_fDarkDose);
        }


        if (m_bHaveDarkCurrent) {
            #pragma omp parallel for
            for (int i=0; i<N; i++) {

//                if(m_bHaveBBDoseROI && m_bHaveDoseROI) {
//                    pFlatBB[i]*=(dose/tau); // the OB_BB is already dose corrected
//                }
                float fProjPixel=pFlat[i]-pDark[i]-pFlatBB[i];
                if (fProjPixel<=0)
                    pFlat[i]=0;
                else {
                    if (m_bComputeLogarithm)
                    {
                        pFlat[i]=log(fProjPixel)+log(dose);
                    }
                    else
                    {
                        pFlat[i]=fProjPixel*(dose);
                    }
                }

            }
        }
        else {

                #pragma omp parallel for
                for (int i=0; i<N; i++) {

                    float fProjPixel=pFlat[i]-pFlatBB[i];
                    if (fProjPixel<=0)
                        pFlat[i]=0;
                    else {
                        if (m_bComputeLogarithm)
                        {
                             pFlat[i]=log(fProjPixel)+log(dose);
                        }
                        else
                        {
                             pFlat[i]=(fProjPixel*dose);
                        }
                    }
                }

        }
    }
    else {
        // throw Exception
        throw ImagingException("Black Body error in PrepareReferencesBB",__FILE__,__LINE__);
        // not the right spot

    }




}

void ReferenceImageCorrection::PrepareReferencesExtBB(){

    if (m_bHaveExternalBlackBody) {

    const int N=static_cast<int>(m_OpenBeam.Size());
    float *pFlat=m_OpenBeam.GetDataPtr();
    float *pDark=m_DarkCurrent.GetDataPtr();

    float dose=1.0f/(m_fOpenBeamDose-m_fDarkDose-fdoseOB_ext);

    float *pFlatBB = m_OB_BB_ext.GetDataPtr();


        if (m_bHaveDarkCurrent) {
            #pragma omp parallel for
            for (int i=0; i<N; i++) {

                float fProjPixel=pFlat[i]-pDark[i]-pFlatBB[i];
                if (fProjPixel<=0)
                    pFlat[i]=0;
                else {
                    if (m_bComputeLogarithm)
                        pFlat[i]=log(fProjPixel*(dose));
                    else
                        pFlat[i]=(fProjPixel*(dose));
                }

            }
        }
        else {

                #pragma omp parallel for
                for (int i=0; i<N; i++) {

                    float fProjPixel=pFlat[i]-pFlatBB[i];
                    if (fProjPixel<=0)
                        pFlat[i]=0;
                    else {
                        if (m_bComputeLogarithm)
                            pFlat[i]=log(fProjPixel*dose);
                        else
                            pFlat[i] = fProjPixel*dose;
                    }
                }

        }
    }
    else {
        // throw Exception
        throw ImagingException("Black Body error in PrepareReferencesExtBB",__FILE__,__LINE__);
    }

}

int ReferenceImageCorrection:: ComputeLogNorm(kipl::base::TImage<float,2> &img, float dose)
{
    std::ostringstream msg;
    float Pdose = 0.0f;

    float defaultdose=-log(1.0f/(m_fOpenBeamDose-m_fDarkDose));
    const float transmissionTreshold=0.005f;
//    float defaultdose= 0.0f;


//    float logdose = log(dose<1 ? 1.0f : dose);


    int N=static_cast<int>(img.Size(0)*img.Size(1));

    float *pFlat=m_OpenBeam.GetDataPtr();
    float *pDark=m_DarkCurrent.GetDataPtr();

    std::list<size_t> negPixelList;

    if (m_bHaveBlackBody) {
        if (m_bHaveDarkCurrent) {


            if (m_bHaveOpenBeam) {
    //                #pragma omp parallel for firstprivate(pFlat,pDark)

                        float *pImg=img.GetDataPtr();
                        float *pImgBB = m_BB_sample_Interpolated.GetDataPtr();

                        if(bPBvariante){
                            Pdose = computedose(m_DoseBBsample_image);
                        }

                        #pragma omp parallel for firstprivate(pImgBB, pFlat, pDark)
                        for (int i=0; i<N; i++) {

                            float fProjPixel=(pImg[i]-pDark[i]-pImgBB[i]);
                            if (fProjPixel<=transmissionTreshold){
                                pImg[i] = defaultdose;

                                #pragma omp critical
                                {
                                    negPixelList.push_back(static_cast<size_t>(i));
                                }
                            }
                            else
                                pImg[i]=pFlat[i]-log(fProjPixel)+log((dose-Pdose)<1 ? 1.0f : (dose-Pdose));

                        }

            }
            else {
    //            #pragma omp parallel for firstprivate(pDark)

                  float *pImg=img.GetDataPtr();
                  float *pImgBB = m_BB_sample_Interpolated.GetDataPtr();


                    #pragma omp parallel for firstprivate(pImgBB, pDark)
                    for (int i=0; i<N; i++) {

                        float fProjPixel=(pImg[i]-pDark[i]-pImgBB[i]);// to add dose normalization in pImgBB - done
                        if (fProjPixel<=transmissionTreshold){
//                            pImg[i]=0;
                            pImg[i] = defaultdose;
                            #pragma omp critical
                            {
                                negPixelList.push_back(static_cast<size_t>(i));
                            }
                        }
                        else
                           pImg[i]=-log(fProjPixel)+log((dose-Pdose)<1 ? 1.0f : (dose-Pdose)); // yes but what is logdose if there is no open beam?
                    }

            }
        }
    }
    else if (m_bHaveExternalBlackBody){
        if (m_bHaveDarkCurrent) {


            if (m_bHaveOpenBeam) {
    //                #pragma omp parallel for firstprivate(pFlat,pDark)

                        float *pImg=img.GetDataPtr();
                        float *pImgBB = m_BB_slice_ext.GetDataPtr();




                        #pragma omp parallel for firstprivate (pImgBB,pFlat,pDark)
                        for (int i=0; i<N; i++) {

                            float fProjPixel=(pImg[i]-pDark[i]-pImgBB[i]);
                            if (fProjPixel<=transmissionTreshold){
    //                                pImg[i]=0;
                                pImg[i] = defaultdose;
                                #pragma omp critical
                                {
                                    negPixelList.push_back(static_cast<size_t>(i));
                                }
                            }
                            else
                                pImg[i]=pFlat[i]-log(fProjPixel)+log((dose-fdose_ext_slice)<1 ? 1.0f : (dose-fdose_ext_slice));

                        }

            }
            else {
    //            #pragma omp parallel for firstprivate(pDark)

                  float *pImg=img.GetDataPtr();
                  float *pImgBB = m_BB_slice_ext.GetDataPtr();


                    #pragma omp parallel for firstprivate(pImgBB, pDark)
                    for (int i=0; i<N; i++) {

                        float fProjPixel=(pImg[i]-pDark[i]-pImgBB[i]);
                        if (fProjPixel<=transmissionTreshold) {
//                            pImg[i]=0;
                            pImg[i]= defaultdose;
                            #pragma omp critical
                            {
                                negPixelList.push_back(static_cast<size_t>(i));
                            }
                        }
                        else
                           pImg[i]=-log(fProjPixel)+log((dose-fdose_ext_slice)<1 ? 1.0f : (dose-fdose_ext_slice)); // yes but what is logdose if there is no open beam?
                    }

            }
        }


    }
    else {

        if (m_bHaveDarkCurrent) {
            if (m_bHaveOpenBeam) {

                        float *pImg=img.GetDataPtr();

                        #pragma omp parallel for firstprivate(pFlat,pDark)
                        for (int i=0; i<N; i++) {

                            float fProjPixel=(pImg[i]-pDark[i]);
                            if (fProjPixel<=transmissionTreshold){
//                                pImg[i]=0;
                                pImg[i]= defaultdose;
                                #pragma omp critical
                                {
                                    negPixelList.push_back(static_cast<size_t>(i));
                                }
                            }
                            else
                                pImg[i]=pFlat[i]-log(fProjPixel)+log(dose<=0 ? 1.0f : dose);

                        }

            }
            else {

                  float *pImg=img.GetDataPtr();

                    #pragma omp parallel for firstprivate(pDark)
                    for (int i=0; i<N; i++) {
                        float fProjPixel=(pImg[i]-pDark[i]);
                        if (fProjPixel<=transmissionTreshold){
//                            pImg[i]=0;
                            pImg[i]= defaultdose;
                            #pragma omp critical
                            {
                                negPixelList.push_back(static_cast<size_t>(i));
                            }
                        }
                        else
                           pImg[i]=-log(fProjPixel)+log(dose<=0 ? 1.0f : dose);
                    }

            }
        }
    }

    msg<<"Found "<<negPixelList.size()<<" pixels with negative values.";
    logger.verbose(msg.str());
    kipl::morphology::RepairHoles(img,negPixelList,kipl::base::conn8);
    logger.verbose("Negative values repaired with RepairHoles");

    return 1;
}

void ReferenceImageCorrection::ComputeNorm(kipl::base::TImage<float,2> &img, float dose)
{

//    dose = dose - m_fDarkDose;
    float Pdose = 0.0f;


//    float logdose = log(dose<1 ? 1.0f : dose);

    int N=static_cast<int>(img.Size(0)*img.Size(1));

    float *pFlat=m_OpenBeam.GetDataPtr();
    float *pDark=m_DarkCurrent.GetDataPtr();


    if (m_bHaveBlackBody)
    {
        if (m_bHaveDarkCurrent)
        {
            if (m_bHaveOpenBeam)
            {
//                #pragma omp parallel for firstprivate(pFlat,pDark)

                float *pImg=img.GetDataPtr();
                float *pImgBB = m_BB_sample_Interpolated.GetDataPtr();

                if (bPBvariante)
                {
                    Pdose = computedose(m_DoseBBsample_image);
                } // for now I assume I have all images.. other wise makes no much sense

                #pragma omp parallel for firstprivate(pImgBB, pDark, pFlat)
                for (int i=0; i<N; i++)
                {
                    float fProjPixel=(pImg[i]-pDark[i]-pImgBB[i]);
                    if (fProjPixel<=0)
                        pImg[i]=0;
                    else
                        pImg[i] = fProjPixel/pFlat[i]/((dose-Pdose)<1 ? 1.0f : (dose-Pdose)); // pFlat is already dose corrected

                }

            }
            else {
    //            #pragma omp parallel for firstprivate(pDark)

                  float *pImg=img.GetDataPtr();
                  float *pImgBB = m_BB_sample_Interpolated.GetDataPtr();


                    #pragma omp parallel for firstprivate(pImgBB, pDark)
                    for (int i=0; i<N; i++) {


                        float fProjPixel=(pImg[i]-pDark[i]-pImgBB[i]);// to add dose normalization in pImgBB - done
                        if (fProjPixel<=0)
                            pImg[i]=0;
                        else
                            pImg[i] = fProjPixel/((dose-Pdose)<1 ? 1.0f : (dose-Pdose));
                    }

            }
        }
    }
    else if (m_bHaveExternalBlackBody){
        if (m_bHaveDarkCurrent) {


            if (m_bHaveOpenBeam) {
    //                #pragma omp parallel for firstprivate(pFlat,pDark)

                        float *pImg=img.GetDataPtr();
                        float *pImgBB = m_BB_slice_ext.GetDataPtr();




                        #pragma omp parallel for firstprivate(pImgBB, pFlat, pDark)
                        for (int i=0; i<N; i++) {


                            float fProjPixel=(pImg[i]-pDark[i]-pImgBB[i]); // to add dose normalization in pImgBB - done
                            if (fProjPixel<=0)
                                pImg[i]=0;
                            else
                                pImg[i] = fProjPixel/pFlat[i]/((dose-fdose_ext_slice)<1 ? 1.0f : (dose-fdose_ext_slice));
                        }

            }
            else {
    //            #pragma omp parallel for firstprivate(pDark)

                  float *pImg=img.GetDataPtr();
                  float *pImgBB = m_BB_slice_ext.GetDataPtr();


                    #pragma omp parallel for firstprivate(pImgBB, pDark)
                    for (int i=0; i<N; i++) {


                        float fProjPixel=(pImg[i]-pDark[i]-pImgBB[i]);// to add dose normalization in pImgBB - done
                        if (fProjPixel<=0)
                            pImg[i]=0;
                        else
                           pImg[i] = fProjPixel/((dose-fdose_ext_slice)<1 ? 1.0f : (dose-fdose_ext_slice));
                    }

            }
        }


    }
    else {
        if (m_bHaveDarkCurrent) {
            if (m_bHaveOpenBeam) {


                        float *pImg=img.GetDataPtr();

                        #pragma omp parallel for firstprivate(pFlat,pDark)
                        for (int i=0; i<N; i++) {

                            float fProjPixel=(pImg[i]-pDark[i]);
                            if (fProjPixel<=0)
                                pImg[i]=0;
                            else
                              pImg[i]=fProjPixel/pFlat[i]/(dose<1 ? 1.0f : dose);

                        }

            }
            else {
    //            #pragma omp parallel for firstprivate(pDark)

                  float *pImg=img.GetDataPtr();

                    #pragma omp parallel for firstprivate(pDark)
                    for (int i=0; i<N; i++) {
                        float fProjPixel=(pImg[i]-pDark[i]);
                        if (fProjPixel<=0)
                            pImg[i]=0;
                        else
                           pImg[i]=fProjPixel/(dose<1 ? 1.0f : dose);
                    }

            }
        }
    }
}


float ReferenceImageCorrection::computedose(kipl::base::TImage<float,2> &img){

    float *pImg=img.GetDataPtr();
    float *means=new float[img.Size(1)];
    memset(means,0,img.Size(1)*sizeof(float));

    for (size_t y=0; y<img.Size(1); y++) {
        pImg=img.GetLinePtr(y);

        for (size_t x=0; x<img.Size(0); x++) {
            means[y]+=pImg[x];
        }
        means[y]=means[y]/static_cast<float>(img.Size(0));
    }

    float dose;
    kipl::math::median(means,img.Size(1),&dose);
    delete [] means;
    return dose;

}

void ReferenceImageCorrection::SetExternalBBimages(kipl::base::TImage<float, 2> &bb_ext, kipl::base::TImage<float, 2> &bb_sample_ext, float &dose, float &dose_s)
{
    m_OB_BB_ext.resize(bb_ext.dims());
    m_BB_slice_ext.resize(bb_sample_ext.dims());
    memcpy(m_OB_BB_ext.GetDataPtr(), bb_ext.GetDataPtr(), sizeof(float)*bb_ext.Size());
    memcpy(m_BB_slice_ext.GetDataPtr(), bb_sample_ext.GetDataPtr(), sizeof(float)*bb_sample_ext.Size());
    fdoseOB_ext = dose;
    fdoseS_ext = dose_s;

}

void ReferenceImageCorrection::SetExternalBBimages(kipl::base::TImage<float, 2> &bb_ext, kipl::base::TImage<float, 3> &bb_sample_ext, float &dose, const std::vector<float> & doselist){

    m_OB_BB_ext.resize(bb_ext.dims());
    m_BB_sample_ext.resize(bb_sample_ext.dims());
    memcpy(m_OB_BB_ext.GetDataPtr(), bb_ext.GetDataPtr(), sizeof(float)*bb_ext.Size());
    memcpy(m_BB_sample_ext.GetDataPtr(), bb_sample_ext.GetDataPtr(), sizeof(float)*bb_sample_ext.Size());

    fdoseOB_ext = dose;

    fdose_ext_list = doselist;

}

bool ReferenceImageCorrection::updateStatus(float val, std::string msg)
{
    if (m_Interactor!=nullptr) {
        return m_Interactor->SetProgress(val,msg);
    }

    return false;
}

}





void string2enum(std::string str, ImagingAlgorithms::ReferenceImageCorrection::eReferenceMethod &erm)
{
    std::map<std::string, ImagingAlgorithms::ReferenceImageCorrection::eReferenceMethod> methods;
    methods["LogNorm"] = ImagingAlgorithms::ReferenceImageCorrection::ReferenceLogNorm;
    methods["Norm"] = ImagingAlgorithms::ReferenceImageCorrection::ReferenceNorm; // to be updated with more options

    if (methods.count(str)==0)
        throw ImagingException("The key string does not exist for eReferenceMethod",__FILE__,__LINE__);

    erm=methods[str];


}

std::string enum2string(const ImagingAlgorithms::ReferenceImageCorrection::eReferenceMethod &erm)
{
    std::string str;

    switch (erm) {
        case ImagingAlgorithms::ReferenceImageCorrection::ReferenceLogNorm: str="LogNorm"; break;
        case ImagingAlgorithms::ReferenceImageCorrection::ReferenceNorm: str="Norm"; break;
        default: throw ImagingException("Unknown reference method in enum2string for eReferenceMethod",__FILE__,__LINE__);
    }
    return  str;

}

void  string2enum(std::string str, ImagingAlgorithms::ReferenceImageCorrection::eBBOptions &ebo)
{
    std::map<std::string, ImagingAlgorithms::ReferenceImageCorrection::eBBOptions > bb_options;
    bb_options["noBB"] = ImagingAlgorithms::ReferenceImageCorrection::eBBOptions::noBB;
    bb_options["Interpolate"] = ImagingAlgorithms::ReferenceImageCorrection::eBBOptions::Interpolate;
    bb_options["Average"] = ImagingAlgorithms::ReferenceImageCorrection::eBBOptions::Average;
    bb_options["OneToOne"] = ImagingAlgorithms::ReferenceImageCorrection::eBBOptions::OneToOne;
    bb_options["ExternalBB"] = ImagingAlgorithms::ReferenceImageCorrection::eBBOptions::ExternalBB;

    if (bb_options.count(str)==0)
        throw  ImagingException("The key string does not exist for eBBOptions",__FILE__,__LINE__);

    ebo=bb_options[str];


}

std::string enum2string(const ImagingAlgorithms::ReferenceImageCorrection::eBBOptions &ebo)
{
    std::string str;

    switch (ebo) {
        case ImagingAlgorithms::ReferenceImageCorrection::noBB: str="noBB"; break;
        case ImagingAlgorithms::ReferenceImageCorrection::Interpolate: str="Interpolate"; break;
        case ImagingAlgorithms::ReferenceImageCorrection::Average: str="Average"; break;
        case ImagingAlgorithms::ReferenceImageCorrection::OneToOne: str="OneToOne"; break;
        case ImagingAlgorithms::ReferenceImageCorrection::ExternalBB: str="ExternalBB"; break;
        default                                     	: return "Undefined BB option"; break;
    }
    return  str;

}

std::ostream & operator<<(ostream & s, ImagingAlgorithms::ReferenceImageCorrection::eBBOptions ebo)
{
    s<<enum2string(ebo);

    return s;
}


void  string2enum(std::string str, ImagingAlgorithms::ReferenceImageCorrection::eInterpOrderX &eim_x)
{
    std::map<std::string, ImagingAlgorithms::ReferenceImageCorrection::eInterpOrderX > int_methods;
    int_methods["SecondOrder"] = ImagingAlgorithms::ReferenceImageCorrection::eInterpOrderX::SecondOrder_x;
    int_methods["FirstOrder"] = ImagingAlgorithms::ReferenceImageCorrection::eInterpOrderX::FirstOrder_x;
    int_methods["ZeroOrder"] = ImagingAlgorithms::ReferenceImageCorrection::eInterpOrderX::ZeroOrder_x;

    if (int_methods.count(str)==0)
        throw  ImagingException("The key string does not exist for eInterpMethod",__FILE__,__LINE__);

    eim_x=int_methods[str];


}

std::string enum2string(const ImagingAlgorithms::ReferenceImageCorrection::eInterpOrderX &eim_x)
{
    std::string str;

    switch (eim_x) {
        case ImagingAlgorithms::ReferenceImageCorrection::SecondOrder_x: str="SecondOrder"; break;
        case ImagingAlgorithms::ReferenceImageCorrection::FirstOrder_x: str="FirstOrder"; break;
        case ImagingAlgorithms::ReferenceImageCorrection::ZeroOrder_x: str="ZeroOrder"; break;
        default                                     	: return "Undefined Interpolation method"; break;
    }
    return  str;

}

std::ostream & operator<<(ostream & s, ImagingAlgorithms::ReferenceImageCorrection::eInterpOrderX eim_x)
{
    s<<enum2string(eim_x);

    return s;
}

void  string2enum(std::string str, ImagingAlgorithms::ReferenceImageCorrection::eInterpOrderY &eim_y)
{
    std::map<std::string, ImagingAlgorithms::ReferenceImageCorrection::eInterpOrderY > int_methods;
    int_methods["SecondOrder"] = ImagingAlgorithms::ReferenceImageCorrection::eInterpOrderY::SecondOrder_y;
    int_methods["FirstOrder"] = ImagingAlgorithms::ReferenceImageCorrection::eInterpOrderY::FirstOrder_y;
    int_methods["ZeroOrder"] = ImagingAlgorithms::ReferenceImageCorrection::eInterpOrderY::ZeroOrder_y;

    if (int_methods.count(str)==0)
        throw  ImagingException("The key string does not exist for eInterpMethod",__FILE__,__LINE__);

    eim_y=int_methods[str];


}

std::string enum2string(const ImagingAlgorithms::ReferenceImageCorrection::eInterpOrderY &eim_y)
{
    std::string str;

    switch (eim_y) {
        case ImagingAlgorithms::ReferenceImageCorrection::SecondOrder_y: str="SecondOrder"; break;
        case ImagingAlgorithms::ReferenceImageCorrection::FirstOrder_y: str="FirstOrder"; break;
        case ImagingAlgorithms::ReferenceImageCorrection::ZeroOrder_y: str="ZeroOrder"; break;
        default                                     	: return "Undefined Interpolation method"; break;
    }
    return  str;

}

std::ostream & operator<<(ostream & s, ImagingAlgorithms::ReferenceImageCorrection::eInterpOrderY eim_y)
{
    s<<enum2string(eim_y);

    return s;
}


void  string2enum(std::string str, ImagingAlgorithms::ReferenceImageCorrection::eInterpMethod &eint)
{
    std::map<std::string, ImagingAlgorithms::ReferenceImageCorrection::eInterpMethod > int_types;
    int_types["Polynomial"] = ImagingAlgorithms::ReferenceImageCorrection::eInterpMethod::Polynomial;
    int_types["ThinPlateSplines"] = ImagingAlgorithms::ReferenceImageCorrection::eInterpMethod::ThinPlateSplines;

    if (int_types.count(str)==0)
        throw  ImagingException("The key string does not exist for eInterpMethod",__FILE__,__LINE__);

    eint=int_types[str];


}

std::string enum2string(const ImagingAlgorithms::ReferenceImageCorrection::eInterpMethod &eint)
{
    std::string str;

    switch (eint) {
        case ImagingAlgorithms::ReferenceImageCorrection::Polynomial: str="Polynomial"; break;
        case ImagingAlgorithms::ReferenceImageCorrection::ThinPlateSplines: str="ThinPlateSplines"; break;
        default                                     	: return "Undefined Interpolation method"; break;
    }
    return  str;

}

std::ostream & operator<<(ostream & s, ImagingAlgorithms::ReferenceImageCorrection::eInterpMethod eint)
{
    s<<enum2string(eint);

    return s;
}



void string2enum(const std::string &str, ImagingAlgorithms::ReferenceImageCorrection::eMaskCreationMethod &emask)
{
    std::map<std::string, ImagingAlgorithms::ReferenceImageCorrection::eMaskCreationMethod> strmap;

    strmap["otsumask"]                = ImagingAlgorithms::ReferenceImageCorrection::otsuMask;
    strmap["rosinmask"]               = ImagingAlgorithms::ReferenceImageCorrection::rosinMask;
    strmap["manuallythresholdedmask"] = ImagingAlgorithms::ReferenceImageCorrection::manuallyThresholdedMask;
    strmap["userdefinedmask"]         = ImagingAlgorithms::ReferenceImageCorrection::userDefinedMask;
    strmap["referencefreemask"]       = ImagingAlgorithms::ReferenceImageCorrection::referenceFreeMask;

    try
    {
        emask = strmap.at(str);
    }
    catch (std::out_of_range &e)
    {
        std::ostringstream msg;
        msg<<"Could not convert "<<str<<" to maskEnum\n"<<e.what();
        throw ImagingException(msg.str(),__FILE__,__LINE__);
    }
}

std::string enum2string(const ImagingAlgorithms::ReferenceImageCorrection::eMaskCreationMethod &emask)
{

    switch (emask)
    {
        case ImagingAlgorithms::ReferenceImageCorrection::otsuMask                : return "otsumask";
        case ImagingAlgorithms::ReferenceImageCorrection::rosinMask               : return "rosinmask";
        case ImagingAlgorithms::ReferenceImageCorrection::manuallyThresholdedMask : return "manuallythresholdedmask";
        case ImagingAlgorithms::ReferenceImageCorrection::userDefinedMask         : return "userdefinedmask";
        case ImagingAlgorithms::ReferenceImageCorrection::referenceFreeMask       : return "referencefreemask";
        default : throw ImagingException("Unknown mask enum",__FILE__,__LINE__);
    }

}


std::ostream IMAGINGALGORITHMSSHARED_EXPORT & operator<<(ostream & s, ImagingAlgorithms::ReferenceImageCorrection::eMaskCreationMethod emask)
{
    s<<enum2string(emask);

    return s;

}
