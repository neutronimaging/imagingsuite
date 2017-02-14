/*
 * ReferenceImageCorrection.cpp
 *
 *  Created on: Jan 17, 2013
 *      Author: anders
 */

//#include "stdafx.h"
#include "../include/ReferenceImageCorrection.h"
#include "../include/ImagingException.h"
#include <strings/miscstring.h>

//#include <cstring>
#include <map>
#include <sstream>
#include <math/median.h>

#include <io/io_tiff.h>
#include <segmentation/thresholds.h>
#include <base/thistogram.h>
#include <base/tprofile.h>
#include <base/tsubimage.h>


#include <tnt_array1d.h>
#include <tnt_array2d.h>
#include <jama_lu.h>
#include <jama_qr.h>

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
    m_nBBimages(0)
{
    m_nDoseROI[0]=0;
    m_nDoseROI[1]=0;
    m_nDoseROI[2]=0;
    m_nDoseROI[3]=0;

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
        bool useBB,
        float dose_OB,
        float dose_DC,
        bool normBB,
        size_t *roi)
{
	m_bHaveOpenBeam=m_bHaveDarkCurrent=m_bHaveBlackBody=false;
	m_bHaveDoseROI=m_bHaveBlackBodyROI=false;

	if (ob!=NULL) {
		m_bHaveOpenBeam=true;
        m_OpenBeam=*ob;
//        kipl::io::WriteTIFF32(m_OpenBeam,"~/repos/testdata/roi_ob.tif");
        std::cout << "have OB!" << std::endl;
	}

	if (dc!=NULL) {
		m_bHaveDarkCurrent=true;
        m_DarkCurrent=*dc;
//        kipl::io::WriteTIFF32(m_DarkCurrent,"~/repos/testdata/roi_dc.tif");
        std::cout << "have DC!" << std::endl;
	}

    if(roi!=nullptr){

        memcpy(m_nDoseROI,roi, sizeof(size_t)*4);
    }

    if (dose_OB!=0) {
        m_bHaveDoseROI=true;
        m_fOpenBeamDose = dose_OB;
                std::cout << " have dose roi!  " << std::endl;
        //        std::cout << dose_OB << std::endl;


    }

    if (dose_DC!=0){
       m_fDarkDose = dose_DC;
    }

    if (normBB) {
        m_bHaveBlackBodyROI = true;
        m_bHaveBBDoseROI = true;

        std::cout << "have dose roi for BBs!" << std::endl;
//		memcpy(m_nBlackBodyROI,dose_BB,4*sizeof(size_t));
    }


    if (useBB) {
		m_bHaveBlackBody=true;
        // try with the open beam first,, and then go on with the sample one
        std::cout << "have BB!" << std::endl;

        // to understand what is done here with these images... they are used to compute the dose in the PB variante
        // for some reason they must be declared at this point otherwise it chrashes

        m_OB_BB_Interpolated = InterpolateBlackBodyImage(ob_bb_parameters, m_nBlackBodyROI); // it is done in process it seems..
        m_DoseBBflat_image = InterpolateBlackBodyImage(ob_bb_parameters, m_nDoseROI);

//        std::cout << "m_OB_BB_Interpolated image SIZEs: " << m_OB_BB_Interpolated.Size(0) << " " << m_OB_BB_Interpolated.Size(1) << std::endl;
//        std::cout << "m_DoseBBflat_image image SIZEs: " << m_DoseBBflat_image.Size(0) << " " << m_DoseBBflat_image.Size(1) << std::endl;
        PrepareReferencesBB();
	}
    else {
        PrepareReferences();
    }



//    std::cout << "roi: " << roi[0] << " " << roi[1] << " " << roi[2] << " " << roi[3] <<std::endl;


//    std::cout << "before prepare references " << std::endl;



//    std::cout << "after prepare references " << std::endl;

}

void ReferenceImageCorrection::SetInterpolationOrderX(eInterpMethodX eim_x){
    m_IntMeth_x = eim_x;
    std::cout << enum2string(m_IntMeth_x)<< std::endl;

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

    std::cout << "x order: " << b << " " << d << " " << e << std::endl;
}

void ReferenceImageCorrection::SetInterpolationOrderY(eInterpMethodY eim_y){
    m_IntMeth_y = eim_y;
    std::cout << enum2string(m_IntMeth_y)<< std::endl;

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

    std::cout << "y order: " << c << " " << a << " " << f << std::endl;

}

kipl::base::TImage<float,2> ReferenceImageCorrection::Process(kipl::base::TImage<float,2> &img, float dose)
{
    if (m_bComputeLogarithm) {
//        std::cout << "ComputeLogNorm" << std::endl;
        ComputeLogNorm(img,dose);
    }
	else
		ComputeNorm(img,dose);

    return img;

}

void ReferenceImageCorrection::Process(kipl::base::TImage<float,3> &img, float *dose)
{
	kipl::base::TImage<float, 2> slice(img.Dims());
    std::cout<< img.Size(0) << " " << img.Size(1) << " " << img.Size(2) << std::endl;
    float *current_param = new float[6];

	for (size_t i=0; i<img.Size(2); i++) {

        if (m_bHaveBlackBody) {
            // compute m_BB_sample_Interpolated just for the current slice
            // here I have to know if it is to be interpolated between projections
            //check how many parameters do i have?
            if (i==0) {
                std::cout << "number of BB images with sample: " <<  m_nBBimages <<std::endl;
            }
            // so i am expetcing that the parameters are m_nBBimages*6 size.. of course! go on from here after lunch!
//            sample_bb_interp_parameters = InterpolateParameters(sample_bb_parameters, img.Size());

            memcpy(current_param, sample_bb_interp_parameters+i*6, sizeof(float)*6);
            m_BB_sample_Interpolated = InterpolateBlackBodyImage(current_param ,m_nBlackBodyROI);
            m_DoseBBsample_image = InterpolateBlackBodyImage(current_param, m_nDoseROI);


            if (i==10) // random numbers
            {
//                    kipl::io::WriteTIFF32(m_BB_sample_Interpolated,"~/repos/testdata/samplebb10.tif");
            }

            if (i==300) // random numbers
            {
//                    kipl::io::WriteTIFF32(m_BB_sample_Interpolated,"~/repos/testdata/samplebb300.tif");
            }


        }

        memcpy(slice.GetDataPtr(),img.GetLinePtr(0,i), sizeof(float)*slice.Size());
        Process(slice,dose[i]);
        memcpy(img.GetLinePtr(0,i), slice.GetDataPtr(), sizeof(float)*slice.Size()); // and copy the result back
	}

    delete[] current_param;
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

//    std::cout << "max: " << *std::max_element(norm.GetLinePtr(0), norm.GetLinePtr(0)+norm.Size()) << std::endl;
//    std::cout << "min: " << *std::min_element(norm.GetLinePtr(0), norm.GetLinePtr(0)+norm.Size()) << std::endl;


    size_t * vec_hist = new size_t[256];
    for(int i=0; i<256; i++) {

//    std::cout << histo.at(i).first <<  " " << histo.at(i).second << std::endl;
    vec_hist[i] = histo.at(i).second;

    }




//     for( size_t it = histo.begin(); it != histo.end(); ++it )
//        {
//          float* p_a = it.first;
//          size_t* p_b = it.second;
//        }

    //2.b compute otsu threshold

    int value = kipl::segmentation::Threshold_Otsu(vec_hist, 256);
//    std::cout << "OTSU THRESHOLD: " << std::endl;
//    std::cout<< value << std::endl;
//    std::cout << histo.at(value).first << " "<< histo.at(value).second << std::endl;
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

   // inverts values back
//    for (size_t i=0; i<N; i++) {
//        if (res[i]==0) res[1]=1;
//        else res[i]=0;
//    }

//    kipl::io::WriteTIFF32(maskOtsu,"~/testdata/maskOtsu.tif");


    // 3. Compute mask within Otsu
    // 3.a sum of rows and columns and location of rois

//    const size_t *dims = new size_t[2];
//    dims[0] = maskOtsu.Size(0);
//    dims[1] = maskOtsu.Size(1);

//    const size_t dims[2] = {maskOtsu.Size(0), maskOtsu.Size(1)};
    float *vert_profile = new float[maskOtsu.Size(1)];
    float *hor_profile = new float[maskOtsu.Size(0)];


    kipl::base::VerticalProjection2D(maskOtsu.GetDataPtr(), maskOtsu.Dims(), vert_profile, true); // sum of rows
    kipl::base::HorizontalProjection2D(maskOtsu.GetDataPtr(), maskOtsu.Dims(), hor_profile, true); // sum of columns

    // they seem meaningfull
//    std::cout<< "vertical profiles: " << std::endl;
//    for (size_t i=0; i<maskOtsu.Size(1); i++) {
//        std::cout << vert_profile[i] << std::endl;
//    }

//    std::cout<< "horizontal profiles: " << std::endl;
//    for (size_t i=0; i<maskOtsu.Size(0); i++) {
//        std::cout << hor_profile[i] << std::endl;
//    }

    //3.b create binary Signal
    float *bin_VP = new float[maskOtsu.Size(1)];
    float *bin_HP = new float[maskOtsu.Size(0)];

//    std::cout << "binary vertical signal" << std::endl;
    for (size_t i=0; i<maskOtsu.Size(1); i++) {
        float max = *std::max_element(vert_profile, vert_profile+maskOtsu.Size(1));
        if(vert_profile[i]<max) {
            bin_VP[i] = 1;
        }
        else {
            bin_VP[i] = 0;
        }

//        std::cout << bin_VP[i] << std::endl;
    }

//    std::cout << "binary horizontal signal" << std::endl;

    for (size_t i=0; i<maskOtsu.Size(0); i++) {
        float max = *std::max_element(hor_profile, hor_profile+maskOtsu.Size(0));
        if(hor_profile[i]<max) {
            bin_HP[i] = 1;
        }
        else {
            bin_HP[i] = 0;
        }

//        std::cout << bin_HP[i] << std::endl;
    }

    // 3.c compute edges - diff signal
//    std::vector<float> pos_left(100);
    float *pos_left = new float[100];
    float *pos_right = new float[100];
    int index_left = 0;
    int index_right = 0;

//    std::cout << "vertical edges: " << std::endl;
     for (int i=0; i<maskOtsu.Size(1)-1; i++) {
         float diff = bin_VP[i+1]-bin_VP[i];
         if (diff>=1) {
             pos_left[index_left] = i;
//             std::cout << pos_left[index_left] << std::endl;
             index_left++;
         }
         if (diff<=-1) {
             pos_right[index_right]=i+1; // to have all BB within the rois
//             std::cout << pos_right[index_right] << std::endl;
             index_right++;
         }
     }
//     std::cout<< "index left: " << index_left << std::endl;
//     std::cout << "index right: " << index_right << std::endl;

     // the same on the horizontal profile:
     float *pos_left_2 = new float[100];
     float *pos_right_2 = new float[100];
     int index_left_2 = 0;
     int index_right_2 = 0;

//     std::cout << "horizontal edges" << std::endl;
      for (int i=0; i<maskOtsu.Size(0)-1; i++) {
          float diff = bin_HP[i+1]-bin_HP[i];
          if (diff>=1) {
              pos_left_2[index_left_2] = i;
//              std::cout << pos_left_2[index_left_2] << std::endl;
              index_left_2++;
          }
          if (diff<=-1) {
              pos_right_2[index_right_2]=i+1;
//              std::cout << pos_right_2[index_right_2] << std::endl;
              index_right_2++;
          }
      }

//      std::cout<< "index left: " << index_left_2 << std::endl;
//      std::cout << "index right: " << index_right_2 << std::endl;

      // from these define ROIs containing BBs --> i can probably delete them later on
      std::vector<pair <int,int>> right_edges((index_left)*(index_left_2));
      std::vector<pair <int,int>> left_edges((index_left)*(index_left_2));

//      std::cout << "edges together: " << std::endl;
//      std::cout << (index_left-1)*(index_left_2-1)<< std::endl;


      int pos = 0;
      for (int i=0; i<(index_left); i++) {

          for (int j=0; j<(index_left_2); j++) {

                right_edges.at(pos).first = pos_right[i]; // right position on vertical profiles = sum of rows = y axis = y1
                right_edges.at(pos).second = pos_right_2[j]; // right position on horizontal profiles = sum of columns = x axis = x1
                left_edges.at(pos).first = pos_left[i]; // left position on vertical profiles = y0
                left_edges.at(pos).second = pos_left_2[j]; // left position on horizontal profiles = x0

//                std::cout << "x0 y0 x1 y1: " << std::endl;

//                std::cout << left_edges.at(pos).second << " " << left_edges.at(pos).first << " ";
//                std::cout << right_edges.at(pos).second << " " << right_edges.at(pos).first << std::endl;
                pos++;

          }

      }


//      std::cout << "pos: " << pos << std::endl; // it is the number of potential BBs that I have found:

      for (size_t bb_index=0; bb_index<pos; bb_index++){

          const size_t roi_dim[2] = { size_t(right_edges.at(bb_index).second-left_edges.at(bb_index).second), size_t(right_edges.at(bb_index).first-left_edges.at(bb_index).first)}; // Dx and Dy
//          std::cout << "roi1 dimension: "<< roi_dim[0]<< " " << roi_dim[1]<< std::endl;
          kipl::base::TImage<float,2> roi(roi_dim);
          kipl::base::TImage<float,2> roi_im(roi_dim);


          for (int i=0; i<roi_dim[1]; i++) {
                memcpy(roi.GetLinePtr(i),maskOtsu.GetLinePtr(left_edges.at(bb_index).first+i)+left_edges.at(bb_index).second, sizeof(float)*roi_dim[0]); // one could use tsubimage
                memcpy(roi_im.GetLinePtr(i),norm.GetLinePtr(left_edges.at(bb_index).first+i)+left_edges.at(bb_index).second, sizeof(float)*roi_dim[0]);
//                std::cout << left_edges.at(bb_index).first+i << "  " << left_edges.at(bb_index).second << " " << right_edges.at(bb_index).second<< std::endl;
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
          if (size>=20) { // to check if 20 is a meaningfull number


              x_com /=sum_roi;
              y_com /=sum_roi;

              // std::cout << "roi size: " << roi.Size(0) << " " << roi.Size(1) << std::endl;

              // std::cout << "x_com and y_com and sum_roi: " << x_com << " " << y_com << " " << sum_roi << std::endl; // i don't understand why sometimes I have nan... should not really happens


                  for (size_t x=0; x<roi.Size(0); x++) {
                      for (size_t y=0; y<roi.Size(1); y++) {
                          if ((sqrt(int(x-x_com+0.5)*int(x-x_com+0.5)+int(y-y_com+0.5)*int(y-y_com+0.5)))<=radius && roi(x,y)==0) {
//                              roi(x,y)=1; // this one actually I don't need
                              mask(x+left_edges.at(bb_index).second, y+left_edges.at(bb_index).first) = 1;
                          }
                      }
                  }

                  roi(int(x_com+0.5), int(y_com+0.5))=2;
//                  std::cout << roi(0,0)<< std::endl;
//                  std::cout << x_com << " " << y_com << std::endl;
          }

      }


//      kipl::io::WriteTIFF32(mask,"/home/carminati_c/repos/testdata/mask.tif");

}

void ReferenceImageCorrection::SetBBInterpRoi(size_t *roi){

    memcpy(m_nBlackBodyROI,roi,sizeof(size_t)*4);
//    std::cout << "black body roi: " << m_nBlackBodyROI[0] << " " << m_nBlackBodyROI[1] << " " << m_nBlackBodyROI[2] << " "
//                                       << m_nBlackBodyROI[3] << std::endl;

}

float* ReferenceImageCorrection::ComputeInterpolationParameters(kipl::base::TImage<float,2>&mask, kipl::base::TImage<float,2>&img){

    // IMPORTANT! WHEN THE A SPECIFIC ROI IS SELECTED FOR THE BBs, THEN THE INTERPOLATION PARAMETERS NEED TO BE COMPUTED ON THE BIGGER IMAGE GRID
    std::map<std::pair<size_t,size_t>, float> values;


//      std::cout << "VALUES TO INTERPOLATE: " << std::endl;
    // find values to interpolate

    float mean_value = 0.0f;
    for (size_t x=0; x<mask.Size(0); x++) {
        for (size_t y=0; y<mask.Size(1); y++) {
            if (mask(x,y)==1){
                std::pair<size_t, size_t> temp;
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

    for (std::map<std::pair<size_t,size_t>, float>::const_iterator it = values.begin();
                it != values.end(); ++it) {
        std_dev += (it->second-mean_value)*(it->second-mean_value);
    }

    std_dev=sqrt(std_dev/values.size());


//    std::cout << "MEAN AND STD DEV: " << mean_value << " " << std_dev << std::endl;

    for (std::map<std::pair<size_t,size_t>, float>::const_iterator it = values.begin();
                it != values.end(); ++it) {

        if(it->second >= (mean_value+2*std_dev)) {
//            std::cout << "found outlier value" << std::endl;
//            std:cout << it->first.first << " " << it->first.second << std::endl;
            values.erase(it); // not sure it won't cause problems

        }

    }

    float *myparam = new float[6];
    float error;
    myparam = SolveLinearEquation(values, error);


// OLD CODE
//    // uncomment this to print out values from map

////      std::cout << values.size() << std::endl;

////      std::cout << "FROM MAP: " << std::endl;


////      for(std::map<std::pair<size_t,size_t>, float>::const_iterator it = values.begin();
////          it != values.end(); ++it)
////      {
////          std::cout << it->first.first << " " << it->first.second << " " << it->second << "\n";
////      } // giusto!!!

//    // c_2D_2order_int:

//    Array2D< double > my_matrix(values.size(),6, 0.0);
//    Array1D< double > I(values.size(), 0.0);
//    Array1D< double > param(6, 0.0);
//    std::map<std::pair<size_t,size_t>, float>::const_iterator it = values.begin();

////    std::cout << "TNT STUFF: " << std::endl;
////    std::cout << my_matrix.dim1() << " " << my_matrix.dim2() << std::endl;
//    for (int x=0; x<my_matrix.dim1(); x++) {
//              my_matrix[x][0] = 1;
//              my_matrix[x][1] = it->first.first;
//              my_matrix[x][2] = it->first.first*it->first.first;
//              my_matrix[x][3] = it->first.first*it->first.second;
//              my_matrix[x][4] = it->first.second;
//              my_matrix[x][5] = it->first.second*it->first.second;

//              I[x] = static_cast<double>(it->second);
////                          std::cout << it->first.first << " " << it->first.second << " " << it->second << "\n";

//              it++;

//    }

//    JAMA::QR<double> qr(my_matrix);
//    param = qr.solve(I);
////    std::cout << "paramters! " << std::endl;
////    std::cout << param.dim()<< std::endl;
//    std::cout << param[0] << "\n" << param[1] << "\n" << param[2] << "\n" << param[3] << "\n" << param[4] << "\n" << param[5] << std::endl;

//    float *myparam = new float[6];
////    memcpy(myparam, float(param), sizeof(float)*6); // problems with casting..
//    myparam[0] = static_cast<float>(param[0]);
//    myparam[1] = static_cast<float>(param[1]);
//    myparam[2] = static_cast<float>(param[2]);
//    myparam[3] = static_cast<float>(param[3]);
//    myparam[4] = static_cast<float>(param[4]);
//    myparam[5] = static_cast<float>(param[5]);

    return myparam;

}

float* ReferenceImageCorrection::ComputeInterpolationParameters(kipl::base::TImage<float,2>&mask, kipl::base::TImage<float,2>&img, float &error){

    // IMPORTANT! WHEN THE A SPECIFIC ROI IS SELECTED FOR THE BBs, THEN THE INTERPOLATION PARAMETERS NEED TO BE COMPUTED ON THE BIGGER IMAGE GRID
    std::map<std::pair<size_t,size_t>, float> values;


//      std::cout << "VALUES TO INTERPOLATE: " << std::endl;
    // find values to interpolate

    kipl::io::WriteTIFF32(mask, "mask.tif");

    float mean_value = 0.0f;
    for (size_t x=0; x<mask.Size(0); x++) {
        for (size_t y=0; y<mask.Size(1); y++) {
            if (mask(x,y)==1){
                std::pair<size_t, size_t> temp;
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

    for (std::map<std::pair<size_t,size_t>, float>::const_iterator it = values.begin();
                it != values.end(); ++it) {
        std_dev += (it->second-mean_value)*(it->second-mean_value);
    }

    std_dev=sqrt(std_dev/values.size());


//    std::cout << "MEAN AND STD DEV: " << mean_value << " " << std_dev << std::endl;

    for (std::map<std::pair<size_t,size_t>, float>::const_iterator it = values.begin();
                it != values.end(); ++it) {

        if(it->second >= (mean_value+2*std_dev)) {
//            std::cout << "found outlier value" << std::endl;
//            std:cout << it->first.first << " " << it->first.second << std::endl;
            values.erase(it); // not sure it won't cause problems

        }

    }


    float *myparam = new float[6];
//    float error;
    float my_error = 0.0f;
    myparam = SolveLinearEquation(values, my_error);
    error = my_error;


// old code..
//    // uncomment this to print out values from map

//      std::cout << values.size() << std::endl;

//      std::cout << "FROM MAP inside ComputeInterpolationParameters: " << std::endl;


//      for(std::map<std::pair<size_t,size_t>, float>::const_iterator it = values.begin();
//          it != values.end(); ++it)
//      {
//          std::cout << it->first.first << " " << it->first.second << " " << it->second << "\n";
//      } // giusto!!! adesso sono diventati strani...

//    // c_2D_2order_int:

//    Array2D< double > my_matrix(values.size(),6, 0.0);
//    Array1D< double > I(values.size(), 0.0);
//    Array1D< double > param(6, 0.0);
//    std::map<std::pair<size_t,size_t>, float>::const_iterator it = values.begin();

////    std::cout << "TNT STUFF: " << std::endl;
////    std::cout << my_matrix.dim1() << " " << my_matrix.dim2() << std::endl;
//    for (int x=0; x<my_matrix.dim1(); x++) {
//              my_matrix[x][0] = 1;
//              my_matrix[x][1] = it->first.first;
//              my_matrix[x][2] = it->first.first*it->first.first;
//              my_matrix[x][3] = it->first.first*it->first.second;
//              my_matrix[x][4] = it->first.second;
//              my_matrix[x][5] = it->first.second*it->first.second;

//              I[x] = static_cast<double>(it->second);
////                          std::cout << it->first.first << " " << it->first.second << " " << it->second << "\n";

//              it++;

//    }

//    JAMA::QR<double> qr(my_matrix);
//    param = qr.solve(I);
////    std::cout << "paramters! " << std::endl;
////    std::cout << param.dim()<< std::endl;
////    std::cout << param[0] << "\n" << param[1] << "\n" << param[2] << "\n" << param[3] << "\n" << param[4] << "\n" << param[5] << std::endl;

//    float *myparam = new float[6];
////    memcpy(myparam, float(param), sizeof(float)*6); // problems with casting..
//    myparam[0] = static_cast<float>(param[0]);
//    myparam[1] = static_cast<float>(param[1]);
//    myparam[2] = static_cast<float>(param[2]);
//    myparam[3] = static_cast<float>(param[3]);
//    myparam[4] = static_cast<float>(param[4]);
//    myparam[5] = static_cast<float>(param[5]);


//    Array1D< double > I_int(values.size(), 0.0);


//    it = values.begin();
//  for (int i=0; i<values.size(); i++){
//            I_int[i] = static_cast<float>(param[0]) + static_cast<float>(param[1])*static_cast<float>(it->first.first)+static_cast<float>(param[2])*static_cast<float>(it->first.first*it->first.first)+static_cast<float>(param[3])*static_cast<float>(it->first.first)*static_cast<float>(it->first.second)+static_cast<float>(param[4])*static_cast<float>(it->first.second)+static_cast<float>(param[5])*static_cast<float>(it->first.second*it->first.second);
//           // img(x,y) = static_cast<float>(param[0]) + static_cast<float>(param[1])*static_cast<float>(x)+static_cast<float>(param[2])*static_cast<float>(x)*static_cast<float>(x)+static_cast<float>(param[3])*static_cast<float>(x)*static_cast<float>(y)+static_cast<float>(param[4])*static_cast<float>(y)+static_cast<float>(param[5])*static_cast<float>(y)*static_cast<float>(y);
//            it++;
//  }


//  error = 0.0f;

//  for (int i=0; i<values.size(); i++){
//        error += (((I_int[i]-I[i])/I[i])*((I_int[i]-I[i])/I[i]));
//  }

//          error = sqrt(1/float(values.size())*error);

////          std::cout << "INTERPOLATION ERROR: " << error << std::endl;

         return myparam;

}

float * ReferenceImageCorrection::SolveLinearEquation(std::map<std::pair<size_t, size_t>, float> &values, float &error){


//          std::cout << values.size() << std::endl;

//          std::cout << "FROM MAP inside SolveLinearEquation: " << std::endl;


//          for(std::map<std::pair<size_t,size_t>, float>::const_iterator it = values.begin();
//              it != values.end(); ++it)
//          {
//              std::cout << it->first.first << " " << it->first.second << " " << it->second << "\n";
//          } // giusto!!!

    int matrix_size = 1 + ((d && a)+b+c+f+e);
//    std::cout << "matrix_size: " << matrix_size << std::endl;

    Array2D< double > my_matrix(values.size(),matrix_size, 0.0);
    Array1D< double > I(values.size(), 0.0);
    Array1D< double > param(matrix_size, 0.0);
    std::map<std::pair<size_t,size_t>, float>::const_iterator it = values.begin();

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
      it++;
  }


    error = 0.0f;

    for (int i=0; i<values.size(); i++){
          error += (((I_int[i]-I[i])/I[i])*((I_int[i]-I[i])/I[i]));
    }

            error = sqrt(1/float(values.size())*error);

//            std::cout << "error in solve linear equation: " << error << std::endl;

    std::cout << myparam[0] << " " << myparam[1] << " " << myparam[2] << " " << myparam[3] << " " << myparam[4] << " " << myparam[5] << std::endl;


    return myparam;


}

kipl::base::TImage<float,2> ReferenceImageCorrection::InterpolateBlackBodyImage(float *parameters, size_t *roi) {

//    std::cout << "roi in interpolated black body image" << std::endl;
//    std::cout << roi[0] << " " << roi[1] << " " << roi[2] << " " << roi[3] << std::endl;

    size_t dimx = roi[2]-roi[0];
    size_t dimy = roi[3]-roi[1];
//    std::cout << "dimx and dimy in interpolateblackbodyimage" << std::endl;
//    std::cout << dimx << " " << dimy << std::endl;

//    std::cout << "parameters in interpolate blackbody image" << std::endl;
//    std::cout << parameters[0] << " "
//                                  <<  parameters[1] << " " <<
//                                       parameters[2] << " " <<
//                                       parameters[3] << " "<<
//                                       parameters[4] << " " <<
//                                       parameters[5] << " " << std::endl;
    size_t dims[2] = {dimx,dimy};
    kipl::base::TImage <float,2> interpolated_img(dims);
    interpolated_img = 0.0f;
//    std::cout << interpolated_img.Size(0) << " " << interpolated_img.Size(1) << std::endl;

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
    memcpy(norm.GetDataPtr(),bb.GetDataPtr(), sizeof(float)*bb.Size());

    norm -=dark;
    norm /= (flat-=dark);

//    kipl::io::WriteTIFF32(norm,"~/repos/testdata/normBB.tif");
//    kipl::io::WriteTIFF32(bb,"~/testdata/BB.tif");
//    kipl::io::WriteTIFF32(flat,"~/repos/testdata/openBB.tif");

    SegmentBlackBody(norm, mask);

//    kipl::io::WriteTIFF32(mask,"~/testdata/mask.tif");

    kipl::base::TImage<float,2> BB_DC(bb.Dims());
    memcpy(BB_DC.GetDataPtr(), bb.GetDataPtr(), sizeof(float)*bb.Size());
    BB_DC-=dark;

    kipl::base::TImage<float, 2> interpolated_BB(bb.Dims());
    interpolated_BB = 0.0f;

    float * param = new float[6];
    param = ComputeInterpolationParameters(mask,BB_DC);

    return param;

}

float * ReferenceImageCorrection::PrepareBlackBodyImage(kipl::base::TImage<float, 2> &flat, kipl::base::TImage<float, 2> &dark, kipl::base::TImage<float, 2> &bb, kipl::base::TImage<float,2> &mask, float &error)
{


    // 1. normalize image

    kipl::base::TImage<float, 2> norm(bb.Dims());
    memcpy(norm.GetDataPtr(),bb.GetDataPtr(), sizeof(float)*bb.Size());

    norm -=dark;
    norm /= (flat-=dark);

//    kipl::io::WriteTIFF32(norm,"~/repos/testdata/normBB.tif");
//    kipl::io::WriteTIFF32(bb,"~/testdata/BB.tif");
//    kipl::io::WriteTIFF32(flat,"~/repos/testdata/openBB.tif");

    SegmentBlackBody(norm, mask);

//    kipl::io::WriteTIFF32(mask,"~/testdata/mask.tif");

    kipl::base::TImage<float,2> BB_DC(bb.Dims());
    memcpy(BB_DC.GetDataPtr(), bb.GetDataPtr(), sizeof(float)*bb.Size());
    BB_DC-=dark;

    kipl::base::TImage<float, 2> interpolated_BB(bb.Dims());
    interpolated_BB = 0.0f;

    float * param = new float[6];
    param = ComputeInterpolationParameters(mask,BB_DC,error);

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

//    std::cout << "SetAngles" << std::endl;
//    std::cout << angles[0] << " " << angles[1] << " " << angles[2] << " " << angles[3] << std::endl;
//    std::cout << m_nProj << std::endl;
//    std::cout << m_nBBimages << std::endl;


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


void ReferenceImageCorrection::SetInterpParameters(float *ob_parameter, float *sample_parameter, size_t nBBSampleCount, size_t nProj, eBBOptions ebo)
{



    // it seems to work.. roi is not used

//    std::cout << "ciao parameters " << std::endl;
//    std::cout << ob_parameter[0] << std::endl;
//    std::cout << sample_parameter[0] << std::endl;
//    std::cout << sample_parameter[10] << std::endl;


//    std::cout << (sizeof(sample_parameter)/sizeof(float)) << std::endl;
//    std::cout << (sizeof(ob_parameter)/sizeof(float)) << std::endl;



    ob_bb_parameters = new float[6];
    sample_bb_interp_parameters = new float[6*m_nProj];

    m_nBBimages = nBBSampleCount; // store how many images with BB and sample i do have
    memcpy(ob_bb_parameters, ob_parameter, sizeof(float)*6);

    switch(ebo) {
    case(Interpolate) : {
        std::cout << "I am in....." << enum2string(ebo) << std::endl;
//        size_t step = (nProj)/(nBBSampleCount);
//        std::cout << "STEP: "  << step<<  std::endl;
        sample_bb_parameters = new float[6*m_nBBimages];
        memcpy(sample_bb_parameters, sample_parameter, sizeof(float)*6*m_nBBimages);
//        float *int_par = new float[6*m_nProj];
        sample_bb_interp_parameters = InterpolateParametersGeneric(sample_bb_parameters);

        // try to add here the doselist stuff

        for (size_t i=0; i<m_nProj; i++){
            for (size_t j=0; j<6; j++){
                sample_bb_interp_parameters[j+i*6] *= (dosesamplelist[i]/tau);
            }
        }

//        std::cout << "prova InterpolateParametersGeneric" << std::endl;
//        for (size_t i=0; i<m_nProj; i++){
//            std::cout << sample_bb_interp_parameters[0+i*6] << " " << sample_bb_interp_parameters[1+i*6] << " " << sample_bb_interp_parameters[2+i*6] << " " <<sample_bb_interp_parameters[3+i*6] << " " <<sample_bb_interp_parameters[4+i*6] << " " << sample_bb_interp_parameters[5+i*6] << std::endl;
//        }


//        int_par = InterpolateParameters(sample_bb_parameters, nProj, step); // it assumes that BB sample image where acquired corresponding to the number of projections.. to be generalized
        break;
    }
    case(OneToOne) : {
        std::cout << "I am in....." << enum2string(ebo) << std::endl;
        memcpy(sample_bb_interp_parameters, sample_bb_parameters, sizeof(float)*6*m_nBBimages);
        break;
    }
    case(Average) : {
        std::cout << "I am in....." << enum2string(ebo) << std::endl;
        sample_bb_parameters = new float[6];
        memcpy(sample_bb_parameters, sample_parameter, sizeof(float)*6);
        sample_bb_interp_parameters = ReplicateParameters(sample_bb_parameters, nProj);
        break;
    }
    }

//    std::cout << (sizeof(sample_bb_parameters)/sizeof(*sample_bb_parameters)) << std::endl;
//    std::cout << (sizeof(ob_bb_parameters)/sizeof(*ob_bb_parameters)) << std::endl;

//    std::cout << ob_bb_parameters[0] << std::endl;
//    std::cout << sample_bb_parameters[0] << std::endl;
//    std::cout << sample_bb_parameters[10] << std::endl;





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

//    std::cout << "bb_angles:" << std::endl;
    for (size_t i=0; i<m_nBBimages; i++){
        bb_angles[i]= angles[2]+(angles[3]-angles[2])/(m_nBBimages-1)*i; // this is now more generic starting at angle angles[2]
//        std::cout << bb_angles[i] << " ";
    }

//    std::cout << std::endl;

//    std::cout << "number of projections: " << m_nProj << std::endl;

    for (size_t i=0; i<m_nProj; i++){
    //1. compute current angle for projection

        curr_angle = angles[0]+(angles[1]-angles[0])/(m_nProj-1)*i;
//        std::cout << "curr angle: " << curr_angle << " ";
//        std::cout << "curr proj: " << i << std::endl;

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
//        std::cout<< "interpolated parameters: " << std::endl;
        for (size_t k=0; k<6;k++){
            temp_param[k] = param[index_1+k]+(step)*(param[index_2+k]-param[index_1+k]);
//            std::cout << temp_param[k] << " ";
        }
//        std::cout << std::endl;

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




//        std::cout << "generic step: " << step << std::endl;

//        std::cout << "angles: " <<*small_a << " " << *big_a << std::endl;



    }

    delete [] bb_angles;

    return interpolated_param;


}

float* ReferenceImageCorrection::InterpolateParameters(float *param, size_t n, size_t step){

    float *interpolated_param = new float[6*(n+1)];
//    float *int_param = new float[6*n];

   // first copy the one that i already have
    std::cout << "copy interpolated values" << std::endl;
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

//    std::cout <<" ---- END TEMP PARAMETER ----" << std::endl;
//   std::cout << "INTERPOLATED PARAMETERS:" << std::endl;

//   for (size_t i=0; i<=6*n; i+=6){

//       for (size_t j=0; j<6; j++){
//           std::cout << interpolated_param[i+j] << " ";
//       }
//       std::cout << std::endl;
//   }




    return interpolated_param;



}


float * ReferenceImageCorrection::ReplicateParameters(float *param, size_t n){
    float *interpolated_param = new float[6*(n+1)];
    int index=0;

        for (int i=0; i<=n; i++){
            memcpy(interpolated_param+index,param,sizeof(float)*6);
            index+=6;
        }

//       std::cout << "Replicated PARAMETERS:" << std::endl;

//       for (size_t i=0; i<=6*n; i+=6){

//           for (size_t j=0; j<6; j++){
//               std::cout << interpolated_param[i+j] << " ";
//           }
//           std::cout << std::endl;
//       }

    return interpolated_param;
}

// not used.
int* ReferenceImageCorrection::repeat_matrix(int* source, int count, int expand) {
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
    float Pdose = 0.0f;
//    if (dose!=dose)
//        throw ReconException("The reference dose is a NaN",__FILE__,__LINE__);

    const int N=static_cast<int>(m_OpenBeam.Size());
    float *pFlat=m_OpenBeam.GetDataPtr();
    float *pDark=m_DarkCurrent.GetDataPtr();

    if (!m_bHaveBlackBody) {

            if (m_bHaveDarkCurrent) {
                #pragma omp parallel for
                for (int i=0; i<N; i++) {
                    float fProjPixel=pFlat[i]-pDark[i];
                    if (fProjPixel<=0)
                        pFlat[i]=0;
                    else

                        pFlat[i]=log(fProjPixel*dose);

                }
            }
            else {
                #pragma omp parallel for
                for (int i=0; i<N; i++) {
                    float fProjPixel=pFlat[i];
                    if (fProjPixel<=0)
                        pFlat[i]=0;
                    else
                       pFlat[i]=log(fProjPixel*dose);
                }
            }
            }

}

void ReferenceImageCorrection::PrepareReferencesBB() {

    if (m_bHaveBlackBody) {

    const int N=static_cast<int>(m_OpenBeam.Size());
    float *pFlat=m_OpenBeam.GetDataPtr();
    float *pDark=m_DarkCurrent.GetDataPtr();

    float dose=1.0f/(m_fOpenBeamDose-m_fDarkDose);
    float Pdose = 0.0f;

    float *pFlatBB = m_OB_BB_Interpolated.GetDataPtr();

        // set here the PB variante...
        if(bPBvariante){

            Pdose = computedose(m_DoseBBflat_image); // this must be the biggest dose !
            dose = 1.0f/(m_fOpenBeamDose-m_fDarkDose-Pdose);
//            std::cout << "dose with PB variante: " << m_fOpenBeamDose-m_fDarkDose-Pdose << std::endl;

        } // for now I assume I have all images.. otherwise makes no much sense


        if (m_bHaveDarkCurrent) {
            #pragma omp parallel for
            for (int i=0; i<N; i++) {

                if(m_bHaveBBDoseROI && m_bHaveDoseROI) {
                    pFlatBB[i]*=(dose/tau);
                }
                float fProjPixel=pFlat[i]-pDark[i]-pFlatBB[i];
                if (fProjPixel<=0)
                    pFlat[i]=0;
                else
                    pFlat[i]=log(fProjPixel*(dose));

            }
        }
        else {



                #pragma omp parallel for
                for (int i=0; i<N; i++) {

//                    if(m_bHaveBBDoseROI && m_bHaveDoseROI) {
//                        pFlatBB[i]*=(dose/tau);
//                    }

                    float fProjPixel=pFlat[i]-pFlatBB[i];
                    if (fProjPixel<=0)
                        pFlat[i]=0;
                    else
                       pFlat[i]=log(fProjPixel*dose);
                }

        }
    }
    else {
        // throw Exception
//        throw ImagingException("Black Body error in PrepareReferencesBB",__FILE__,__LINE__);
        // not the right spot

    }




}

int ReferenceImageCorrection::ComputeLogNorm(kipl::base::TImage<float,2> &img, float dose)
{
    dose = dose - m_fDarkDose;
    float Pdose = 0.0f;



//    float logdose = log(dose<1 ? 1.0f : dose);

    int N=static_cast<int>(img.Size(0)*img.Size(1));

    float *pFlat=m_OpenBeam.GetDataPtr();
    float *pDark=m_DarkCurrent.GetDataPtr();

//    std::cout << m_bHaveDarkCurrent << " " << m_bHaveOpenBeam << std::endl;
//    std::cout << "number of pixels: " << N << std::endl;

    if (!m_bHaveBlackBody) {

                if (m_bHaveDarkCurrent) {
                    if (m_bHaveOpenBeam) {
            //                #pragma omp parallel for firstprivate(pFlat,pDark)

                                float *pImg=img.GetDataPtr();

                                #pragma omp parallel for
                                for (int i=0; i<N; i++) {

            //                        if (i==0) std::cout<< "sono dentro" << std::endl; // ci entra..

                                    float fProjPixel=(pImg[i]-pDark[i]);
                                    if (fProjPixel<=0)
                                        pImg[i]=0;
                                    else
                                        pImg[i]=pFlat[i]-log(fProjPixel)+log(dose<1 ? 1.0f : dose);

                                }

                    }
                    else {
            //            #pragma omp parallel for firstprivate(pDark)

                          float *pImg=img.GetDataPtr();

                            #pragma omp parallel for
                            for (int i=0; i<N; i++) {
                                float fProjPixel=(pImg[i]-pDark[i]);
                                if (fProjPixel<=0)
                                    pImg[i]=0;
                                else
                                   pImg[i]=-log(fProjPixel)+log(dose<1 ? 1.0f : dose);
                            }

                    }
                }
        }
     else {
//        std::cout << "correct for BB as well" << std::endl;
        //these cases are anyhow probably not used since without open beam we would not have segmented anyway the BBs
        if (m_bHaveDarkCurrent) {




            if (m_bHaveOpenBeam) {
    //                #pragma omp parallel for firstprivate(pFlat,pDark)

                        float *pImg=img.GetDataPtr();
                        float *pImgBB = m_BB_sample_Interpolated.GetDataPtr();

                        if(bPBvariante){

//                            kipl::base::TImage<float,2> imgDose = InterpolateBlackBodyImage();
//                            imgDose = kipl::base::TSubImage<float,2>::Get(m_BB_sample_Interpolated, m_nDoseROI); // extract the roi

//                            for (int i=0; i < static_cast<int>(m_DoseBBsample_image.Size(0)*m_DoseBBsample_image.Size(1)); i++){
//                                m_DoseBBsample_image[i]*=(dose/tau);
//                            }

                            Pdose = computedose(m_DoseBBsample_image);
//                            std::cout << "pierre's dose in BB sample interpolated: " <<  Pdose << std::endl;

                        } // for now I assume I have all images.. other wise makes no much sense





                        #pragma omp parallel for
                        for (int i=0; i<N; i++) {

                            // now computed in prepareBBdata
//                            if (m_bHaveBBDoseROI &&  m_bHaveDoseROI){
//                                 pImgBB[i] *=(dose/tau); // this dose must be computed on another roi !
//                            }



                            float fProjPixel=(pImg[i]-pDark[i]-pImgBB[i]); // to add dose normalization in pImgBB - done
                            if (fProjPixel<=0)
                                pImg[i]=0;
                            else
                                pImg[i]=pFlat[i]-log(fProjPixel)+log((dose-Pdose)<1 ? 1.0f : (dose-Pdose));

                        }

            }
            else {
    //            #pragma omp parallel for firstprivate(pDark)

                  float *pImg=img.GetDataPtr();
                  float *pImgBB = m_BB_sample_Interpolated.GetDataPtr();


                    #pragma omp parallel for
                    for (int i=0; i<N; i++) {

//                        if (m_bHaveBBDoseROI &&  m_bHaveDoseROI){
//                             pImgBB[i] *=(dose/tau);
//                        }

                        float fProjPixel=(pImg[i]-pDark[i]-pImgBB[i]);// to add dose normalization in pImgBB - done
                        if (fProjPixel<=0)
                            pImg[i]=0;
                        else
                           pImg[i]=-log(fProjPixel)+log(dose<1 ? 1.0f : dose); // yes but what is logdose if there is no open beam?
                    }

            }
        }

       }

    return 1;
}

void ReferenceImageCorrection::ComputeNorm(kipl::base::TImage<float,2> &img, float dose)
{

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

void ReferenceImageCorrection::SetExternalBBimages(kipl::base::TImage<float, 2> &bb_ext, kipl::base::TImage<float, 3> &bb_sample_ext){

    m_OB_BB_ext.Resize(bb_ext.Dims());
    m_BB_sample_ext.Resize(bb_sample_ext.Dims());
    memcpy(m_OB_BB_ext.GetDataPtr(), bb_ext.GetDataPtr(), sizeof(float)*bb_ext.Size());
    memcpy(m_BB_sample_ext.GetDataPtr(), bb_sample_ext.GetDataPtr(), sizeof(float)*bb_sample_ext.Size());
//    m_OB_BB_ext = bb_ext;
//    m_BB_sample_ext = bb_sample_ext;

    std::cout << "SetExternalBBimages"<< std::endl;
    kipl::io::WriteTIFF32(m_OB_BB_ext,"m_OB_BB_ext.tif");

    kipl::base::TImage<float,2> slice(m_OB_BB_ext.Dims());
    memcpy(slice.GetDataPtr(),m_BB_sample_ext.GetLinePtr(0,0), sizeof(float)*bb_ext.Size());
    kipl::io::WriteTIFF32(slice,"slice.tif");



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

std::string enum2string(ImagingAlgorithms::ReferenceImageCorrection::eReferenceMethod &erm)
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

std::string enum2string(ImagingAlgorithms::ReferenceImageCorrection::eBBOptions &ebo)
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


void  string2enum(std::string str, ImagingAlgorithms::ReferenceImageCorrection::eInterpMethodX &eim_x)
{
    std::map<std::string, ImagingAlgorithms::ReferenceImageCorrection::eInterpMethodX > int_methods;
    int_methods["SecondOrder"] = ImagingAlgorithms::ReferenceImageCorrection::eInterpMethodX::SecondOrder_x;
    int_methods["FirstOrder"] = ImagingAlgorithms::ReferenceImageCorrection::eInterpMethodX::FirstOrder_x;
    int_methods["ZeroOrder"] = ImagingAlgorithms::ReferenceImageCorrection::eInterpMethodX::ZeroOrder_x;

    if (int_methods.count(str)==0)
        throw  ImagingException("The key string does not exist for eInterpMethod",__FILE__,__LINE__);

    eim_x=int_methods[str];


}

std::string enum2string(ImagingAlgorithms::ReferenceImageCorrection::eInterpMethodX &eim_x)
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

std::ostream & operator<<(ostream & s, ImagingAlgorithms::ReferenceImageCorrection::eInterpMethodX eim_x)
{
    s<<enum2string(eim_x);

    return s;
}

void  string2enum(std::string str, ImagingAlgorithms::ReferenceImageCorrection::eInterpMethodY &eim_y)
{
    std::map<std::string, ImagingAlgorithms::ReferenceImageCorrection::eInterpMethodY > int_methods;
    int_methods["SecondOrder"] = ImagingAlgorithms::ReferenceImageCorrection::eInterpMethodY::SecondOrder_y;
    int_methods["FirstOrder"] = ImagingAlgorithms::ReferenceImageCorrection::eInterpMethodY::FirstOrder_y;
    int_methods["ZeroOrder"] = ImagingAlgorithms::ReferenceImageCorrection::eInterpMethodY::ZeroOrder_y;

    if (int_methods.count(str)==0)
        throw  ImagingException("The key string does not exist for eInterpMethod",__FILE__,__LINE__);

    eim_y=int_methods[str];


}

std::string enum2string(ImagingAlgorithms::ReferenceImageCorrection::eInterpMethodY &eim_y)
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

std::ostream & operator<<(ostream & s, ImagingAlgorithms::ReferenceImageCorrection::eInterpMethodY eim_y)
{
    s<<enum2string(eim_y);

    return s;
}




