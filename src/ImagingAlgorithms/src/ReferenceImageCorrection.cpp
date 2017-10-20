//<LICENSE>

#include <map>
#include <math.h>
#include <sstream>

#include <tnt_array1d.h>
#include <tnt_array2d.h>
#include <jama_lu.h>
#include <jama_qr.h>
#include <jama_svd.h>
#include <tnt_cmat.h>

#include <math/median.h>
#include <io/io_tiff.h>
#include <segmentation/thresholds.h>
#include <base/thistogram.h>
#include <base/tprofile.h>
#include <base/tsubimage.h>
#include <strings/miscstring.h>

#include "../include/ReferenceImageCorrection.h"
#include "../include/ImagingException.h"

using namespace TNT;

namespace ImagingAlgorithms {

ReferenceImageCorrection::ReferenceImageCorrection() :
    logger("ReferenceImageCorrection"),
    m_bHaveOpenBeam(false),
    m_bHaveDarkCurrent(false),
    m_bHaveBlackBody(false),
    m_bComputeLogarithm(true),
    m_fOpenBeamDose(1.0f),
    m_bHaveDoseROI(false),
    m_bHaveBlackBodyROI(false),
    m_bHaveBBDoseROI(false),
    tau(1.0f),
    radius(0),
    m_AverageMethod(ImagingAlgorithms::AverageImage::ImageWeightedAverage),
    m_IntMeth_x(SecondOrder_x),
    m_IntMeth_y(SecondOrder_y),
    m_nProj(0),
    m_nBBimages(0),
    m_bHaveExternalBlackBody(false),
    fdose_ext_slice(0.0f),
    min_area(0)
{
    m_nDoseROI[0]=0;
    m_nDoseROI[1]=0;
    m_nDoseROI[2]=0;
    m_nDoseROI[3]=0;

    m_nROI[0]=0;
    m_nROI[1]=0;
    m_nROI[2]=0;
    m_nROI[3]=0;

    m_nBlackBodyROI[0]=0;
    m_nBlackBodyROI[1]=0;
    m_nBlackBodyROI[2]=0;
    m_nBlackBodyROI[3]=0;

    m_diffBBroi[0]=0;
    m_diffBBroi[0]=0;
    m_diffBBroi[0]=0;
    m_diffBBroi[0]=0;

    angles[0] = 0.0f;
    angles[1] = 0.0f;
    angles[2] = 0.0f;
    angles[3] = 0.0f;

    a = b = c = d = e = f = 1;

}

ReferenceImageCorrection::~ReferenceImageCorrection()
{

}

//not used
void ReferenceImageCorrection::LoadReferenceImages(std::string path, std::string obname, size_t firstob, size_t obcnt,
        std::string dcname, size_t firstdc, size_t dccnt,
        std::string bbname, size_t firstbb, size_t bbcnt, size_t *roi,
        size_t *doseroi)
{

    std::cout << "ciao process " << std::endl;

}

void ReferenceImageCorrection::SetReferenceImages(kipl::base::TImage<float,2> *ob,
        kipl::base::TImage<float,2> *dc,
        bool useBB, bool useExtBB,
        float dose_OB,
        float dose_DC,
        bool normBB,
        size_t *roi,
        size_t *doseroi)
{

	if (ob!=NULL) {
		m_bHaveOpenBeam=true;
        m_OpenBeam=*ob;
//        kipl::io::WriteTIFF32(m_OpenBeam,"roi_ob.tif");
	}

	if (dc!=NULL) {
		m_bHaveDarkCurrent=true;
        m_DarkCurrent=*dc;
//        kipl::io::WriteTIFF32(m_DarkCurrent,"roi_dc.tif");
	}

    if(doseroi!=nullptr){

        memcpy(m_nDoseROI,doseroi, sizeof(size_t)*4);
    }
    if (roi!=nullptr){
        memcpy(m_nROI,roi, sizeof(size_t)*4);
    }

    if (dose_OB!=0) {
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


    if (useBB) {
		m_bHaveBlackBody=true;

        // these images are used to compute the dose in the PB variante
        // they must be declared here because they are then used in PrepareReferencesBB();

//        m_OB_BB_Interpolated = InterpolateBlackBodyImage(ob_bb_parameters, m_nBlackBodyROI);
//        m_DoseBBflat_image = InterpolateBlackBodyImage(ob_bb_parameters, m_nDoseBBRoi);
//        std::cout << "before switch interpmethod" << std::endl;

        switch (m_InterpMethod) {
        case (Polynomial):{
//            std::cout << "case polynomial" << std::endl;
            m_OB_BB_Interpolated = InterpolateBlackBodyImage(ob_bb_parameters, m_nROI); // now rois are in absolute coordinates , richtig?
            m_DoseBBflat_image = InterpolateBlackBodyImage(ob_bb_parameters, m_nDoseROI);
            break;
        }
        case(ThinPlateSplines): {
//            std::cout << "case splines: " << std::endl;
//            for (std::map<std::pair<int, int>, float>::const_iterator it = spline_values.begin(); it != spline_values.end();  ++it)
//            {
//                std::cout << it->first.first << " " << it->first.second << " " << it->second << std::endl;
//            }

//            std::cout << "number of centroids found: " << spline_values.size() << " "  << std::endl;

            m_OB_BB_Interpolated = InterpolateBlackBodyImagewithSplines(ob_bb_parameters, spline_ob_values, m_nROI); // now rois are in absolute coordinates , richtig?
            m_DoseBBflat_image = InterpolateBlackBodyImagewithSplines(ob_bb_parameters, spline_ob_values, m_nDoseROI);



            break;}
//        default: throw ImagingException("Unknown m_InterpMethod in ReferenceImageCorrection::SetReferenceImages", __FILE__, __LINE__);
        }


//    std::cout   << "before PrepareReferencesBB" << std::endl;
        kipl::io::WriteTIFF32(m_OB_BB_Interpolated,"ob_backgroundimage.tif"); // seem correct
//        kipl::io::WriteTIFF32(m_DoseBBflat_image,"dose_bb.tif");
        PrepareReferencesBB();
	}

    if (useExtBB){
        m_bHaveExternalBlackBody = true;
        PrepareReferencesExtBB();
    }

    if(!useBB && !useExtBB) {
//        std::cout << "noBB option" << std::endl;
        PrepareReferences(); // original way
    }


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
    }

    }


}

kipl::base::TImage<float,2> ReferenceImageCorrection::Process(kipl::base::TImage<float,2> &img, float dose)
{
    if (m_bComputeLogarithm) {
//        kipl::io::WriteTIFF32(img,"prelogimg.tif");
        ComputeLogNorm(img,dose);
//        kipl::io::WriteTIFF32(img,"img.tif");
    }
	else
		ComputeNorm(img,dose);

    return img;

}

void ReferenceImageCorrection::Process(kipl::base::TImage<float,3> &img, float *dose)
{
	kipl::base::TImage<float, 2> slice(img.Dims());


    for (size_t i=0; i<img.Size(2); i++) {

        if (m_bHaveBlackBody) {

            float *current_param;

            switch (m_InterpMethod) {
            case Polynomial:{
                current_param =new float[6];
                memcpy(current_param, sample_bb_interp_parameters+i*6, sizeof(float)*6);
                m_BB_sample_Interpolated = InterpolateBlackBodyImage(current_param ,m_nROI);
                if (i==0)  kipl::io::WriteTIFF32(m_BB_sample_Interpolated,"background_sample.tif");

                m_DoseBBsample_image = InterpolateBlackBodyImage(current_param, m_nDoseROI);
                delete[] current_param;
                break;
            }
            case ThinPlateSplines:{
                current_param =new float[spline_sample_values.size()+3];

                memcpy(current_param, sample_bb_interp_parameters+i*(spline_sample_values.size()+3), sizeof(float)*(spline_sample_values.size()+3));

//                if (i==0){
//                    std::cout << "------DEBUG on CURRENT PARAM ----" << std::endl;
//                    for(int j=0;j<spline_sample_values.size()+3;++j){
//                        std::cout << current_param[j] << " ";
//                    }
//                    std::cout << std::endl;

////                     spline sample  values sembrano giusti..
//                    std::cout << "-----DEBUG on spline_sample_values-------" << std::endl;
//                  for (std::map<std::pair<int, int>, float>::const_iterator it = spline_sample_values.begin(); it != spline_sample_values.end();  ++it)
//                  {
//                      std::cout << it->first.first << " " << it->first.second << " " << it->second << std::endl;
//                  }
//                    std::cout << std::endl;

//                }

                m_BB_sample_Interpolated = InterpolateBlackBodyImagewithSplines(current_param, spline_sample_values, m_nROI);
                m_DoseBBsample_image = InterpolateBlackBodyImagewithSplines(current_param, spline_sample_values, m_nDoseROI);
                if (i==0)  kipl::io::WriteTIFF32(m_BB_sample_Interpolated,"background_sample.tif");
                    delete[] current_param;
                break;
            }
            default: throw ImagingException("Unknown m_InterpMethod in ReferenceImageCorrection::SetReferenceImages", __FILE__, __LINE__);
            }


//            if (i==0)
//            {
//                    kipl::io::WriteTIFF32(m_BB_sample_Interpolated,"samplebb0.tif"); // these are correct.. seems to me..
//                    kipl::io::WriteTIFF32(m_DoseBBsample_image,"dosesamplebb0.tif");
//            }

//            if (i==10)
//            {
//                    kipl::io::WriteTIFF32(m_BB_sample_Interpolated,"samplebb10.tif"); // these are wrong!
//                    kipl::io::WriteTIFF32(m_DoseBBsample_image,"dosesamplebb10.tif");
//            }

//            if (i==img.Size(2)-1)
//            {
//                    kipl::io::WriteTIFF32(m_BB_sample_Interpolated,"last_samplebb.tif");
//            }


        }

        if (m_bHaveExternalBlackBody){

            if (m_BB_sample_ext.Size(2)!=img.Size(2)){
                throw ImagingException ("Number of externally processed BB images are not the same as Projection data",__FILE__,__LINE__);
            }
            m_BB_slice_ext.Resize(m_OB_BB_ext.Dims());
            memcpy(m_BB_slice_ext.GetDataPtr(),m_BB_sample_ext.GetLinePtr(0,i), sizeof(float)*m_BB_sample_ext.Size(0)*m_BB_sample_ext.Size(1));
            fdose_ext_slice = fdose_ext_list[i];
        }

        memcpy(slice.GetDataPtr(),img.GetLinePtr(0,i), sizeof(float)*slice.Size());
//        std::cout << "before Process: " << i << std::endl;
        Process(slice,dose[i]);
//        std::cout << "after process" << std::endl;
        memcpy(img.GetLinePtr(0,i), slice.GetDataPtr(), sizeof(float)*slice.Size()); // and copy the result back
	}


}

void ReferenceImageCorrection::SegmentBlackBody(kipl::base::TImage<float, 2> &img, kipl::base::TImage<float, 2> &mask){

    // 2. otsu threshold
    // 2.a compute histogram


    kipl::base::TImage<float,2> norm(img.Dims());
    memcpy(norm.GetDataPtr(), img.GetDataPtr(), sizeof(float)*img.Size()); // copy to norm.. evalute if necessary

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


    int value = kipl::segmentation::Threshold_Otsu(vec_hist, 256);
    float ot = static_cast<float>(histo.at(value).first);

    //2.c threshold image

    kipl::base::TImage<float,2> maskOtsu(mask.Dims());

 // now it works:
//    kipl::segmentation::Threshold(norm.GetDataPtr(), maskOtsu.GetDataPtr(), norm.Size(), ot);

    float *pImg = norm.GetDataPtr();
    float *res = maskOtsu.GetDataPtr();
    const int N=static_cast<int>(norm.Size());
    for (size_t i=0; i<N; i++){
        if (pImg[i]>=ot){
            res[i] = 1;
        }
        else {
            res[i] = 0;
        }
    }


    // 3. Compute mask within Otsu
    // 3.a sum of rows and columns and location of rois


    float *vert_profile = new float[maskOtsu.Size(1)];
    float *hor_profile = new float[maskOtsu.Size(0)];


    kipl::base::VerticalProjection2D(maskOtsu.GetDataPtr(), maskOtsu.Dims(), vert_profile, true); // sum of rows
    kipl::base::HorizontalProjection2D(maskOtsu.GetDataPtr(), maskOtsu.Dims(), hor_profile, true); // sum of columns


    //3.b create binary Signal
    float *bin_VP = new float[maskOtsu.Size(1)];
    float *bin_HP = new float[maskOtsu.Size(0)];

    for (size_t i=0; i<maskOtsu.Size(1); i++) {
        float max = *std::max_element(vert_profile, vert_profile+maskOtsu.Size(1));
        if(vert_profile[i]<max) {
            bin_VP[i] = 1;
        }
        else {
            bin_VP[i] = 0;
        }

    }


    for (size_t i=0; i<maskOtsu.Size(0); i++) {
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


     for (int i=0; i<maskOtsu.Size(1)-1; i++) {
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


      for (int i=0; i<maskOtsu.Size(0)-1; i++) {
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


//      std::cout << "pos: " << pos << std::endl; // it is the number of potential BBs that I have found:

      for (size_t bb_index=0; bb_index<pos; bb_index++){

          const size_t roi_dim[2] = { size_t(right_edges.at(bb_index).second-left_edges.at(bb_index).second), size_t(right_edges.at(bb_index).first-left_edges.at(bb_index).first)}; // Dx and Dy
          kipl::base::TImage<float,2> roi(roi_dim);
          kipl::base::TImage<float,2> roi_im(roi_dim);


          for (int i=0; i<roi_dim[1]; i++) {
                memcpy(roi.GetLinePtr(i),maskOtsu.GetLinePtr(left_edges.at(bb_index).first+i)+left_edges.at(bb_index).second, sizeof(float)*roi_dim[0]); // one could use tsubimage
                memcpy(roi_im.GetLinePtr(i),norm.GetLinePtr(left_edges.at(bb_index).first+i)+left_edges.at(bb_index).second, sizeof(float)*roi_dim[0]);
          }

          float x_com= 0.0f;
          float y_com= 0.0f;
          int size = 0;

//          // old implementation with geometrical mean:
//          for (size_t x=0; x<roi.Size(0); x++) {
//              for (size_t y=0; y<roi.Size(1); y++) {
//                  if(roi(x,y)==0) {
//                      x_com +=x;
//                      y_com +=y;
//                      size++;
//                  }

//              }
//          }
//          x_com /=size;
//          y_com /=size;

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

                  for (size_t x=0; x<roi.Size(0); x++) {
                      for (size_t y=0; y<roi.Size(1); y++) {
                          if ((sqrt(int(x-x_com+0.5)*int(x-x_com+0.5)+int(y-y_com+0.5)*int(y-y_com+0.5)))<=radius && roi(x,y)==0) {
//                              roi(x,y)=1; // this one actually I don't need
                              mask(x+left_edges.at(bb_index).second, y+left_edges.at(bb_index).first) = 1;
                          }
                      }
                  }

                  roi(int(x_com+0.5), int(y_com+0.5))=2;
          }

      }



//      kipl::io::WriteTIFF32(mask,"/home/carminati_c/repos/testdata/mask.tif");

}

void ReferenceImageCorrection::ComputeBlackBodyCentroids(kipl::base::TImage<float, 2> &img, kipl::base::TImage<float, 2> &mask, std::map<std::pair<int, int>, float> &values)
{
    // 3. Compute mask within Otsu
    // 3.a sum of rows and columns and location of rois
    kipl::base::TImage<float,2> maskOtsu(img.Dims());
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


//    kipl::io::WriteTIFF32(mask, "mask.tif");
//    kipl::io::WriteTIFF32(img, "img.tif"); // they are correct
//    kipl::io::WriteTIFF32(maskOtsu, "maskOtsu.tif");

    kipl::base::VerticalProjection2D(maskOtsu.GetDataPtr(), maskOtsu.Dims(), vert_profile, true); // sum of rows
    kipl::base::HorizontalProjection2D(maskOtsu.GetDataPtr(), maskOtsu.Dims(), hor_profile, true); // sum of columns


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


//            std::cout << "pos: " << pos << std::endl; // it is the number of potential BBs that I have found: that one is correct, error comes later on
//      ostringstream msg;
//      msg.str(""); msg<<"pos= "<<pos;
//      logger(kipl::logging::Logger::LogMessage,msg.str());

      std::map<std::pair<int, int>, float>::const_iterator it_values = values.begin();

      for (size_t bb_index=0; bb_index<pos; ++bb_index){

//          msg.str(""); msg<<"bb_index= "<<bb_index;
//          logger(kipl::logging::Logger::LogMessage,msg.str());

          const size_t roi_dim[2] = { size_t(right_edges.at(bb_index).second-left_edges.at(bb_index).second), size_t(right_edges.at(bb_index).first-left_edges.at(bb_index).first)}; // Dx and Dy
          kipl::base::TImage<float,2> roi(roi_dim);
          kipl::base::TImage<float,2> roi_im(roi_dim);


          for (int i=0; i<roi_dim[1]; i++) {
                memcpy(roi.GetLinePtr(i),maskOtsu.GetLinePtr(left_edges.at(bb_index).first+i)+left_edges.at(bb_index).second, sizeof(float)*roi_dim[0]); // one could use tsubimage
                memcpy(roi_im.GetLinePtr(i),img.GetLinePtr(left_edges.at(bb_index).first+i)+left_edges.at(bb_index).second, sizeof(float)*roi_dim[0]);
          }

          float x_com= 0.0f;
          float y_com= 0.0f;
          int size = 0;

//          // old implementation with geometrical mean:
//          for (size_t x=0; x<roi.Size(0); x++) {
//              for (size_t y=0; y<roi.Size(1); y++) {
//                  if(roi(x,y)==0) {
//                      x_com +=x;
//                      y_com +=y;
//                      size++;
//                  }

//              }
//          }
//          x_com /=size;
//          y_com /=size;

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

//              std::pair<int,int> temp;
//              temp = std::make_pair(floor(x_com+0.5)+left_edges.at(bb_index).second+m_diffBBroi[0], floor(y_com+0.5)+left_edges.at(bb_index).first+m_diffBBroi[1]);

              float value = 0.0f;
              std::vector<float> grayvalues;
//              float mean_value =0.0f;

                  for (size_t x=0; x<roi.Size(0); x++) {
                      for (size_t y=0; y<roi.Size(1); y++) {

                          if (roi(x,y)==0){
//                          if ((sqrt(int(x-x_com+0.5)*int(x-x_com+0.5)+int(y-y_com+0.5)*int(y-y_com+0.5)))<=radius && roi(x,y)==0) { // and also this one is useless
//                              roi(x,y)=1; // this one actually I don't need
//                              mask(x+left_edges.at(bb_index).second, y+left_edges.at(bb_index).first) = 1; // this is now useless
                              value = img(x+left_edges.at(bb_index).second, y+left_edges.at(bb_index).first); // have to compute the median value from it
                              grayvalues.push_back(value);
//                              mean_value += value;

                          }
                      }
                  }


                      roi(int(x_com+0.5), int(y_com+0.5))=2;
                      const auto median_it1 = grayvalues.begin() + grayvalues.size() / 2 - 1;
                      const auto median_it2 = grayvalues.begin() + grayvalues.size() / 2;
                      std::nth_element(grayvalues.begin(), median_it1 , grayvalues.end()); // e1
                      std::nth_element(grayvalues.begin(), median_it2 , grayvalues.end()); // e2
                      float median = (grayvalues.size() % 2 == 0) ? (*median_it1 + *median_it2) / 2 : *median_it2;
    //                  values.insert(std::make_pair(temp,median));
                      std::pair<int,int> mypair;\
                      mypair = std::make_pair(it_values->first.first, it_values->first.second);
                      mypair.first = it_values->first.first;
                      mypair.second = it_values->first.second;
                      values.at(mypair) = median;
//                      logger(kipl::logging::Logger::LogMessage,"before insert");
//                      new_values.insert(std::make_pair(mypair, median));
//                      logger(kipl::logging::Logger::LogMessage,"after insert");
                       ++it_values;

         }

      }

//      return new_values;

//      for (std::map<std::pair<int, int>, float>::const_iterator it = values.begin(); it != values.end();  ++it)
//      {
//          std::cout << it->first.first << " " << it->first.second << " " << it->second << std::endl;
//      }

//      std::cout << "number of centroids found: " << values.size() << " "  << std::endl;

}

void ReferenceImageCorrection::SegmentBlackBody(kipl::base::TImage<float,2> &normimg, kipl::base::TImage<float,2> &img, kipl::base::TImage<float,2> &mask, std::map<std::pair<int, int>, float> &values)
{
    // 2. otsu threshold
    // 2.a compute histogram
    kipl::base::TImage<float,2> norm(normimg.Dims());
    memcpy(norm.GetDataPtr(), normimg.GetDataPtr(), sizeof(float)*img.Size()); // copy to norm.. evalute if necessary

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

    int value = kipl::segmentation::Threshold_Otsu(vec_hist, 256);
    float ot = static_cast<float>(histo.at(value).first);

    //2.c threshold image

    kipl::base::TImage<float,2> maskOtsu(mask.Dims());

 // now it works:
//    kipl::segmentation::Threshold(norm.GetDataPtr(), maskOtsu.GetDataPtr(), norm.Size(), ot);

    float *pImg = norm.GetDataPtr();
    float *res = maskOtsu.GetDataPtr();
    const int N=static_cast<int>(norm.Size());
    for (size_t i=0; i<N; i++){
        if (pImg[i]>=ot){
            res[i] = 1;
        }
        else {
            res[i] = 0;
        }
    }


    // 3. Compute mask within Otsu
    // 3.a sum of rows and columns and location of rois

    float *vert_profile = new float[maskOtsu.Size(1)];
    float *hor_profile = new float[maskOtsu.Size(0)];


    kipl::base::VerticalProjection2D(maskOtsu.GetDataPtr(), maskOtsu.Dims(), vert_profile, true); // sum of rows
    kipl::base::HorizontalProjection2D(maskOtsu.GetDataPtr(), maskOtsu.Dims(), hor_profile, true); // sum of columns


    //3.b create binary Signal
    float *bin_VP = new float[maskOtsu.Size(1)];
    float *bin_HP = new float[maskOtsu.Size(0)];

    for (size_t i=0; i<maskOtsu.Size(1); i++) {
        float max = *std::max_element(vert_profile, vert_profile+maskOtsu.Size(1));
        if(vert_profile[i]<max) {
            bin_VP[i] = 1;
        }
        else {
            bin_VP[i] = 0;
        }

    }


    for (size_t i=0; i<maskOtsu.Size(0); i++) {
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


     for (int i=0; i<maskOtsu.Size(1)-1; i++) {
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


      for (int i=0; i<maskOtsu.Size(0)-1; i++) {
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

          const size_t roi_dim[2] = { size_t(right_edges.at(bb_index).second-left_edges.at(bb_index).second), size_t(right_edges.at(bb_index).first-left_edges.at(bb_index).first)}; // Dx and Dy
          kipl::base::TImage<float,2> roi(roi_dim);
          kipl::base::TImage<float,2> roi_im(roi_dim);


          for (int i=0; i<roi_dim[1]; i++) {
                memcpy(roi.GetLinePtr(i),maskOtsu.GetLinePtr(left_edges.at(bb_index).first+i)+left_edges.at(bb_index).second, sizeof(float)*roi_dim[0]); // one could use tsubimage
                memcpy(roi_im.GetLinePtr(i),norm.GetLinePtr(left_edges.at(bb_index).first+i)+left_edges.at(bb_index).second, sizeof(float)*roi_dim[0]);
          }

          float x_com= 0.0f;
          float y_com= 0.0f;
          int size = 0;

          // old implementation with geometrical mean:
          for (size_t x=0; x<roi.Size(0); x++) {
              for (size_t y=0; y<roi.Size(1); y++) {
                  if(roi(x,y)==0) {
                      x_com +=x;
                      y_com +=y;
                      size++;
                  }

              }
          }
          x_com /=size;
          y_com /=size;

//          float sum_roi = 0.0f;

//          // weighted center of mass:
//          for (size_t x=0; x<roi.Size(0); x++) {
//              for (size_t y=0; y<roi.Size(1); y++) {
//                  if(roi(x,y)==0) {
//                      x_com += (roi_im(x,y)*float(x));
//                      y_com += (roi_im(x,y)*float(y));
//                      sum_roi += roi_im(x,y);
//                      size++;
//                  }

//              }
//          }



          // draw the circle with user-defined radius

          // check on BB dimensions:
          if (size>=min_area) {


//              x_com /=sum_roi;
//              y_com /=sum_roi;

              std::pair<int,int> temp;
              temp = std::make_pair(floor(x_com+0.5)+left_edges.at(bb_index).second+m_diffBBroi[0], floor(y_com+0.5)+left_edges.at(bb_index).first+m_diffBBroi[1]);

              float value = 0.0f;
              std::vector<float> grayvalues;

                  for (size_t x=0; x<roi.Size(0); x++) {
                      for (size_t y=0; y<roi.Size(1); y++) {
                          if ((sqrt(int(x-x_com+0.5)*int(x-x_com+0.5)+int(y-y_com+0.5)*int(y-y_com+0.5)))<=radius && roi(x,y)==0) {
//                              roi(x,y)=1; // this one actually I don't need
                              mask(x+left_edges.at(bb_index).second, y+left_edges.at(bb_index).first) = 1;
                              value = img(x+left_edges.at(bb_index).second, y+left_edges.at(bb_index).first); // have to compute the median value from it
                              grayvalues.push_back(value);

                          }
                      }
                  }

                  roi(int(x_com+0.5), int(y_com+0.5))=2;
                  // mi manca il check sugli outliers!! me misera e me tapina! ----- CONTROLLARE DOMANI DA QUI CHE COSA È SUCC .. NON SI SA ANCORA

                  const auto median_it1 = grayvalues.begin() + grayvalues.size() / 2 - 1;
                  const auto median_it2 = grayvalues.begin() + grayvalues.size() / 2;
                  std::nth_element(grayvalues.begin(), median_it1 , grayvalues.end()); // e1
                  std::nth_element(grayvalues.begin(), median_it2 , grayvalues.end()); // e2
                  float median = (grayvalues.size() % 2 == 0) ? (*median_it1 + *median_it2) / 2 : *median_it2;
                  float average = accumulate( grayvalues.begin(), grayvalues.end(), 0.0/ grayvalues.size());
                  values.insert(std::make_pair(temp,median));
//                  values.insert(std::make_pair(temp,average));

//                  float average = accumulate( grayvalues.begin(), grayvalues.end(), 0.0)/grayvalues.size();
//                  std::vector<double> diff(grayvalues.size());
//                  std::transform(grayvalues.begin(), grayvalues.end(), diff.begin(),
//                                 std::bind2nd(std::minus<double>(), average));
//                  double sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
//                  double stdev = std::sqrt(sq_sum / grayvalues.size());

//                  float mean_value = 0;
//                  float n= 0;

//                  for (std::vector<float>::const_iterator it = grayvalues.begin(); it!= grayvalues.end(); ){
//                      if( *it < (average+2*stdev)) {
//                          mean_value += *it;
//                          n+=1.0f;
//                      }
//                  }

//                  mean_value/=n;


//                  values.insert(std::make_pair(temp,mean_value));
         }

      }

//      // print out the values: // they seem correct!
//      for (std::map<std::pair<int, int>, float>::const_iterator it = values.begin(); it != values.end();  ++it)
//      {
//          std::cout << it->first.first << " " << it->first.second << " " << it->second << std::endl;
//      }

//      std::cout << "number of centroids found: " << values.size() << " "  << std::endl;
}

void ReferenceImageCorrection::SetBBInterpRoi(size_t *roi){

    memcpy(m_nBlackBodyROI,roi,sizeof(size_t)*4);
//    std::cout << "black body roi: " << m_nBlackBodyROI[0] << " " << m_nBlackBodyROI[1] << " " << m_nBlackBodyROI[2] << " "
//                                       << m_nBlackBodyROI[3] << std::endl;

}

void ReferenceImageCorrection::SetBBInterpDoseRoi(size_t *roi){
    memcpy(m_nDoseBBRoi, roi, sizeof(size_t)*4);
}

float* ReferenceImageCorrection::ComputeInterpolationParameters(kipl::base::TImage<float,2>&mask, kipl::base::TImage<float,2>&img){


    std::map<std::pair<int,int>, float> values;


    float mean_value = 0.0f;
    for (int x=0; x<mask.Size(0); x++) {
        for (int y=0; y<mask.Size(1); y++) {
            if (mask(x,y)==1){
                std::pair<int, int> temp;
                temp = std::make_pair(x+m_diffBBroi[0],y+m_diffBBroi[1]);// m_diffBBroi compensates for the relative position of BBroi in the images 
                values.insert(std::make_pair(temp,img(x,y)));
//                  std::cout << " " << x << " " << y << " " <<  BB_DC(x,y) << std::endl;
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


//    std::cout << "MEAN AND STD DEV: " << mean_value << " " << std_dev << std::endl;

//    for (std::map<std::pair<int,int>, float>::const_iterator it = values.begin();
//                it != values.end(); ++it) {

//        if(it->second >= (mean_value+2*std_dev)) {
//            values.erase(it); // not sure it won't cause problems

//        }

//    }

    // this should be the correct way of removing element (STL book, page 205)

    for (std::map<std::pair<int,int>, float>::const_iterator it = values.begin(); it!= values.end(); ){
        if(it->second >= (mean_value+2*std_dev)){
            values.erase(it++);
        }
        else{
            ++it;
        }
    }

    float *myparam = new float[6];
    float error;
    myparam = SolveLinearEquation(values, error);

    return myparam;

}

float* ReferenceImageCorrection::ComputeInterpolationParameters(kipl::base::TImage<float,2>&mask, kipl::base::TImage<float,2>&img, float &error){

    // IMPORTANT! WHEN THE A SPECIFIC ROI IS SELECTED FOR THE BBs, THEN THE INTERPOLATION PARAMETERS NEED TO BE COMPUTED ON THE BIGGER IMAGE GRID
    std::map<std::pair<int,int>, float> values;


    // find values to interpolate

//    kipl::io::WriteTIFF32(mask, "mask.tif");

    float mean_value = 0.0f;
    for (int x=0; x<mask.Size(0); x++) {
        for (int y=0; y<mask.Size(1); y++) {
            if (mask(x,y)==1){
                std::pair<int,int> temp;
                temp = std::make_pair(x+m_diffBBroi[0],y+m_diffBBroi[1]);// m_diffBBroi compensates for the relative position of BBroi in the images. now it should be in absolute coordinates
                values.insert(std::make_pair(temp,img(x,y)));
//                  std::cout << " " << x << " " << y << " " <<  img(x,y) << std::endl;
//                  std::cout << m_diffBBroi[0] << " " << m_diffBBroi[1] << std::endl;
//                  std::cout << " " << x+m_diffBBroi[0] << " " << y+m_diffBBroi[1] << " " <<  img(x,y) << std::endl;
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

int outlier = 0;
    for (std::map<std::pair<int,int>, float>::const_iterator it = values.begin();
                it != values.end(); ++it) {

        if(it->second >= (mean_value+2*std_dev)) {
//            std::cout << "found outlier value" << std::endl;
//            std::cout << it->first.first << " " << it->first.second << " " << it->second << std::endl;
            values.erase(it); // not sure it won't cause problems
            outlier++;

        }

    }



    float *myparam = new float[6];
//    float error;
    float my_error = 0.0f;
    myparam = SolveLinearEquation(values, my_error);
    error = my_error;

//    std::cout << "error in compute interpolation parameters: " << error << std::endl;

         return myparam;

}

float * ReferenceImageCorrection::SolveLinearEquation(std::map<std::pair<int, int>, float> &values, float &error){


    int matrix_size = 1 + ((d && a)+b+c+f+e);
//    std::cout << "matrix_size: " << matrix_size << std::endl;

    Array2D< double > my_matrix(values.size(),matrix_size, 0.0);
    Array1D< double > I(values.size(), 0.0);
    Array1D< double > param(matrix_size, 0.0);
    std::map<std::pair<int, int>, float>::const_iterator it = values.begin();

//    std::cout << "TNT STUFF: " << std::endl;
//    std::cout << my_matrix.dim1() << " " << my_matrix.dim2() << std::endl;
    for (int x=0; x<my_matrix.dim1(); x++) {

        if (a && b && c && d && e && f) { // second order X, second order Y
              my_matrix[x][0] = 1;
              my_matrix[x][1] = it->first.first;
              my_matrix[x][2] = it->first.first*it->first.first;
              my_matrix[x][3] = it->first.first*it->first.second;
              my_matrix[x][4] = it->first.second;
              my_matrix[x][5] = it->first.second*it->first.second;
        }
        if ((b && d && a && c) && (e==0) && (f==0)){ // first order X, first order Y
            my_matrix[x][0] = 1;
            my_matrix[x][1] = it->first.first;
            my_matrix[x][2] = it->first.second;
            my_matrix[x][3] = it->first.first*it->first.second;

        }
        if (b==0 && a==0 && c==0 && d==0 && e==0 && f==0) { // zero order X, zero order Y
            my_matrix[x][0] = 1;
        }

        if ((b && d && e && c && a) && f==0 ) { // second order X, first order Y
            my_matrix[x][0] = 1;
            my_matrix[x][1] = it->first.first;
            my_matrix[x][2] = it->first.first*it->first.first;
            my_matrix[x][3] = it->first.first*it->first.second;
            my_matrix[x][4] = it->first.second;
        }
        if ((b && d && e) && c==0 && a==0 && f==0 ) { // second order X, zero order Y
            my_matrix[x][0] = 1;
            my_matrix[x][1] = it->first.first;
            my_matrix[x][2] = it->first.first*it->first.first;
        }

        if ((b && c && d && a && f) && e==0){ // first order X, second order Y
            my_matrix[x][0] = 1;
            my_matrix[x][1] = it->first.first;
            my_matrix[x][2] = it->first.first*it->first.second;
            my_matrix[x][3] = it->first.second;
            my_matrix[x][4] = it->first.second*it->first.second;
        }

        if( (b && d) && a==0 && c==0 &&  e==0 && f==0){ //first order X, zero order Y
            my_matrix[x][0] = 1;
            my_matrix[x][1] = it->first.first;
        }

        if (b==0 && d==0 && e==0 && (c && a && f)){ // zero order X, second order Y
            my_matrix[x][0] = 1;
//            my_matrix[x][1] = it->first.first;
//            my_matrix[x][2] = it->first.first*it->first.first;
//            my_matrix[x][3] = it->first.first*it->first.second;
            my_matrix[x][1] = it->first.second;
            my_matrix[x][2] = it->first.second*it->first.second;
        }

        if ((c && a) && b==0 && d==0 && e==0 && f==0){ // zero order X, first order Y
            my_matrix[x][0] = 1;
            my_matrix[x][1] = it->first.second;

        }

              I[x] = static_cast<double>(it->second);
//                          std::cout << it->first.first << " " << it->first.second << " " << it->second << "\n";

              it++;

    }

    JAMA::QR<double> qr(my_matrix);
    param = qr.solve(I);



//    for (int i=0; i<matrix_size; i++) {
//    std::cout << param[i] <<  " ";
//    }
//    std::cout << std::endl;



    float *myparam = new float[6];
//    memcpy(myparam, float(param), sizeof(float)*6); // problems with casting..

    for (int i=0; i<6; i++) { // initialize them to zero:
        myparam[i] = 0.0f;
    }




    Array1D< double > I_int(values.size(), 0.0);

    // here my param must be in the same order...


    it = values.begin();
  for (int i=0; i<values.size(); i++){

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


//    std::cout << "error in solve linear equation " << error << std::endl;

//    std::cout << myparam[0] << " " << myparam[1] << " " << myparam[2] << " " << myparam[3] << " " << myparam[4] << " " << myparam[5] << std::endl;


    return myparam;


}

float * ReferenceImageCorrection::SolveThinPlateSplines(std::map<std::pair<int,int>,float> &values) {



    //1. create system matrix, dimension of the matrix depends on the number of BB found
    float *param = new float[values.size()+3];

    Array2D< double > my_matrix(values.size()+3,values.size()+3, 0.0);
    Array1D< double > b_vector(values.size()+3, 0.0);
    Array1D< double > qr_param(values.size()+3, 0.0);


    std::map<std::pair<int,int>, float>::const_iterator it_i = values.begin();
    for (int i=0; i<values.size(); ++i) {
            std::map<std::pair<int,int>, float>::const_iterator it_j = values.begin();
        for (int j=0; j<values.size(); ++j) {

            if (i==j){
                my_matrix[i][j] =  0.0f;
            }
            else {
                float ii_1 = static_cast<float>(it_i->first.first);
                float ii_2 = static_cast<float>(it_i->first.second);
                float jj_1 = static_cast<float>(it_j->first.first);
                float jj_2 = static_cast<float>(it_j->first.second);
                float dist = sqrt((ii_1-jj_1)*(ii_1-jj_1)+(ii_2-jj_2)*(ii_2-jj_2));
  //              float dist = sqrt((it_i->first.first-it_j->first.first)*(it_i->first.first-it_j->first.first)+(it_i->first.second-it_j->first.second)*(it_i->first.second-it_j->first.second));
//                float dist = sqrt((values.at(i).first.first-values.at(j).first.first)*(values.at(i).first.first-values.at(j).first.first)+(values.at(i).first.second-values.at(j).first.second)*(values.at(i).first.second-values.at(j).first.second)); // does not work like this...
                my_matrix[i][j] = dist*dist*log(dist);

            }
            ++it_j;


        }

        ++it_i;
    }

    it_i = values.begin();
    for (int i=0; i<values.size(); ++i){
        my_matrix[i][values.size()] = 1.0;
        my_matrix[i][values.size()+1] = it_i->first.second; // y
        my_matrix[i][values.size()+2] = it_i->first.first; // x
        ++it_i;
    }

    it_i = values.begin();
    for (int i=0; i<values.size(); ++i){
        my_matrix[values.size()][i] = 1.0;
        my_matrix[values.size()+1][i] = it_i->first.second; // y
        my_matrix[values.size()+2][i] = it_i->first.first; // x
        ++it_i;
    }

    int i=0;
    for (it_i = values.begin(); it_i!=values.end(); ++it_i){
        b_vector[i] = it_i->second;
        ++i;
    }

//    JAMA::QR<double> qr(my_matrix);
//    qr_param = my_matrix.solve(b_vector); // crashed with QR

    JAMA::SVD<double> svd(my_matrix);
    Array2D< double > U(values.size()+3,values.size()+3, 0.0);
    Array2D< double > V(values.size()+3,values.size()+3, 0.0);
    Array1D< double > S(values.size()+3, 0.0);

    svd.getU(U);
    svd.getV(V);
    svd.getSingularValues(S);

    TNT::Array2D<double> Utran(U.dim1(), U.dim2());
    for(int r=0; r<U.dim1(); ++r)
        for(int c=0; c<U.dim2(); ++c)
            Utran[c][r] = U[r][c];

    TNT::Array2D<double> Si(U.dim1(), U.dim2(), 0.0);
    for (int i=0; i<Si.dim1(); ++i)
        Si[i][i] = 1/S[i];

    TNT::Array2D<double> inv_matrix(my_matrix.dim1(), my_matrix.dim2());
    inv_matrix =  matmult(matmult(V,Si),Utran);


    for (int i=0;i<my_matrix.dim1();i++){
         for (int j=0;j<my_matrix.dim2();j++){
             qr_param[i]+=( inv_matrix[i][j]*b_vector[j]);
         }
     }

    for (i=0; i<values.size()+3; ++i){
        param[i] = static_cast<float>(qr_param[i]);
    }


    return param;

}

kipl::base::TImage<float,2> ReferenceImageCorrection::InterpolateBlackBodyImagewithSplines(float *parameters, std::map<std::pair<int, int>, float> &values, size_t *roi){


    size_t dimx = roi[2]-roi[0];
    size_t dimy = roi[3]-roi[1];


    size_t dims[2] = {dimx,dimy};
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
                dist[ind] = sqrt((it_i->first.first-(x+roi[0]))*(it_i->first.first-(x+roi[0]))+(it_i->first.second-(y+roi[1]))*(it_i->first.second-(y+roi[1])));
                if (dist[ind]!=0){
                    sumdist += (dist[ind]*dist[ind]*log(dist[ind])*parameters[ind]); // correct?                }
                }

                ++it_i;
            }

                delete [] dist;

            interpolated_img(x,y) = sumdist+parameters[values.size()]+parameters[values.size()+1]*(y+roi[1])+parameters[values.size()+2]*(x+roi[0]);
        }
    }

    return interpolated_img;




}

kipl::base::TImage<float,2> ReferenceImageCorrection::InterpolateBlackBodyImage(float *parameters, size_t *roi) {


    size_t dimx = roi[2]-roi[0];
    size_t dimy = roi[3]-roi[1];

    size_t dims[2] = {dimx,dimy};
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

float * ReferenceImageCorrection::PrepareBlackBodyImage(kipl::base::TImage<float, 2> &flat, kipl::base::TImage<float, 2> &dark, kipl::base::TImage<float, 2> &bb, kipl::base::TImage<float,2> &mask)
{

    // 1. normalize image

    kipl::base::TImage<float, 2> norm(bb.Dims());
    kipl::base::TImage<float, 2> normdc(bb.Dims());
    memcpy(norm.GetDataPtr(),bb.GetDataPtr(), sizeof(float)*bb.Size());
    memcpy(normdc.GetDataPtr(),bb.GetDataPtr(), sizeof(float)*bb.Size());

    normdc -=dark;

    norm -=dark;
    norm /= (flat-=dark);

    std::map<std::pair<int, int>, float> values;
//    kipl::io::WriteTIFF32(norm,"normBB.tif");
//    kipl::io::WriteTIFF32(bb,"BB.tif");
//    kipl::io::WriteTIFF32(flat,"openBB.tif");

    try{
        SegmentBlackBody(norm, mask);
//        SegmentBlackBody(norm, normdc, mask, values); // try for thin plates spline
    }
    catch (...) {
        throw ImagingException("SegmentBlackBodyNorm failed", __FILE__, __LINE__);
    }

//    kipl::io::WriteTIFF32(mask,"mask.tif");

    kipl::base::TImage<float,2> BB_DC(bb.Dims());
    memcpy(BB_DC.GetDataPtr(), bb.GetDataPtr(), sizeof(float)*bb.Size());
    BB_DC-=dark;

    kipl::base::TImage<float, 2> interpolated_BB(bb.Dims());
    interpolated_BB = 0.0f;

    float * param = new float[6];
    param = ComputeInterpolationParameters(mask,BB_DC);

    return param;

}

float *ReferenceImageCorrection::PrepareBlackBodyImagewithSplines(kipl::base::TImage<float, 2> &flat, kipl::base::TImage<float, 2> &dark, kipl::base::TImage<float, 2> &bb, kipl::base::TImage<float, 2> &mask, std::map<std::pair<int, int>, float> &values)
{
    // 1. normalize image

    kipl::base::TImage<float, 2> norm(bb.Dims());
    kipl::base::TImage<float, 2> normdc(bb.Dims());
    memcpy(norm.GetDataPtr(),bb.GetDataPtr(), sizeof(float)*bb.Size());
    memcpy(normdc.GetDataPtr(),bb.GetDataPtr(), sizeof(float)*bb.Size());

    normdc -=dark;
    norm -=dark;
    norm /= (flat-=dark);

    SegmentBlackBody(norm, normdc, mask, values);
    float *tps_param = new float[values.size()+3];

    tps_param = SolveThinPlateSplines(values);

    return tps_param;

}

float *ReferenceImageCorrection::PrepareBlackBodyImagewithSplinesAndMask(kipl::base::TImage<float, 2> &dark, kipl::base::TImage<float, 2> &bb, kipl::base::TImage<float, 2> &mask, std::map<std::pair<int, int>, float> &values)
{

    kipl::base::TImage<float,2> BB_DC(bb.Dims());
    memcpy(BB_DC.GetDataPtr(), bb.GetDataPtr(), sizeof(float)*bb.Size());
    BB_DC-=dark;

    ComputeBlackBodyCentroids(BB_DC,mask, values);

    float *tps_param = new float[values.size()+3];
    tps_param = SolveThinPlateSplines(values);

    return tps_param;
}

float * ReferenceImageCorrection::PrepareBlackBodyImage(kipl::base::TImage<float, 2> &flat, kipl::base::TImage<float, 2> &dark, kipl::base::TImage<float, 2> &bb, kipl::base::TImage<float,2> &mask, float &error)
{


    // 1. normalize image

    kipl::base::TImage<float, 2> norm(bb.Dims());
    kipl::base::TImage<float, 2> normdc(bb.Dims());
    memcpy(norm.GetDataPtr(),bb.GetDataPtr(), sizeof(float)*bb.Size());
    memcpy(normdc.GetDataPtr(),bb.GetDataPtr(), sizeof(float)*bb.Size());

    normdc -=dark;
    norm -=dark;
    norm /= (flat-=dark);

    SegmentBlackBody(norm, mask);

    kipl::base::TImage<float,2> BB_DC(bb.Dims());
    memcpy(BB_DC.GetDataPtr(), bb.GetDataPtr(), sizeof(float)*bb.Size());
    BB_DC-=dark;

    kipl::base::TImage<float, 2> interpolated_BB(bb.Dims());
    interpolated_BB = 0.0f;

    float * param = new float[6];
    float myerror;

    param = ComputeInterpolationParameters(mask,BB_DC,myerror);
    error = myerror;


    return param;

}

float * ReferenceImageCorrection::PrepareBlackBodyImagewithMask(kipl::base::TImage<float, 2> &dark, kipl::base::TImage<float, 2> &bb, kipl::base::TImage<float, 2> &mask){

    kipl::base::TImage<float,2> BB_DC(bb.Dims());
    memcpy(BB_DC.GetDataPtr(), bb.GetDataPtr(), sizeof(float)*bb.Size());
    BB_DC-=dark;

//    kipl::base::TImage<float, 2> interpolated_BB(bb.Dims());
//    interpolated_BB = 0.0f;

    float * param = new float[6];
    param = ComputeInterpolationParameters(mask,BB_DC);

    return param;

}

void ReferenceImageCorrection::SetAngles(float *ang, size_t nProj, size_t nBB){
    angles[0] = ang[0];
    angles[1] = ang[1];
    angles[2] = ang[2];
    angles[3] = ang[3];

    m_nProj = nProj; // to set eventually around where it is used
    m_nBBimages = nBB;


}

void ReferenceImageCorrection::SetDoseList(float *doselist){

    if (m_nProj!=0) {
        dosesamplelist = new float[m_nProj];
        memcpy(dosesamplelist, doselist, sizeof(float)*m_nProj);
    }
    else {
        throw ImagingException("m_nProj was not set before calling ReferenceImageCorrection::SetDoseList",__FILE__,__LINE__);
    }

}

void ReferenceImageCorrection::SetSplinesParameters(float *ob_parameter, float *sample_parameter, size_t nBBSampleCount, size_t nProj, eBBOptions ebo, int nBBs)
{
    m_nProj = nProj; // not very smartly defined
    ob_bb_parameters = new float[nBBs+3];
    sample_bb_interp_parameters = new float[(nBBs+3)*m_nProj];

    m_nBBimages = nBBSampleCount; // store how many images with BB and sample i do have
    memcpy(ob_bb_parameters, ob_parameter, sizeof(float)*(nBBs+3));

    switch(ebo) {
    case(Interpolate) : {

        sample_bb_parameters = new float[(nBBs+3)*m_nBBimages];
        memcpy(sample_bb_parameters, sample_parameter, sizeof(float)*(nBBs+3)*m_nBBimages);
        sample_bb_interp_parameters = InterpolateSplineGeneric(sample_bb_parameters, nBBs);


        for (size_t i=0; i<m_nProj; i++){
            for (size_t j=0; j<(nBBs+3); j++){
                sample_bb_interp_parameters[j+i*(nBBs+3)] *= (dosesamplelist[i]/tau);
            }
        }

        break;
    }
    case(OneToOne) : {

        if (m_nBBimages!=m_nProj){
            throw ImagingException("The number of BB images is not the same as the number of Projection images",__FILE__, __LINE__);
        }
        memcpy(sample_bb_interp_parameters, sample_parameter, sizeof(float)*(nBBs+3)*m_nProj);

        break;
    }
    case(Average) : {

        sample_bb_parameters = new float[nBBs+3];
        memcpy(sample_bb_parameters, sample_parameter, sizeof(float)*(nBBs+3));
        sample_bb_interp_parameters = ReplicateSplineParameters(sample_bb_parameters, nProj, nBBs);
        break;
    }
    }
}


void ReferenceImageCorrection::SetInterpParameters(float *ob_parameter, float *sample_parameter, size_t nBBSampleCount, size_t nProj, eBBOptions ebo)
{

    m_nProj = nProj; // not very smartly defined
    ob_bb_parameters = new float[6];
    sample_bb_interp_parameters = new float[6*m_nProj];

    m_nBBimages = nBBSampleCount; // store how many images with BB and sample i do have
    memcpy(ob_bb_parameters, ob_parameter, sizeof(float)*6);

    switch(ebo) {
    case(Interpolate) : {

        sample_bb_parameters = new float[6*m_nBBimages];
        memcpy(sample_bb_parameters, sample_parameter, sizeof(float)*6*m_nBBimages);
        sample_bb_interp_parameters = InterpolateParametersGeneric(sample_bb_parameters);

        for (size_t i=0; i<m_nProj; i++){
            for (size_t j=0; j<6; j++){
                sample_bb_interp_parameters[j+i*6] *= (dosesamplelist[i]/tau);
            }
        }

        break;
    }
    case(OneToOne) : {
//        std::cout << "OneToOne.... " << std::endl;
//        std::cout << "copying parameters" << std::endl;

        if (m_nBBimages!=m_nProj){
            throw ImagingException("The number of BB images is not the same as the number of Projection images",__FILE__, __LINE__);
        }
        memcpy(sample_bb_interp_parameters, sample_parameter, sizeof(float)*6*m_nProj);

        break;
    }
    case(Average) : {

        sample_bb_parameters = new float[6];
        memcpy(sample_bb_parameters, sample_parameter, sizeof(float)*6);
        sample_bb_interp_parameters = ReplicateParameters(sample_bb_parameters, nProj);
        break;
    }
    }


}

float* ReferenceImageCorrection::InterpolateSplineGeneric(float *param, int nBBs)
{
    // it is assumed that angles are in order
    float *interpolated_param = new float[(nBBs+3)*m_nProj];
    float curr_angle;
    float *small_a;
    float *big_a;
    float *bb_angles = new float[m_nBBimages];
    float step;
    int index_1;
    int index_2;


    for (size_t i=0; i<m_nBBimages; i++){
        bb_angles[i]= angles[2]+(angles[3]-angles[2])/(m_nBBimages-1)*i; // this is now more generic starting at angle angles[2]
    }


    for (size_t i=0; i<m_nProj; i++){
    //1. compute current angle for projection

        curr_angle = angles[0]+(angles[1]-angles[0])/(m_nProj-1)*i;


    //2. find two closest projection in BB data
        if (i==0){
            small_a = &bb_angles[0];
            big_a = &bb_angles[1];
            index_1=0;
            index_2=(nBBs+3);
        }

        if(curr_angle<*small_a) {
            step = (curr_angle-0.0f)/(*small_a-0.0f); // case in which the BB angles do not start from zero
        }
        else {
            step = (curr_angle-*small_a)/(*big_a-*small_a);
        }


        float *temp_param = new float[nBBs+3];

        for (size_t k=0; k<(nBBs+3);k++){
            temp_param[k] = param[index_1+k]+(step)*(param[index_2+k]-param[index_1+k]);
        }

        memcpy(interpolated_param+i*(nBBs+3),temp_param,sizeof(float)*(nBBs+3));
        delete[] temp_param;




        if (curr_angle>=*big_a){ // se esco dal range incremento di 1



            if(curr_angle>=bb_angles[m_nBBimages-1]) {

                if (*big_a==bb_angles[m_nBBimages-1]) { // if pointers have not been modified yes, change the index, otherwise keep them as they are
                    index_1=index_2;
                    index_2 =0;
                }

                small_a = &bb_angles[m_nBBimages-1];
                big_a = &bb_angles[0];
                step = (curr_angle-*small_a)/(360.0f-*small_a);



            }
            else {

                index_1=index_2;
                index_2 +=(nBBs+3);

                small_a++;
                big_a++;
            }
        }




    }

    delete [] bb_angles;

    return interpolated_param;
}

float* ReferenceImageCorrection::InterpolateParametersGeneric(float* param){

    // it is assumed that angles are in order
    float *interpolated_param = new float[6*m_nProj];
    float curr_angle;
    float *small_a;
    float *big_a;
    float *bb_angles = new float[m_nBBimages];
    float step;
    int index_1;
    int index_2;


    for (size_t i=0; i<m_nBBimages; i++){
        bb_angles[i]= angles[2]+(angles[3]-angles[2])/(m_nBBimages-1)*i; // this is now more generic starting at angle angles[2]
    }


    for (size_t i=0; i<m_nProj; i++){
    //1. compute current angle for projection

        curr_angle = angles[0]+(angles[1]-angles[0])/(m_nProj-1)*i;


    //2. find two closest projection in BB data
        if (i==0){
            small_a = &bb_angles[0];
            big_a = &bb_angles[1];
            index_1=0;
            index_2=6;
        }

        if(curr_angle<*small_a) {
            step = (curr_angle-0.0f)/(*small_a-0.0f); // case in which the BB angles do not start from zero
        }
        else {
            step = (curr_angle-*small_a)/(*big_a-*small_a);
        }


        float *temp_param = new float[6];

        for (size_t k=0; k<6;k++){
            temp_param[k] = param[index_1+k]+(step)*(param[index_2+k]-param[index_1+k]);
        }

        memcpy(interpolated_param+i*6,temp_param,sizeof(float)*6);
        delete[] temp_param;




        if (curr_angle>=*big_a){ // se esco dal range incremento di 1



            if(curr_angle>=bb_angles[m_nBBimages-1]) {

                if (*big_a==bb_angles[m_nBBimages-1]) { // if pointers have not been modified yes, change the index, otherwise keep them as they are
                    index_1=index_2;
                    index_2 =0;
                }

                small_a = &bb_angles[m_nBBimages-1];
                big_a = &bb_angles[0];
                step = (curr_angle-*small_a)/(360.0f-*small_a);



            }
            else {

                index_1=index_2;
                index_2 +=6;

                small_a++;
                big_a++;
            }
        }




    }

    delete [] bb_angles;

    return interpolated_param;


}

float* ReferenceImageCorrection::InterpolateParameters(float *param, size_t n, size_t step){

    float *interpolated_param = new float[6*(n+1)];
//    float *int_param = new float[6*n];

   // first copy the one that i already have
    size_t index = 0;
    for (size_t i=0; i<6*n; i+=step*6){
        memcpy(interpolated_param+i, param+index, sizeof(float)*6);
        index +=6;

    }

    memcpy(interpolated_param+6*n, param, sizeof(float)*6); // copio i primi in coda


    for (size_t i=0; i<6*n; i+=step*6){
            index=i;

        for (size_t j=1; j<step; j++){
            float *temp_param = new float[6];

            for (size_t k=0; k<6; k++) {
                   temp_param[k] = interpolated_param[i+k]+(j)*(interpolated_param[i+k+step*6]-interpolated_param[i+k])/(step);
            }

            index +=6;
            memcpy(interpolated_param+index,temp_param,sizeof(float)*6);
            delete[] temp_param;


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
    int* target = (int*)malloc(sizeof(int)*count * expand);
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
//    std::cout << "dose: " << dose << std::endl;

//    if (dose!=dose)
//        throw ReconException("The reference dose is a NaN",__FILE__,__LINE__);

    const int N=static_cast<int>(m_OpenBeam.Size());
    float *pFlat=m_OpenBeam.GetDataPtr();
    float *pDark=m_DarkCurrent.GetDataPtr();

//            kipl::io::WriteTIFF32(m_OpenBeam,"prepare_ob.tif");
//            kipl::io::WriteTIFF32(m_DarkCurrent,"prepare_dc.tif");


    if (!m_bHaveBlackBody) {

            if (m_bHaveDarkCurrent) {
                #pragma omp parallel for
                for (int i=0; i<N; i++) {
                    float fProjPixel=pFlat[i]-pDark[i];
                    if (fProjPixel<=0)
                        pFlat[i]=0;
                    else {
                        if (m_bComputeLogarithm) {
                            pFlat[i]=log(fProjPixel)+log(dose);
                        }
                        else
                            pFlat[i]=fProjPixel*dose;
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
                            pFlat[i]=log(fProjPixel*dose);
                        else
                            pFlat[i] = fProjPixel*dose;
                        }
                }
            }
            }

//    kipl::io::WriteTIFF32(m_OpenBeam,"log_ob.tif");

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
                        pFlat[i]=log(fProjPixel*(dose));
                    else
                        pFlat[i]=fProjPixel*(dose);
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
                             pFlat[i]=(fProjPixel*dose);
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

    float Pdose = 0.0f;

    float defaultdose=-log(1.0f/(m_fOpenBeamDose-m_fDarkDose));
//    float defaultdose= 0.0f;


//    float logdose = log(dose<1 ? 1.0f : dose);


    int N=static_cast<int>(img.Size(0)*img.Size(1));

    float *pFlat=m_OpenBeam.GetDataPtr();
    float *pDark=m_DarkCurrent.GetDataPtr();


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
                            if (fProjPixel<=0){
                                pImg[i] = defaultdose;
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
                        if (fProjPixel<=0){
//                            pImg[i]=0;
                            pImg[i] = defaultdose;
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
                            if (fProjPixel<=0){
    //                                pImg[i]=0;
                                pImg[i] = defaultdose;
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
                        if (fProjPixel<=0) {
//                            pImg[i]=0;
                            pImg[i]= defaultdose;
                        }
                        else
                           pImg[i]=-log(fProjPixel)+log((dose-fdose_ext_slice)<1 ? 1.0f : (dose-fdose_ext_slice)); // yes but what is logdose if there is no open beam?
                    }

            }
        }


    }
    else {
//        std::cout << "noBB option" << std::endl;
        if (m_bHaveDarkCurrent) {
            if (m_bHaveOpenBeam) {
    //                #pragma omp parallel for firstprivate(pFlat,pDark)

//                 kipl::io::WriteTIFF32(img,"img_beforenorm.tif");

                        float *pImg=img.GetDataPtr();

                        #pragma omp parallel for firstprivate(pFlat,pDark)
                        for (int i=0; i<N; i++) {

                            float fProjPixel=(pImg[i]-pDark[i]);
                            if (fProjPixel<=0){
//                                pImg[i]=0;
                                pImg[i]= defaultdose;
                            }
                            else
                                pImg[i]=pFlat[i]-log(fProjPixel)+log(dose<=0 ? 1.0f : dose);

                        }

            }
            else {
    //            #pragma omp parallel for firstprivate(pDark)

                  float *pImg=img.GetDataPtr();

                    #pragma omp parallel for firstprivate(pDark)
                    for (int i=0; i<N; i++) {
                        float fProjPixel=(pImg[i]-pDark[i]);
                        if (fProjPixel<=0){
//                            pImg[i]=0;
                            pImg[i]= defaultdose;
                        }
                        else
                           pImg[i]=-log(fProjPixel)+log(dose<=0 ? 1.0f : dose);
                    }

            }
        }
    }



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


    if (m_bHaveBlackBody) {
        if (m_bHaveDarkCurrent) {


            if (m_bHaveOpenBeam) {
    //                #pragma omp parallel for firstprivate(pFlat,pDark)

                        float *pImg=img.GetDataPtr();
                        float *pImgBB = m_BB_sample_Interpolated.GetDataPtr();

                        if(bPBvariante){
                            Pdose = computedose(m_DoseBBsample_image);
                        } // for now I assume I have all images.. other wise makes no much sense





                        #pragma omp parallel for firstprivate(pImgBB, pDark, pFlat)
                        for (int i=0; i<N; i++) {


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
    //                #pragma omp parallel for firstprivate(pFlat,pDark)

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

void ReferenceImageCorrection::SetExternalBBimages(kipl::base::TImage<float, 2> &bb_ext, kipl::base::TImage<float, 3> &bb_sample_ext, float &dose, float *doselist){

    m_OB_BB_ext.Resize(bb_ext.Dims());
    m_BB_sample_ext.Resize(bb_sample_ext.Dims());
    memcpy(m_OB_BB_ext.GetDataPtr(), bb_ext.GetDataPtr(), sizeof(float)*bb_ext.Size());
    memcpy(m_BB_sample_ext.GetDataPtr(), bb_sample_ext.GetDataPtr(), sizeof(float)*bb_sample_ext.Size());

    //    kipl::io::WriteTIFF32(m_OB_BB_ext,"m_OB_BB_ext.tif");

//    kipl::base::TImage<float,2> slice(m_OB_BB_ext.Dims());
//    memcpy(slice.GetDataPtr(),m_BB_sample_ext.GetLinePtr(0,0), sizeof(float)*bb_ext.Size());
//    kipl::io::WriteTIFF32(slice,"slice.tif");

    fdoseOB_ext = dose;
    fdose_ext_list = new float[bb_sample_ext.Size(2)];
    memcpy(fdose_ext_list, doselist, sizeof(float)*bb_sample_ext.Size(2));

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

std::ostream & operator<<(ostream & s, ImagingAlgorithms::ReferenceImageCorrection::eReferenceMethod erm)
{
    s<<enum2string(erm);

    return s;
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


