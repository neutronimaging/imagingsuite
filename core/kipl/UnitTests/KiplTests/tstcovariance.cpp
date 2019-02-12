#include <QtCore/QString>
#include <QtTest/QtTest>

#include <base/timage.h>
#include <math/covariance.h>

#include <cmath>
#include <iostream>

#include "tstcovariance.h"

//tstCovariance::tstCovariance()
//{
//    size_t dims[2]={100,6};
//    kipl::base::TImage<float,2> img(dims);
//    img=0.0f;
//    for (int i=0; i<int(dims[1]); i++) {
//        float *d=img.GetLinePtr(i);
//        float w=2*(i+1)*3.1415926f/float(dims[0]);
// //       std::cout<<"w="<<w<<std::endl;
//        for (int j=0; j<int(dims[0]); j++) {
//            d[j]=sin(j*w)+i;
//        }
//    }
//    sin2D=img;

//}

//void tstCovariance::testDims()
//{
//    kipl::base::TImage<float,2> img=sin2D;

//    img.Clone();

//    kipl::math::Covariance<float> cov;

//    TNT::Array2D<double> C=cov.compute(img.GetDataPtr(),img.Dims(),2);

//    QCOMPARE(C.dim1(),C.dim2());

//    QCOMPARE(C.dim1(),int(img.Size(1)));
//}

//void tstCovariance::testSymmetry()
//{
//    kipl::base::TImage<float,2> img=sin2D;

//    img.Clone();

//    kipl::math::Covariance<float> cov;

//    TNT::Array2D<double> C=cov.compute(img.GetDataPtr(),img.Dims(),2);

//    for (int i=0; i<int(img.Size(1)); i++) {
//        for (int j=i; j<int(img.Size(1)); j++) {
//            QCOMPARE(C[i][j],C[j][i]);
//        }
//    }
//}

//void tstCovariance::testIntactData()
//{
//    kipl::base::TImage<float,2> img=sin2D;

//    img.Clone();

//    kipl::math::Covariance<float> cov;

//    TNT::Array2D<double> C=cov.compute(img.GetDataPtr(),img.Dims(),2);

//    for (int i=0; i<int(img.Size()); i++)
//        QCOMPARE(img[i],sin2D[i]);

//}

//void tstCovariance::testSmallDataCov()
//{
//    kipl::base::TImage<float,2> img=sin2D;

//    img.Clone();

//    kipl::math::Covariance<float> cov;

//    cov.setResultMatrixType(kipl::math::CovarianceMatrix);

//    TNT::Array2D<double> C=cov.compute(img.GetDataPtr(),img.Dims(),2);

//    for (int i=0 ; i<img.Size(1); i++) {
//        for (int j=0 ; j<img.Size(1); j++) {
//  //          std::cout<<std::setw(12)<<C[i][j];

//        }
////        std::cout<<std::endl;
//        QVERIFY(fabs(C[i][i]-0.5)<1e-7);
//    }
//}

//void tstCovariance::testSmallDataCorr()
//{
//    kipl::base::TImage<float,2> img=sin2D;

//    img.Clone();

//    kipl::math::Covariance<float> cov;
//    cov.setResultMatrixType(kipl::math::CorrelationMatrix);

//    TNT::Array2D<double> C=cov.compute(img.GetDataPtr(),img.Dims(),2);

//    for (int i=0 ; i<img.Size(1); i++) {
//        for (int j=0 ; j<img.Size(1); j++) {
//            std::cout<<std::setw(12)<<C[i][j];
//        }
//        std::cout<<std::endl;
//        QVERIFY(fabs(C[i][i]-1.0)<1e-7);
//    }
//}

