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
    m_AverageMethod(ImagingAlgorithms::AverageImage::ImageAverage)
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

}

ReferenceImageCorrection::~ReferenceImageCorrection()
{

}

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
        bool normBB)
{
	m_bHaveOpenBeam=m_bHaveDarkCurrent=m_bHaveBlackBody=false;
	m_bHaveDoseROI=m_bHaveBlackBodyROI=false;

	if (ob!=NULL) {
		m_bHaveOpenBeam=true;
        m_OpenBeam=*ob;
//        m_OpenBeam =  kipl::base::TSubImage<float,2>::Get(m_OpenBeam_all, roi);
//        kipl::io::WriteTIFF32(m_OpenBeam,"~/repos/testdata/roi_ob.tif");
        std::cout << "have OB!" << std::endl;
	}

	if (dc!=NULL) {
		m_bHaveDarkCurrent=true;
        m_DarkCurrent=*dc;
//        m_DarkCurrent = kipl::base::TSubImage<float,2>::Get(m_DarkCurrent_all,roi);
//        kipl::io::WriteTIFF32(m_DarkCurrent,"~/repos/testdata/roi_dc.tif");
        std::cout << "have DC!" << std::endl;
	}

    if (useBB) {
		m_bHaveBlackBody=true;
//        m_BlackBody=*bb;
        // try with the open beam first,, and then go on with the sample one
        std::cout << "have BB!" << std::endl;
        m_OB_BB_Interpolated = InterpolateBlackBodyImage(ob_bb_parameters, m_nBlackBodyROI);

//        m_OB_BB_Interpolated = kipl::base::TSubImage<float,2>::Get(m_OB_BB_Interpolated_all, roi);
//        kipl::io::WriteTIFF32(m_OB_BB_Interpolated,"~/repos/testdata/roiBB.tif"); // CORRECT!!!


	}

    if (dose_OB!=NULL) {
		m_bHaveDoseROI=true;
        m_fOpenBeamDose = dose_OB;
                std::cout << " have dose roi!  " << std::endl;
        //        std::cout << dose_OB << std::endl;


	}

    if (dose_DC!=NULL){
       m_fDarkDose = dose_DC;
    }

    if (normBB) {
		m_bHaveBlackBodyROI=true;
        m_bHaveBBDoseROI = true;

        std::cout << "have dose roi for BBs!" << std::endl;
//		memcpy(m_nBlackBodyROI,dose_BB,4*sizeof(size_t));
	}

//    std::cout << "before prepare references " << std::endl;
	PrepareReferences();
//    std::cout << "after prepare references " << std::endl;

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
      // they seem OK

//      std::cout << "pos: " << pos << std::endl; // it is the number of potential BBs that I have found:

      for (size_t bb_index=0; bb_index<pos; bb_index++){

          const size_t roi_dim[2] = { size_t(right_edges.at(bb_index).second-left_edges.at(bb_index).second), size_t(right_edges.at(bb_index).first-left_edges.at(bb_index).first)}; // Dx and Dy
//          std::cout << "roi1 dimension: "<< roi_dim[0]<< " " << roi_dim[1]<< std::endl;
          kipl::base::TImage<float,2> roi(roi_dim);


          for (int i=0; i<roi_dim[1]; i++) {
                memcpy(roi.GetLinePtr(i),maskOtsu.GetLinePtr(left_edges.at(bb_index).first+i)+left_edges.at(bb_index).second, sizeof(float)*roi_dim[0]); // one could use tsubimage
//                std::cout << left_edges.at(bb_index).first+i << "  " << left_edges.at(bb_index).second << " " << right_edges.at(bb_index).second<< std::endl;
          }

          float x_com= 0.0f;
          float y_com= 0.0f;
          int size = 0;

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

          // draw the circle with radius 2

          // check on BB dimensions:
          if (size>=20) {

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
    std::cout << "black body roi: " << m_nBlackBodyROI[0] << " " << m_nBlackBodyROI[1] << " " << m_nBlackBodyROI[2] << " "
                                       << m_nBlackBodyROI[3] << std::endl;
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
                temp = std::make_pair(x+m_diffBBroi[0],y+m_diffBBroi[1]);// missing: compensate for eventual roi position.. to be set by appropriate GUI in the module
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
            values.erase(it); // not sure it won't cause problems
        }

    }



    // uncomment this to print out values from map

//      std::cout << values.size() << std::endl;

//      std::cout << "FROM MAP: " << std::endl;


//      for(std::map<std::pair<size_t,size_t>, float>::const_iterator it = values.begin();
//          it != values.end(); ++it)
//      {
//          std::cout << it->first.first << " " << it->first.second << " " << it->second << "\n";
//      } // giusto!!!

    // c_2D_2order_int:

    Array2D< double > my_matrix(values.size(),6, 0.0);
    Array1D< double > I(values.size(), 0.0);
    Array1D< double > param(6, 0.0);
    std::map<std::pair<size_t,size_t>, float>::const_iterator it = values.begin();

//    std::cout << "TNT STUFF: " << std::endl;
//    std::cout << my_matrix.dim1() << " " << my_matrix.dim2() << std::endl;
    for (int x=0; x<my_matrix.dim1(); x++) {
              my_matrix[x][0] = 1;
              my_matrix[x][1] = it->first.first;
              my_matrix[x][2] = it->first.first*it->first.first;
              my_matrix[x][3] = it->first.first*it->first.second;
              my_matrix[x][4] = it->first.second;
              my_matrix[x][5] = it->first.second*it->first.second;

              I[x] = static_cast<double>(it->second);
//                          std::cout << it->first.first << " " << it->first.second << " " << it->second << "\n";

              it++;

    }

    JAMA::QR<double> qr(my_matrix);
    param = qr.solve(I);
//    std::cout << "paramters! " << std::endl;
//    std::cout << param.dim()<< std::endl;
//    std::cout << param[0] << "\n" << param[1] << "\n" << param[2] << "\n" << param[3] << "\n" << param[4] << "\n" << param[5] << std::endl;

    float *myparam = new float[6];
//    memcpy(myparam, float(param), sizeof(float)*6); // problems with casting..
    myparam[0] = static_cast<float>(param[0]);
    myparam[1] = static_cast<float>(param[1]);
    myparam[2] = static_cast<float>(param[2]);
    myparam[3] = static_cast<float>(param[3]);
    myparam[4] = static_cast<float>(param[4]);
    myparam[5] = static_cast<float>(param[5]);




//    for (size_t x=0; x<img.Size(0); x++) {
//        for (size_t y=0; y<img.Size(1); y++){
//            interpolated_img(x,y) = static_cast<float>(param[0]) + static_cast<float>(param[1])*static_cast<float>(x)+static_cast<float>(param[2])*static_cast<float>(x)*static_cast<float>(x)+static_cast<float>(param[3])*static_cast<float>(x)*static_cast<float>(y)+static_cast<float>(param[4])*static_cast<float>(y)+static_cast<float>(param[5])*static_cast<float>(y)*static_cast<float>(y);
//           // img(x,y) = static_cast<float>(param[0]) + static_cast<float>(param[1])*static_cast<float>(x)+static_cast<float>(param[2])*static_cast<float>(x)*static_cast<float>(x)+static_cast<float>(param[3])*static_cast<float>(x)*static_cast<float>(y)+static_cast<float>(param[4])*static_cast<float>(y)+static_cast<float>(param[5])*static_cast<float>(y)*static_cast<float>(y);

//        }
//    }


//    float error = 0.0f;
//  for(std::map<std::pair<size_t,size_t>, float>::const_iterator it = values.begin();
//      it != values.end(); ++it)
//  {
//      error += ((interpolated_img(it->first.first,it->first.second)-it->second)/interpolated_img(it->first.first,it->first.second))*((interpolated_img(it->first.first,it->first.second)-it->second)/interpolated_img(it->first.first,it->first.second));
//  }

//          error = sqrt(1/float(values.size())*error);

//          std::cout << "INTERPOLATION ERROR: " << error << std::endl;

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

//    m_BlackBody_all = bb;
//    m_OpenBeam_all = flat;
//    m_DarkCurrent_all = dark;
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

//    kipl::base::TImage<float, 2> interpolated_BB(bb.Dims());
//    interpolated_BB = 0.0f;

    float * param = new float[6];
    param = ComputeInterpolationParameters(mask,BB_DC);
//    std::cout << "PARAMETERS: " << std::endl;
//    std::cout << param[0] << " " << param[1] << " " << param[2] << " " << param[3] << " " << param[4] << " " << param[5] << std::endl;
//    InterpolateBlackBodyImage(param, interpolated_BB);
//    kipl::io::WriteTIFF32(interpolated_BB,"/home/carminati_c/repos/testdata/interpolated_BB.tif");
//    float error;
//    error = ComputeInterpolationError(interpolated_BB,mask,BB_DC);
//    std::cout << "ERROR: "<< error << std::endl;
//    return interpolated_BB;
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

void ReferenceImageCorrection::SetInterpParameters(float *ob_parameter, float *sample_parameter, size_t nBBSampleCount, size_t nProj)
{

    // it seems to work.. roi is not used

//    std::cout << "ciao parameters " << std::endl;
//    std::cout << ob_parameter[0] << std::endl;
//    std::cout << sample_parameter[0] << std::endl;
//    std::cout << sample_parameter[10] << std::endl;


//    std::cout << (sizeof(sample_parameter)/sizeof(float)) << std::endl;
//    std::cout << (sizeof(ob_parameter)/sizeof(float)) << std::endl;

    ob_bb_parameters = new float[6];
    sample_bb_parameters = new float[6*nBBSampleCount];
    m_nBBimages = nBBSampleCount; // store how many images with BB and sample i do have
//    ob_bb_parameters = ob_parameter;
//    sample_bb_parameters = sample_parameter;

    memcpy(ob_bb_parameters, ob_parameter, sizeof(float)*6);
    memcpy(sample_bb_parameters, sample_parameter, sizeof(float)*6*nBBSampleCount);

    if (nBBSampleCount<=nProj) {
        // interpolate assuming the step is regular.. for now
        size_t step = (nProj)/(nBBSampleCount);
//        std::cout << "STEP: "  << step<<  std::endl;
        sample_bb_interp_parameters = InterpolateParameters(sample_bb_parameters, nProj, step);

    }
    else { // I assume that the same number of images were loaded.
        memcpy(sample_bb_interp_parameters, sample_bb_parameters, sizeof(float)*6*nBBSampleCount);
    }

//    std::cout << (sizeof(sample_bb_parameters)/sizeof(*sample_bb_parameters)) << std::endl;
//    std::cout << (sizeof(ob_bb_parameters)/sizeof(*ob_bb_parameters)) << std::endl;

//    std::cout << ob_bb_parameters[0] << std::endl;
//    std::cout << sample_bb_parameters[0] << std::endl;
//    std::cout << sample_bb_parameters[10] << std::endl;


    // if the number of interpolated parameters is lower than excepted for all projections.. interpolate them



}

float* ReferenceImageCorrection::InterpolateParameters(float *param, size_t n, size_t step){

    float *interpolated_param = new float[6*(n+1)];
//    float *int_param = new float[6*n];

   // first copy the one that i already have
    std::cout << "copy interpolated values" << std::endl;
    size_t index = 0;
    for (size_t i=0; i<6*n; i+=step*6){
//        std::cout << i << " ";
//        std::cout << index << std::endl;
        memcpy(interpolated_param+i, param+index, sizeof(float)*6);
        index +=6;

    }

    memcpy(interpolated_param+6*n, param, sizeof(float)*6); // copio i primi in coda


//    std::cout << "they should be the same: " << std::endl; //and they are
//    std::cout << interpolated_param[600] << " " << interpolated_param[601] << " "  << interpolated_param[602] << " "<< interpolated_param[603] << " "
//                                            << interpolated_param[604] << " "<< interpolated_param[605] << std::endl;
//    std::cout << param[24] << " " << param[25] << " "  << param[26] << " "<< param[27] << " "
//                                            << param[28] << " "<< param[29] << std::endl;



//    std::cout << "step: " << step << std::endl;
//    std::cout << "interpolate remaining: " << std::endl;

    for (size_t i=0; i<6*n; i+=step*6){
            index=i;
//        std::cout << i << std::endl  ;

//        std::cout  << interpolated_param[i] << " " << interpolated_param[i+1] << " "
//                       << interpolated_param[i+2] << " "
//                           << interpolated_param[i+3] << " "
//                               << interpolated_param[i+4] << " "
//                                   << interpolated_param[i+5] << std::endl;
        for (size_t j=1; j<step; j++){
            float *temp_param = new float[6];
//            std::cout << "i and i+step*6 " <<i << " " << i+step*6<< std::endl;
//            std::cout << "j: " <<j << std::endl;
//            std::cout << interpolated_param[i] << " " << interpolated_param[i+step*6]<< std::endl;
            for (size_t k=0; k<6; k++) {
                   temp_param[k] = interpolated_param[i+k]+(j)*(interpolated_param[i+k+step*6]-interpolated_param[i+k])/(step);
//                    std::cout << temp_param[k] <<  " " ;
            }
//            std::cout << std::endl
//            std::cout << "i and i+step*6 " <<i << " " << i+step*6<<    std::endl;
            index +=6;
//            std::cout << "index: " << (index) << std::endl;
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
    else {


        float *pFlatBB = m_OB_BB_Interpolated.GetDataPtr();

        if (m_bHaveDarkCurrent) {
            #pragma omp parallel for
            for (int i=0; i<N; i++) {

                if(m_bHaveBBDoseROI && m_bHaveDoseROI) {
                    pFlatBB[i]*=(dose/tau);
                }
                float fProjPixel=pFlat[i]-pDark[i]-pFlatBB[i]; // dose parameter to be added in pFlatBB
                if (fProjPixel<=0)
                    pFlat[i]=0;
                else
                    pFlat[i]=log(fProjPixel*dose);

            }
        }
        else {



                #pragma omp parallel for
                for (int i=0; i<N; i++) {

                    if(m_bHaveBBDoseROI && m_bHaveDoseROI) {
                        pFlatBB[i]*=(dose/tau);
                    }

                    float fProjPixel=pFlat[i]-pFlatBB[i]; // dose parameter to be added here
                    if (fProjPixel<=0)
                        pFlat[i]=0;
                    else
                       pFlat[i]=log(fProjPixel*dose);
                }

        }


    }

// let's assume for now that I do compute the logarithm
//	if (m_bComputeLogarithm) {

//	}
//	else {

//	}

}

int ReferenceImageCorrection::ComputeLogNorm(kipl::base::TImage<float,2> &img, float dose)
{
    dose = dose - m_fDarkDose;
    float logdose = log(dose<1 ? 1.0f : dose);

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
                                        pImg[i]=pFlat[i]-log(fProjPixel)+logdose;

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
                                   pImg[i]=-log(fProjPixel)+logdose;
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



                        #pragma omp parallel for
                        for (int i=0; i<N; i++) {

                            if (m_bHaveBBDoseROI &&  m_bHaveDoseROI){
                                 pImgBB[i] *=(dose/tau);
                            }

                            float fProjPixel=(pImg[i]-pDark[i]-pImgBB[i]); // to add dose normalization in pImgBB
                            if (fProjPixel<=0)
                                pImg[i]=0;
                            else
                                pImg[i]=pFlat[i]-log(fProjPixel)+logdose;

                        }

            }
            else {
    //            #pragma omp parallel for firstprivate(pDark)

                  float *pImg=img.GetDataPtr();
                  float *pImgBB = m_BB_sample_Interpolated.GetDataPtr();


                    #pragma omp parallel for
                    for (int i=0; i<N; i++) {

                        if (m_bHaveBBDoseROI &&  m_bHaveDoseROI){
                             pImgBB[i] *=(dose/tau);
                        }

                        float fProjPixel=(pImg[i]-pDark[i]-pImgBB[i]);// to add dose normalization in pImgBB
                        if (fProjPixel<=0)
                            pImg[i]=0;
                        else
                           pImg[i]=-log(fProjPixel)+logdose; // yes but what is logdose if there is no open beam?
                    }

            }
        }

       }

    return 1;
}

void ReferenceImageCorrection::ComputeNorm(kipl::base::TImage<float,2> &img, float dose)
{

}




}





void string2enum(std::string str, ImagingAlgorithms::ReferenceImageCorrection::eReferenceMethod &erm)
{
    std::map<std::string, ImagingAlgorithms::ReferenceImageCorrection::eReferenceMethod> methods;
    methods["LogNorm"] = ImagingAlgorithms::ReferenceImageCorrection::ReferenceLogNorm;
    methods["BBLogNorm"] = ImagingAlgorithms::ReferenceImageCorrection::ReferenceBBLogNorm; // to be updated with more options

    if (methods.count(str)==0)
        throw ImagingException("The key string does not exist for eReferenceMethod",__FILE__,__LINE__);

    erm=methods[str];


}

std::string enum2string(ImagingAlgorithms::ReferenceImageCorrection::eReferenceMethod &erm)
{
    std::string str;

    switch (erm) {
        case ImagingAlgorithms::ReferenceImageCorrection::ReferenceLogNorm: str="LogNorm"; break;
        case ImagingAlgorithms::ReferenceImageCorrection::ReferenceBBLogNorm: str="BBLogNorm"; break;
        default: throw ImagingException("Unknown reference method in enum2string for eReferenceMethod",__FILE__,__LINE__);
    }
    return  str;

}

std::ostream & operator<<(ostream & s, ImagingAlgorithms::ReferenceImageCorrection::eReferenceMethod erm)
{
    s<<enum2string(erm);

    return s;
}




