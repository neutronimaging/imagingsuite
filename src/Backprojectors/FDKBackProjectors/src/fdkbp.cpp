
#include <vector>
#include <sstream>
#include <string>
#include <iostream>
#include <emmintrin.h>
#include <cmath>
#include <math.h>

#include "fftw3.h"

#include <base/timage.h>
#include <base/tpermuteimage.h>
#include <math/mathconstants.h>
#include <interactors/interactionbase.h>



#include "fdkbp.h"

#ifndef DEGTORAD
static const double DEGTORAD = dPi/ 180.0;
#endif

#ifndef MARGIN
static const unsigned int MARGIN = 5;
#else
#error "MARGIN IS DEFINED"
#endif


/* Matrix element m[i,j] for matrix with c columns */
#define m_idx(m1,c,i,j) m1[i*c+j]


FDKbp::FDKbp(kipl::interactors::InteractionBase *interactor) :
    FdkReconBase("muhrec","FDKbp",BackProjectorModuleBase::MatrixXYZ,interactor)
{


}

FDKbp::~FDKbp()
{


}

void FDKbp::multiplyMatrix(double *mat1, double *mat2, double *result, int rows, int columns, int columns1){

    int i,j,k;

    for (i = 0; i < rows; i++) {
         for (j = 0; j < columns; j++) {
             float acc = 0.0; // why not double?
             for (k = 0; k < columns1; k++) {
                 acc += m_idx(mat1,columns1,i,k) * m_idx(mat2,columns,k,j);
             }
             m_idx(result,columns,i,j) = acc;
         }
    }

}

void FDKbp::compRTKProjMatrix(float angles, double* nrm, double *proj_matrix){

    // RTK version of projection matrices

    double rotation[16] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
    double translation[12] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
    double sod = mConfig.ProjectionInfo.fSOD; ; /* Distance:source to axis */
    double sdd = mConfig.ProjectionInfo.fSDD; /* Distance: Source to Image */
    double offSourceX =  0.0f; // first with the ideal case. source offset = 0. the source is inline with the isocenter
    double offSourceY = 0.0f;
    double offDetX = (mConfig.ProjectionInfo.nDims[0]/2-mConfig.ProjectionInfo.roi[0])*mConfig.ProjectionInfo.fResolution[0]; // ideal case: the detector is centered with the source and the isocenter
    double offDetY = (mConfig.ProjectionInfo.nDims[1]/2-mConfig.ProjectionInfo.roi[1])*mConfig.ProjectionInfo.fResolution[1];

    std::cout << angles << std::endl;

    // build rotation matrix:
    // rotation given only by the gantry angle - ideal case -
    rotation[0] = cos(-angles*dPi/180);
    rotation[2] = sin(-angles*dPi/180);
    rotation[5] = 1.0;
    rotation[8] = -sin(-angles*dPi/180);
    rotation[10] = cos(-angles*dPi/180);
    rotation[15] = 1.0;



    // build translation matrix
    double MT1 [9] = {1.0, 0.0, offSourceX-offDetX, 0.0, 1.0, offSourceY-offDetY, 0.0, 0.0, 1.0};
    double MT2 [12] = {-sdd, 0.0, 0.0, 0.0, 0.0, -sdd, 0.0, 0.0, 0.0, 0.0, 1.0, -sod};
    double MT3 [16] = {1.0, 0.0, 0.0, -offSourceX, 0.0, 1.0, 0.0, -offSourceY, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0};

    // MT = MT1*MT2*MT3
    double MTT[12]= {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
    multiplyMatrix(MT2, MT3, MTT, 3, 4, 4);

//    printf ("MTT\n%g %g %g %g\n%g %g %g %g\n"
//    "%g %g %g %g\n",
//    MTT[0], MTT[1],
//    MTT[2], MTT[3],
//    MTT[4], MTT[5],
//    MTT[6], MTT[7],
//    MTT[8], MTT[9],
//    MTT[10], MTT[11]);

    multiplyMatrix(MT1, MTT, translation, 3, 4, 3);

//    printf ("translation\n%g %g %g %g\n%g %g %g %g\n"
//    "%g %g %g %g\n",
//    translation[0], translation[1],
//    translation[2], translation[3],
//    translation[4], translation[5],
//    translation[6], translation[7],
//    translation[8], translation[9],
//    translation[10], translation[11]);

    multiplyMatrix(translation, rotation, proj_matrix, 3, 4, 4);

//    printf ("proj_matrix\n%g %g %g %g\n%g %g %g %g\n"
//    "%g %g %g %g\n",
//    proj_matrix[0], proj_matrix[1],
//    proj_matrix[2], proj_matrix[3],
//    proj_matrix[4], proj_matrix[5],
//    proj_matrix[6], proj_matrix[7],
//    proj_matrix[8], proj_matrix[9],
//    proj_matrix[10], proj_matrix[11]);

    nrm[0] = sin(-angles*dPi/180);
    nrm[1] = 0.0;
    nrm[2] = cos(-angles*dPi/180);


}

void FDKbp::computeProjMatrix(float angles, double *nrm, double *proj_matrix){

//    std::cout << mConfig.ProjectionInfo.fpPoint[0] << " " << mConfig.ProjectionInfo.fpPoint[1] << std::endl; // it is already in the projection roi reference
//    std::cout << mConfig.ProjectionInfo.roi[0] << " " << mConfig.ProjectionInfo.roi[1] << " " << mConfig.ProjectionInfo.roi[2] << " " << mConfig.ProjectionInfo.roi[3] << std::endl; // x0 y0 x1 y1
//    std::cout << mConfig.ProjectionInfo.nDims[0] << " " << mConfig.ProjectionInfo.nDims[1] << std::endl; // it is the biggest dimension of the projection data

     // implement smarter projection matrix source perpendicular to the z axis and detector oriented as y
    double extrinsic[16] =   {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}; /* Extrinsic matrix */
    double intrinsic[12] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}; /* Intrinsic matrix */
    double sad = mConfig.ProjectionInfo.fSOD; ; /* Distance:source to axis */
    double sid = mConfig.ProjectionInfo.fSDD; /* Distance: Source to Image */


    double cam[3] = {sad*sin(-angles*dPi/180)+0.0f, 0.0f+0.0f, sad*cos(-angles*dPi/180)}; /* Location of camera */ // so far it is the best..
//    double cam[3] = {sad*sin(-angles*dPi/180)+offSourceX, 0.0f+0.0f, sad*cos(-angles*dPi/180)}; // does not really help
//    double cam[3] = {sad*sin(-angles*dPi/180), 0.0f, sad*cos(-angles*dPi/180)}; /* Location of camera */
    std::cout << angles << std::endl;


//    double ic[2] = {mConfig.ProjectionInfo.fpPoint[0], mConfig.ProjectionInfo.fpPoint[1]};
    double plt[3] = {0.0,0.0,0.0}; /* Detector orientation */  /* Panel left (toward first column) */
    double pup[3] = {0.0,0.0,0.0}; /* Panel up (toward top row) */
    double vup[3] = {0.0,1.0,0.0}; /* hardcoded for ideal panel orientationf */
    double tgt[3] = {0.0,0.0,0.0}; /* isoscenter, center of rotation of the sample in world coordinates */


    /* Compute imager coordinate sys (nrm,pup,plt)
       ---------------
       nrm = cam - tgt
       plt = nrm x vup
       pup = plt x nrm
       ---------------
    */

    nrm[0] = cam[0]-tgt[0];
    nrm[1] = cam[1]-tgt[1];
    nrm[2] = cam[2]-tgt[2];



    double norm_nrm = sqrt(nrm[0]*nrm[0]+nrm[1]*nrm[1]+nrm[2]*nrm[2]);

    nrm[0] /= norm_nrm;
    nrm[1] /= norm_nrm;
    nrm[2] /= norm_nrm;

    plt[0] =  nrm[1]*vup[2]-nrm[2]*vup[1];
    plt[1] =  nrm[2]*vup[0]-nrm[0]*vup[2];
    plt[2] =  nrm[0]*vup[1]-nrm[1]*vup[0];

    double norm_plt = sqrt(plt[0]*plt[0]+plt[1]*plt[1]+plt[2]*plt[2]);

    plt[0] /= norm_plt;
    plt[1] /= norm_plt;
    plt[2] /= norm_plt;

//        vec3_cross (pup, plt, nrm);
//        vec3_normalize1 (pup);

    pup[0] =  plt[1]*nrm[2]-plt[2]*nrm[1];
    pup[1] =  plt[2]*nrm[0]-plt[0]*nrm[2];
    pup[2] =  plt[0]*nrm[1]-plt[1]*nrm[0];

    double norm_pup = sqrt(pup[0]*pup[0]+pup[1]*pup[1]+pup[2]*pup[2]);

    pup[0] /= norm_pup;
    pup[1] /= norm_pup;
    pup[2] /= norm_pup;

    /* Build extrinsic matrix - rotation part */


//        vec3_invert (&pmat->extrinsic[0]);
//        vec3_invert (&pmat->extrinsic[4]);
//        vec3_invert (&pmat->extrinsic[8]);

    extrinsic[0] = -1.0*plt[0];
    extrinsic[1] = -1.0*plt[1];
    extrinsic[2] = -1.0*plt[2];

    extrinsic[4] = -1.0*pup[0];
    extrinsic[5] = -1.0*pup[1];
    extrinsic[6] = -1.0*pup[2];

    extrinsic[8] = -1.0*nrm[0];
    extrinsic[9] = -1.0*nrm[1];
    extrinsic[10] = -1.0*nrm[2];

    extrinsic[15] = 1.0;

    /* Build extrinsic matrix - translation part */

//        pmat->extrinsic[3] = vec3_dot (plt, tgt);
//        pmat->extrinsic[7] = vec3_dot (pup, tgt);
//        pmat->extrinsic[11] = vec3_dot (nrm, tgt) + pmat->sad;


        extrinsic[3] = plt[0]*tgt[0]+plt[1]*tgt[1]+plt[2]*tgt[2]-(mConfig.ProjectionInfo.fpPoint[0]-mConfig.ProjectionInfo.fCenter)*mConfig.ProjectionInfo.fResolution[0]/sid; // added offset contribution.. must be something like this
        extrinsic[7] = pup[0]*tgt[0]+pup[1]*tgt[1]+pup[2]*tgt[2];
        extrinsic[11] = nrm[0]*tgt[0]+nrm[1]*tgt[1]+nrm[2]*tgt[2]+sad;

//   extrinsic[3] = 0;
//   extrinsic[7] = 0;
//   extrinsic[11] = 0+mConfig.ProjectionInfo.fSOD;

   /* Build intrinsic matrix */


//        m_idx (pmat->intrinsic,cols,0,0) = 1 / ps[0];
//        m_idx (pmat->intrinsic,cols,1,1) = 1 / ps[1];
//        m_idx (pmat->intrinsic,cols,2,2) = 1 / sid;
    intrinsic[0] = 1/mConfig.ProjectionInfo.fResolution[0];
//    intrinsic[3] = mConfig.ProjectionInfo.fpPoint[0]-mConfig.ProjectionInfo.fCenter;
    intrinsic[5] = 1/mConfig.ProjectionInfo.fResolution[1];
    intrinsic[10] = 1/sid;

    /* Build Projection Geometry Matrix = intrinsic * extrinsic */
   int i,j,k;

   for (i = 0; i < 3; i++) {
        for (j = 0; j < 4; j++) {
            float acc = 0.0; // why not double?
            for (k = 0; k < 4; k++) {
                acc += m_idx(intrinsic,4,i,k) * m_idx(extrinsic,4,k,j);
            }
            m_idx(proj_matrix,4,i,j) = acc;
        }
   }





}

void FDKbp::getProjMatrix(float angles, double *nrm, double *proj_matrix){

    // compute geometry matrix: detector oriented as the z axis, source perpendicular to x

//    double proj_matrix[12] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}; /* Projection matrix */
    double tilted_proj_matrix[12] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}; /* Tilted projection matrix */
    double tilt_matrix[16] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}; /* Tilt matrix */
    double extrinsic[16] =   {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}; /* Extrinsic matrix */
    double intrinsic[12] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}; /* Intrinsic matrix */
    double sad = mConfig.ProjectionInfo.fSOD; ; /* Distance:source to axis */
    double sid = mConfig.ProjectionInfo.fSDD; /* Distance: Source to Image */
//    double offSourceX =  (mConfig.ProjectionInfo.fpPoint[0]-(mConfig.ProjectionInfo.nDims[0]/2-mConfig.ProjectionInfo.projection_roi[0]))*mConfig.ProjectionInfo.fResolution[0];
//    double offSourceY = (mConfig.ProjectionInfo.fpPoint[1]-(mConfig.ProjectionInfo.nDims[1]/2-mConfig.ProjectionInfo.projection_roi[1]))*mConfig.ProjectionInfo.fResolution[1];

//        double offCenterX =  0.0;
    double offCenter =  (mConfig.ProjectionInfo.fpPoint[0]-mConfig.ProjectionInfo.fCenter)*mConfig.ProjectionInfo.fResolution[0]*sad/sid;
//    std::cout << "offCenterX: "<< offCenterX << std::endl; // it makes sense
//    double offCenterY =  -(mConfig.ProjectionInfo.fpPoint[1]-mConfig.ProjectionInfo.projection_roi[3]/2)*mConfig.ProjectionInfo.fResolution[0]/sid;
//    double OffCenter = 0.0;
//    double offSourceX = 0.0f;
//    double offSourceY = 0.0f;


//    double cam[3] = {-sid*cos(angles*dPi/180)+offSourceX,sid*sin(angles*dPi/180)+offSourceY,sad}; /* Location of camera it is quite wrong
    double cam[3] = {sad*cos(-angles*dPi/180), sad*sin(-angles*dPi/180),0.0}; // Location of camera
//    double nrm[3] = {0.0,0.0,0.0}; /* Ray from image center to source */
//    double ic[2] = {mConfig.ProjectionInfo.fpPoint[0], mConfig.ProjectionInfo.fpPoint[1]}; // not used here
    double plt[3] = {0.0,0.0,0.0}; /* Detector orientation */  /* Panel left (toward first column) */
    double pup[3] = {0.0,0.0,0.0}; /* Panel up (toward top row) */
    double vup[3] = {0.0,0.0,1.0}; /* hardcoded for ideal panel orientationf */
    double tgt[3] = {0.0,0.0,0.0}; // isoscenter, center of rotation of the sample in world coordinates  offCenter in y non ha funzionato tantissimo



    /* Compute imager coordinate sys (nrm,pup,plt)
       ---------------
       nrm = cam - tgt
       plt = nrm x vup
       pup = plt x nrm
       ---------------
    */

    nrm[0] = cam[0];
    nrm[1] = cam[1];
    nrm[2] = cam[2];



    double norm_nrm = sqrt(nrm[0]*nrm[0]+nrm[1]*nrm[1]+nrm[2]*nrm[2]);

    nrm[0] /= norm_nrm;
    nrm[1] /= norm_nrm;
    nrm[2] /= norm_nrm;



    std::cout << angles << std::endl;

//        std::cout << "ray from image center to source: " << std::endl;
//        std::cout << nrm[0] << " " << nrm[1] << " " << nrm[2] << std::endl;

//            vec3_cross (plt, nrm, vup); // cross product
//            vec3_normalize1 (plt);

    plt[0] =  nrm[1]*vup[2]-nrm[2]*vup[1];
    plt[1] =  nrm[2]*vup[0]-nrm[0]*vup[2];
    plt[2] =  nrm[0]*vup[1]-nrm[1]*vup[0];

    double norm_plt = sqrt(plt[0]*plt[0]+plt[1]*plt[1]+plt[2]*plt[2]);

    plt[0] /= norm_plt;
    plt[1] /= norm_plt;
    plt[2] /= norm_plt;

//        vec3_cross (pup, plt, nrm);
//        vec3_normalize1 (pup);

    pup[0] =  plt[1]*nrm[2]-plt[2]*nrm[1];
    pup[1] =  plt[2]*nrm[0]-plt[0]*nrm[2];
    pup[2] =  plt[0]*nrm[1]-plt[1]*nrm[0];

    double norm_pup = sqrt(pup[0]*pup[0]+pup[1]*pup[1]+pup[2]*pup[2]);

    pup[0] /= norm_pup;
    pup[1] /= norm_pup;
    pup[2] /= norm_pup;


//        printf ("CAM = %g %g %g\n", cam[0], cam[1], cam[2]);
//        printf ("TGT = %g %g %g\n", tgt[0], tgt[1], tgt[2]);
//        printf ("NRM = %g %g %g\n", nrm[0], nrm[1], nrm[2]);
//        printf ("PLT = %g %g %g\n", plt[0], plt[1], plt[2]);
//        printf ("PUP = %g %g %g\n", pup[0], pup[1], pup[2]);


    /* Build extrinsic matrix - rotation part */


//        vec3_invert (&pmat->extrinsic[0]);
//        vec3_invert (&pmat->extrinsic[4]);
//        vec3_invert (&pmat->extrinsic[8]);

    extrinsic[0] = -1.0*plt[0];
    extrinsic[1] = -1.0*plt[1];
    extrinsic[2] = -1.0*plt[2];

    extrinsic[4] = -1.0*pup[0];
    extrinsic[5] = -1.0*pup[1];
    extrinsic[6] = -1.0*pup[2];

    extrinsic[8] = -1.0*nrm[0];
    extrinsic[9] = -1.0*nrm[1];
    extrinsic[10] = -1.0*nrm[2];

    extrinsic[15] = 1.0;



     /* Build extrinsic matrix - translation part */

//        pmat->extrinsic[3] = vec3_dot (plt, tgt);
//        pmat->extrinsic[7] = vec3_dot (pup, tgt);
//        pmat->extrinsic[11] = vec3_dot (nrm, tgt) + pmat->sad;

        extrinsic[3] = plt[0]*tgt[0]+plt[1]*tgt[1]+plt[2]*tgt[2]; // -(mConfig.ProjectionInfo.fpPoint[0]-mConfig.ProjectionInfo.fCenter)*mConfig.ProjectionInfo.fResolution[0]*sad/sid
        extrinsic[7] = pup[0]*tgt[0]+pup[1]*tgt[1]+pup[2]*tgt[2];
        extrinsic[11] = nrm[0]*tgt[0]+nrm[1]*tgt[1]+nrm[2]*tgt[2]+sad;

//    extrinsic[3] = 0;
//    extrinsic[7] = 0;
//    extrinsic[11] = 0+mConfig.ProjectionInfo.fSOD;



//        printf ("EXTRINSIC\n%g %g %g %g\n%g %g %g %g\n"
//        "%g %g %g %g\n%g %g %g %g\n",
//        extrinsic[0], extrinsic[1],
//        extrinsic[2], extrinsic[3],
//        extrinsic[4], extrinsic[5],
//        extrinsic[6], extrinsic[7],
//        extrinsic[8], extrinsic[9],
//        extrinsic[10], extrinsic[11],
//        extrinsic[12], extrinsic[13],
//        extrinsic[14], extrinsic[15]);


        /* Build intrinsic matrix */


//        m_idx (pmat->intrinsic,cols,0,0) = 1 / ps[0];
//        m_idx (pmat->intrinsic,cols,1,1) = 1 / ps[1];
//        m_idx (pmat->intrinsic,cols,2,2) = 1 / sid;
    intrinsic[0] = 1/mConfig.ProjectionInfo.fResolution[0];
    intrinsic[5] = 1/mConfig.ProjectionInfo.fResolution[1];
    intrinsic[10] = 1/sid;

//        printf ("INTRINSIC\n%g %g %g %g\n%g %g %g %g\n%g %g %g %g\n",
//        intrinsic[0], intrinsic[1],
//        intrinsic[2], intrinsic[3],
//        intrinsic[4], intrinsic[5],
//        intrinsic[6], intrinsic[7],
//        intrinsic[8], intrinsic[9],
//        intrinsic[10], intrinsic[11]);



    /* Build Projection Geometry Matrix = intrinsic * extrinsic */
   int i,j,k;

   for (i = 0; i < 3; i++) {
        for (j = 0; j < 4; j++) {
            float acc = 0.0; // why not double?
            for (k = 0; k < 4; k++) {
                acc += m_idx(intrinsic,4,i,k) * m_idx(extrinsic,4,k,j);
            }
            m_idx(proj_matrix,4,i,j) = acc;
        }
   }

   //       if (mConfig.ProjectionInfo.bCorrectTilt)  {

   //           /*Build the tilt matrix: rotation around X */

   ////           std::cout << "tilt angle: " << mConfig.ProjectionInfo.fTiltAngle << std::endl;


   //           tilt_matrix[0] = 1.0;
   //           tilt_matrix[1] = 0.0;
   //           tilt_matrix[2] = 0.0;
   //           tilt_matrix[3] = 0.0;

   //           tilt_matrix[5] = cos(mConfig.ProjectionInfo.fTiltAngle*dPi/180);
   //           tilt_matrix[4] = 0.0;
   //           tilt_matrix[6] = sin(mConfig.ProjectionInfo.fTiltAngle*dPi/180);
   //           tilt_matrix[7] = 0.0;

   //           tilt_matrix[9] = -sin(mConfig.ProjectionInfo.fTiltAngle*dPi/180);
   //           tilt_matrix[8] = 0.0;
   //           tilt_matrix[10] = cos(mConfig.ProjectionInfo.fTiltAngle*dPi/180);
   //           tilt_matrix[11] = 0.0;

   //           tilt_matrix[12] = 0.0;
   //           tilt_matrix[13] = 0.0;
   //           tilt_matrix[13] = 0.0;

   //           tilt_matrix[15] = 1.0;


   ////           printf ("TILT MATRIX\n%g %g %g %g\n%g %g %g %g\n%g %g %g %g\n %g %g %g %g\n",
   ////                   tilt_matrix[0], tilt_matrix[1],
   ////                   tilt_matrix[2], tilt_matrix[3],
   ////                   tilt_matrix[4], tilt_matrix[5],
   ////                   tilt_matrix[6], tilt_matrix[7],
   ////                   tilt_matrix[8], tilt_matrix[9],
   ////                   tilt_matrix[10], tilt_matrix[11],
   ////                   tilt_matrix[12], tilt_matrix[13],
   ////                   tilt_matrix[14], tilt_matrix[15]
   ////                   );


   //           for (i=0; i<4; i++) {
   //               for (j=0; j<4; j++) {
   //                   float acc = 0.0;
   //                   for (k=0; k<4; k++) {
   //                       acc += m_idx(proj_matrix,4,i,k)*m_idx(tilt_matrix,4,k,j);
   //                   }
   //                   m_idx(tilted_proj_matrix,4,i,j) = acc;
   //               }
   //           }

   ////           printf ("TILTED PROJECTION MATRIX\n%g %g %g %g\n%g %g %g %g\n%g %g %g %g\n",
   ////                   tilted_proj_matrix[0], tilted_proj_matrix[1],
   ////                   tilted_proj_matrix[2], tilted_proj_matrix[3],
   ////                   tilted_proj_matrix[4], tilted_proj_matrix[5],
   ////                   tilted_proj_matrix[6], tilted_proj_matrix[7],
   ////                   tilted_proj_matrix[8], tilted_proj_matrix[9],
   ////                   tilted_proj_matrix[10], tilted_proj_matrix[11]
   ////                   );

   //       }


//   printf ("GEOMETRY MATRIX in the function\n%g %g %g %g\n%g %g %g %g\n%g %g %g %g\n",
//   proj_matrix[0], proj_matrix[1],
//   proj_matrix[2], proj_matrix[3],
//   proj_matrix[4], proj_matrix[5],
//   proj_matrix[6], proj_matrix[7],
//   proj_matrix[8], proj_matrix[9],
//   proj_matrix[10], proj_matrix[11]);


//   return proj_matrix;
}


/* Main reconstruction loop - Loop over images, and backproject each
    image into the volume. */
size_t FDKbp::reconstruct(kipl::base::TImage<float,2> &proj, float angles)
{

    // missing things todo:
    // 1. timer - OK. total timer
    // 2. weights - OK
    // 3. reconstruct VOI- ok
    // 4. reconstruct from projections ROI - OK
    // 5. use circular mask



//        float scale; // it is weigths that has already been taken into account in fdkreconbase i would say
//        double filter_time = 0.0;
//        double backproject_time = 0.0;
//        double io_time = 0.0;
        double nrm[3] = {0.0, 0.0, 0.0};
        double proj_matrix[12] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}; /* Projection matrix */
//        double proj_matrix2[12] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}; /* Projection matrix */

//        std::cout << mConfig.ProjectionInfo.fCenter<<" " << mConfig.ProjectionInfo.fpPoint[0]<< " "<< mConfig.ProjectionInfo.fpPoint[1]<< std::endl;
        getProjMatrix(angles, nrm, proj_matrix); // first version that I produced - panel oriented as z, to be used with project_volume_onto_image_c
//        computeProjMatrix(angles,nrm,proj_matrix); // second version with attempt to correct misalignments - panel oriented as y, to be used with project_volume_onto_image_c (but origin is not clearly computed..)
//        compRTKProjMatrix(angles, nrm, proj_matrix); // third version with rtk-like projection matrices



//              printf ("GEOMETRY MATRIX outside the function\n%g %g %g %g\n%g %g %g %g\n%g %g %g %g\n",
//              proj_matrix[0], proj_matrix[1],
//              proj_matrix[2], proj_matrix[3],
//              proj_matrix[4], proj_matrix[5],
//              proj_matrix[6], proj_matrix[7],
//              proj_matrix[8], proj_matrix[9],
//              proj_matrix[10], proj_matrix[11]);


        /* Arbitrary scale applied to each image */
//        scale = (float) (sqrt(3.f) / (double) num_imgs);
//        scale = scale * parms->scale;
//        scale = 1.0; // do i need this?



//            /* Apply ramp filter */
//            if (parms->filter == FDK_FILTER_TYPE_RAMP) {
//                timer->start ();
                ramp_filter(proj);
//                filter_time += timer->report ();
//            }



//        REFERENCE FDK IMPLEMENTATION:
//        project_volume_onto_image_reference (proj, &proj_matrix[0], &nrm[0]);

//       DEFAULT FDK BACK PROJECTOR:                
        project_volume_onto_image_c (proj, &proj_matrix[0]);
//          project_volume_onto_image_rtk (proj, &proj_matrix[0]);

//            backproject_time += timer->report ();



//        printf ("I/O time (total) = %g\n", io_time);
//        printf ("I/O time (per image) = %g\n", io_time / num_imgs);
//        printf ("Filter time = %g\n", filter_time);
//        printf ("Filter time (per image) = %g\n", filter_time / num_imgs);
//        printf ("Backprojection time = %g\n", backproject_time);
//        printf ("Backprojection time (per image) = %g\n",
//        backproject_time / num_imgs);

//        delete timer;

//    free(proj_matrix); // crashes everything

    return 0L;
}


void FDKbp::project_volume_onto_image_c(
    kipl::base::TImage<float, 2> &cbi,
    double *proj_matrix
)
{

//                           std::cout << "projection 1: " << std::endl;

//                           for (int j=255*100; j<255*100+255; j++) {
//                               std::cout << cbi[j] << " ";
//                           }
//                           std::cout << std::endl;




    //	printf("project_volume_onto_image_c\n");
        long int i, j, k;

        float* img = volume.GetDataPtr(); // maybe i don't need this
        double *xip, *yip, *zip;
        double sad_sid_2;

//        std::cout << spacing[0] << std::endl;



        /// spacing of the reconstructed volume. Maximum resolution for CBCT = detector pixel spacing/ magnification.
        /// magnification = SDD/SOD
//        float spacing[3] = {0.098f, 0.098f, 1.0f};

        float spacing[3];
        spacing[0] = mConfig.MatrixInfo.fVoxelSize[0];
        spacing[1] = mConfig.MatrixInfo.fVoxelSize[1];
        spacing[2] = mConfig.MatrixInfo.fVoxelSize[2];

        /// origin of the reconstructed volume. Volume isocenter is assumed in position (0,0,0)
//        float origin[3] = {-24.9512f, -24.9512f, -42.00f};
        float origin[3];


        if (mConfig.MatrixInfo.bUseVOI) {

//            origin[0] = - ((mConfig.MatrixInfo.nDims[0]-mConfig.ProjectionInfo.fCenter-1)*spacing[0]-mConfig.MatrixInfo.voi[0]*spacing[0]-spacing[0]/2);
//            origin[1] = - ((mConfig.MatrixInfo.nDims[1]-mConfig.ProjectionInfo.fCenter-1)*spacing[1]-mConfig.MatrixInfo.voi[2]*spacing[1]-spacing[1]/2);
//            origin[2] = - ((mConfig.MatrixInfo.nDims[2]-mConfig.ProjectionInfo.fpPoint[1]-11)*spacing[2]-mConfig.MatrixInfo.voi[4]*spacing[2]-spacing[2]/2);


//            origin[0] = - ((mConfig.MatrixInfo.nDims[0]/2)*spacing[0]-mConfig.MatrixInfo.voi[0]/2*spacing[0]-spacing[0]/2);
//            origin[1] = - ((mConfig.MatrixInfo.nDims[1]/2)*spacing[1]-mConfig.MatrixInfo.voi[2]/2*spacing[1]-spacing[1]/2);
//            origin[2] = - ((mConfig.MatrixInfo.nDims[2]/2)*spacing[2]-mConfig.MatrixInfo.voi[4]/2*spacing[2]-spacing[2]/2);

            origin[0] = - ((mConfig.MatrixInfo.nDims[0]-1)/2*spacing[0]-mConfig.MatrixInfo.voi[0]/2*spacing[0]); // therotical values
            origin[1] = - ((mConfig.MatrixInfo.nDims[1]-1)/2*spacing[1]-mConfig.MatrixInfo.voi[2]/2*spacing[1]);
            origin[2] = - ((mConfig.MatrixInfo.nDims[2]-1)/2*spacing[2]-mConfig.MatrixInfo.voi[4]/2*spacing[2]);


//            std::cout << "origin: " << origin[0] << " " << origin[1] << " " << origin[2] << std::endl;
//            std::cout << "voi: " << mConfig.MatrixInfo.voi[0] << " " << mConfig.MatrixInfo.voi[1] << " " << mConfig.MatrixInfo.voi[2]  << " "
//                          << mConfig.MatrixInfo.voi[3] << " " << mConfig.MatrixInfo.voi[4] << " " << mConfig.MatrixInfo.voi[5]
//                      << std::endl;
        }

        else {
            origin[0] = - ((mConfig.MatrixInfo.nDims[0])/2*spacing[0]-spacing[0]/2);
            origin[1] = - ((mConfig.MatrixInfo.nDims[1])/2*spacing[1]-spacing[1]/2);
            origin[2] = - ((mConfig.MatrixInfo.nDims[2])/2*spacing[2]-spacing[2]/2);

//            origin[0] = - ((mConfig.MatrixInfo.nDims[0]-mConfig.ProjectionInfo.fCenter-1)*spacing[0]-spacing[0]/2);
//            origin[1] = - ((mConfig.MatrixInfo.nDims[1]-mConfig.ProjectionInfo.fCenter-1)*spacing[1]-spacing[1]/2);
//            origin[2] = - ((mConfig.MatrixInfo.nDims[2]-mConfig.ProjectionInfo.fpPoint[1]-1)*spacing[2]-spacing[2]/2);
        }

//        std::cout << "un po' di debug su geometry:" << std::endl;
//        std::cout << "matrix dims: " << mConfig.MatrixInfo.nDims[0] << " " << mConfig.MatrixInfo.nDims[1] << " " << mConfig.MatrixInfo.nDims[2] << std::endl;
//        std::cout << "piercing point: " << mConfig.ProjectionInfo.fpPoint[0] << " " << mConfig.ProjectionInfo.fpPoint[1] << std::endl;
//        std::cout << "center of rotation: " << mConfig.ProjectionInfo.fCenter << std::endl;

//        std::cout << "debug origin: " << std::endl;
//        std::cout << origin[0] << " " << origin[1] << " " << origin[2] << std::endl;




//        double ic[2] = {mConfig.ProjectionInfo.fpPoint[0], mConfig.ProjectionInfo.fpPoint[1]}; //NOT OK
//        double offCenterX =  (mConfig.ProjectionInfo.fCenter-mConfig.ProjectionInfo.fpPoint[0]);
        double ic[2] = {mConfig.ProjectionInfo.fCenter, mConfig.ProjectionInfo.fpPoint[1]}; // the more correct but still I don't get all I need
//          double ic[2] = {mConfig.ProjectionInfo.fCenter, mConfig.ProjectionInfo.projection_roi[3]/2}; // that is also very curious..
//        double ic[2] = {mConfig.ProjectionInfo.fCenter, static_cast<double>(mConfig.ProjectionInfo.nDims[1]/2)}; // totalmente sbagliato
//        double ic[2] = {mConfig.ProjectionInfo.fCenter, mConfig.ProjectionInfo.nDims[1]/2-mConfig.ProjectionInfo.roi[1]}; // NOT OK

//        std::cout << ic[0] << " " << ic[1] << std::endl;



    //	printf("project_volume_onto_image_c:1\n");

        /* Rescale image (destructive rescaling) */
//        sad_sid_2 = (pmat->sad * pmat->sad) / (pmat->sid * pmat->sid);
        sad_sid_2 = (mConfig.ProjectionInfo.fSOD * mConfig.ProjectionInfo.fSOD) / (mConfig.ProjectionInfo.fSDD * mConfig.ProjectionInfo.fSDD);


//        for (i = 0; i < cbi->dim[0]*cbi->dim[1]; i++) {
//        cbi->img[i] *= sad_sid_2;	// Speedup trick re: Kachelsreiss
//        cbi->img[i] *= scale;		// User scaling
//        }

        for (i=0; i<cbi.Size(0)*cbi.Size(1); i++) {
            cbi[i] *=sad_sid_2; 	// Speedup trick re: Kachelsreiss
//            cbi[i] *=scale; // User scaling
        }

//        xip = (double*) malloc (3*vol->dim[0]*sizeof(double));
//        yip = (double*) malloc (3*vol->dim[1]*sizeof(double));
//        zip = (double*) malloc (3*vol->dim[2]*sizeof(double));

        xip = (double*) malloc (3*volume.Size(0)*sizeof(double));
        yip = (double*) malloc (3*volume.Size(1)*sizeof(double));
        zip = (double*) malloc (3*volume.Size(2)*sizeof(double));

//        /* Precompute partial projections here */
//        for (i = 0; i < vol->dim[0]; i++) {
//        double x = (double) (vol->offset[0] + i * vol->spacing[0]);
//        xip[i*3+0] = x * (pmat->matrix[0] + pmat->ic[0] * pmat->matrix[8]);
//        xip[i*3+1] = x * (pmat->matrix[4] + pmat->ic[1] * pmat->matrix[8]);
//        xip[i*3+2] = x * pmat->matrix[8];
//        }
//        for (j = 0; j < vol->dim[1]; j++) {
//        double y = (double) (vol->offset[1] + j * vol->spacing[1]);
//        yip[j*3+0] = y * (pmat->matrix[1] + pmat->ic[0] * pmat->matrix[9]);
//        yip[j*3+1] = y * (pmat->matrix[5] + pmat->ic[1] * pmat->matrix[9]);
//        yip[j*3+2] = y * pmat->matrix[9];
//        }
//        for (k = 0; k < vol->dim[2]; k++) {
//        double z = (double) (vol->offset[2] + k * vol->spacing[2]);
//        zip[k*3+0] = z * (pmat->matrix[2] + pmat->ic[0] * pmat->matrix[10])
//            + pmat->ic[0] * pmat->matrix[11] + pmat->matrix[3];
//        zip[k*3+1] = z * (pmat->matrix[6] + pmat->ic[1] * pmat->matrix[10])
//            + pmat->ic[1] * pmat->matrix[11] + pmat->matrix[7];
//        zip[k*3+2] = z * pmat->matrix[10] + pmat->matrix[11];
//        }

        /* Precompute partial projections here */
        for (i = 0; i < volume.Size(0); i++) {
            double x = (double) (origin[0] + i * spacing[0]);
            xip[i*3+0] = x * (proj_matrix[0] + ic[0] * proj_matrix[8]);
            xip[i*3+1] = x * (proj_matrix[4] + ic[1] * proj_matrix[8]);
            xip[i*3+2] = x * proj_matrix[8];
        }
        for (j = 0; j < volume.Size(1); j++) {
            double y = (double) (origin[1] + j * spacing[1]);
            yip[j*3+0] = y * (proj_matrix[1] + ic[0] * proj_matrix[9]);
            yip[j*3+1] = y * (proj_matrix[5] + ic[1] * proj_matrix[9]);
            yip[j*3+2] = y * proj_matrix[9];
        }
        for (k = 0; k < volume.Size(2); k++) {
            double z = (double) (origin[2] + k * spacing[2]);
            zip[k*3+0] = z * (proj_matrix[2] + ic[0] * proj_matrix[10])
                + ic[0] * proj_matrix[11] + proj_matrix[3];
            zip[k*3+1] = z * (proj_matrix[6] + ic[1] * proj_matrix[10])
                + ic[1] * proj_matrix[11] + proj_matrix[7];
            zip[k*3+2] = z * proj_matrix[10] + proj_matrix[11];
        }

//    //		printf("project_volume_onto_image_c:2\n");
//        /* Main loop */
//    #pragma omp parallel for
//        for (k = 0; k < vol->dim[2]; k++) {
//        plm_long p = k * vol->dim[1] * vol->dim[0];
//        plm_long j;
//        for (j = 0; j < vol->dim[1]; j++) {
//            plm_long i;
//            double acc2[3];
//            vec3_add3 (acc2, &zip[3*k], &yip[3*j]);
//            for (i = 0; i < vol->dim[0]; i++) {
//            double dw;
//            double acc3[3];
//            vec3_add3 (acc3, acc2, &xip[3*i]);
//            dw = 1 / acc3[2];
//            acc3[0] = acc3[0] * dw;
//            acc3[1] = acc3[1] * dw;
//            img[p++] +=
//                dw * dw * get_pixel_value_c (cbi, acc3[1], acc3[0]);
//            }
//        }
//        }
//        free (xip);
//        free (yip);
//        free (zip);

//        std::cout << volume.Size(0) << " " << volume.Size(1) << " " << volume.Size(2) << std::endl;

          /* Main loop */
    #pragma omp parallel for
        for (k = 0; k < volume.Size(2); k++) {
        int long p = k * volume.Size(1) * volume.Size(0);
        int long j;
        for (j = 0; j < volume.Size(1); j++) {
            int long i;
            double acc2[3];
//            static inline void vec3_add3 (double* v1, const double* v2, const double* v3) {
//                v1[0] = v2[0] + v3[0]; v1[1] = v2[1] + v3[1]; v1[2] = v2[2] + v3[2];
//            }
//            vec3_add3 (acc2, &zip[3*k], &yip[3*j]);
            acc2[0] = zip[3*k]+yip[3*j];
            acc2[1] = zip[3*k+1]+yip[3*j+1];
            acc2[2] = zip[3*k+2]+yip[3*j+2];
            // hopefully correct

            for (i = 0; i < volume.Size(0); i++) {
            double dw;
            double acc3[3];
//            vec3_add3 (acc3, acc2, &xip[3*i]);
            acc3[0] = acc2[0]+xip[3*i];
            acc3[1] = acc2[1]+xip[3*i+1];
            acc3[2] = acc2[2]+xip[3*i+2];

            dw = 1 / acc3[2];
            acc3[0] = acc3[0] * dw;
            acc3[1] = acc3[1] * dw;

//            std::cout << "p: " << p << std::endl;
            img[p++] +=
                dw * dw * get_pixel_value_c (cbi, acc3[1], acc3[0]);
            //            volume[p++] +=
            //                dw * dw * get_pixel_value_c (cbi, acc3[1], acc3[0]);
            }
        }
        }
        free (xip);
        free (yip);
        free (zip);
}

void FDKbp::project_volume_onto_image_rtk(
    kipl::base::TImage<float, 2> &cbi,
    double *proj_matrix
)
{

//                           std::cout << "projection 1: " << std::endl;

//                           for (int j=255*100; j<255*100+255; j++) {
//                               std::cout << cbi[j] << " ";
//                           }
//                           std::cout << std::endl;




    //	printf("project_volume_onto_image_c\n");
        long int i, j, k;

        float* img = volume.GetDataPtr(); // maybe i don't need this
        double *xip, *yip, *zip;
        double sad_sid_2;

//        std::cout << spacing[0] << std::endl;



        /// spacing of the reconstructed volume. Maximum resolution for CBCT = detector pixel spacing/ magnification.
        /// magnification = SDD/SOD
//        float spacing[3] = {0.098f, 0.098f, 1.0f};

        float spacing[3];
        spacing[0] = mConfig.MatrixInfo.fVoxelSize[0];
        spacing[1] = mConfig.MatrixInfo.fVoxelSize[1];
        spacing[2] = mConfig.MatrixInfo.fVoxelSize[2];

        /// origin of the reconstructed volume. Volume isocenter is assumed in position (0,0,0)
//        float origin[3] = {-24.9512f, -24.9512f, -42.00f};
        float origin[3];


        if (mConfig.MatrixInfo.bUseVOI) {

//            origin[0] = - ((mConfig.MatrixInfo.nDims[0]-mConfig.ProjectionInfo.fCenter-1)*spacing[0]-mConfig.MatrixInfo.voi[0]*spacing[0]-spacing[0]/2);
//            origin[1] = - ((mConfig.MatrixInfo.nDims[1]/2-1)*spacing[1]-mConfig.MatrixInfo.voi[2]*spacing[1]-spacing[1]/2);
//            origin[2] = - ((mConfig.MatrixInfo.nDims[2]-mConfig.ProjectionInfo.fCenter-1)*spacing[2]-mConfig.MatrixInfo.voi[4]*spacing[2]-spacing[2]/2);


            origin[0] = - ((mConfig.MatrixInfo.nDims[0]/2)*spacing[0]-mConfig.MatrixInfo.voi[0]/2*spacing[0]-spacing[0]/2); // let's try with the ideal case
            origin[1] = - ((mConfig.MatrixInfo.nDims[1]/2)*spacing[1]-mConfig.MatrixInfo.voi[2]/2*spacing[1]-spacing[1]/2);
            origin[2] = - ((mConfig.MatrixInfo.nDims[2]/2)*spacing[2]-mConfig.MatrixInfo.voi[4]/2*spacing[2]-spacing[2]/2);

//            std::cout << "origin: " << origin[0] << " " << origin[1] << " " << origin[2] << std::endl;
//            std::cout << "voi: " << mConfig.MatrixInfo.voi[0] << " " << mConfig.MatrixInfo.voi[1] << " " << mConfig.MatrixInfo.voi[2]  << " "
//                          << mConfig.MatrixInfo.voi[3] << " " << mConfig.MatrixInfo.voi[4] << " " << mConfig.MatrixInfo.voi[5]
//                      << std::endl;
        }

        else {


//            origin[0] = - ((mConfig.MatrixInfo.nDims[0]-mConfig.ProjectionInfo.fCenter-1)*spacing[0]-spacing[0]/2);
//            origin[1] = - ((mConfig.MatrixInfo.nDims[1]-mConfig.ProjectionInfo.fCenter-1)*spacing[1]-spacing[1]/2);
//            origin[2] = - ((mConfig.MatrixInfo.nDims[2]-mConfig.ProjectionInfo.fpPoint[1]-1)*spacing[2]-spacing[2]/2);

                        origin[0] = - ((mConfig.MatrixInfo.nDims[0])/2*spacing[0]-spacing[0]/2);
                        origin[1] = - ((mConfig.MatrixInfo.nDims[1])/2*spacing[1]-spacing[1]/2);
                        origin[2] = - ((mConfig.MatrixInfo.nDims[2])/2*spacing[2]-spacing[2]/2);
        }

//        std::cout << "un po' di debug su geometry:" << std::endl;
//        std::cout << "matrix dims: " << mConfig.MatrixInfo.nDims[0] << " " << mConfig.MatrixInfo.nDims[1] << " " << mConfig.MatrixInfo.nDims[2] << std::endl;
//        std::cout << "piercing point: " << mConfig.ProjectionInfo.fpPoint[0] << " " << mConfig.ProjectionInfo.fpPoint[1] << std::endl;
//        std::cout << "center of rotation: " << mConfig.ProjectionInfo.fCenter << std::endl;

//        std::cout << "debug origin: " << std::endl;
//        std::cout << origin[0] << " " << origin[1] << " " << origin[2] << std::endl;




//        double ic[2] = {mConfig.ProjectionInfo.fpPoint[0], mConfig.ProjectionInfo.fpPoint[1]}; //NOT OK
//        double ic[2] = {mConfig.ProjectionInfo.fCenter, mConfig.ProjectionInfo.fpPoint[1]}; // SEEMS THE MOST CORRECT
//        double ic[2] = {mConfig.ProjectionInfo.fCenter, mConfig.ProjectionInfo.nDims[1]/2-mConfig.ProjectionInfo.roi[1]}; // NOT OK
        double ic[2] = {static_cast<double>(mConfig.ProjectionInfo.nDims[0]/2-mConfig.ProjectionInfo.roi[0]), static_cast<double>(mConfig.ProjectionInfo.nDims[1]/2-mConfig.ProjectionInfo.roi[1])}; // center of the detector with respect of the projectionroi

//        std::cout << ic[0] << " " << ic[1] << std::endl;



    //	printf("project_volume_onto_image_c:1\n");

        /* Rescale image (destructive rescaling) */
//        sad_sid_2 = (pmat->sad * pmat->sad) / (pmat->sid * pmat->sid);
        sad_sid_2 = (mConfig.ProjectionInfo.fSOD * mConfig.ProjectionInfo.fSOD) / (mConfig.ProjectionInfo.fSDD * mConfig.ProjectionInfo.fSDD);


//        for (i = 0; i < cbi->dim[0]*cbi->dim[1]; i++) {
//        cbi->img[i] *= sad_sid_2;	// Speedup trick re: Kachelsreiss
//        cbi->img[i] *= scale;		// User scaling
//        }

        for (i=0; i<cbi.Size(0)*cbi.Size(1); i++) {
            cbi[i] *=sad_sid_2; 	// Speedup trick re: Kachelsreiss
//            cbi[i] *=scale; // User scaling
        }

//        xip = (double*) malloc (3*vol->dim[0]*sizeof(double));
//        yip = (double*) malloc (3*vol->dim[1]*sizeof(double));
//        zip = (double*) malloc (3*vol->dim[2]*sizeof(double));

        xip = (double*) malloc (3*volume.Size(0)*sizeof(double));
        yip = (double*) malloc (3*volume.Size(1)*sizeof(double));
        zip = (double*) malloc (3*volume.Size(2)*sizeof(double));

//        /* Precompute partial projections here */
//        for (i = 0; i < vol->dim[0]; i++) {
//        double x = (double) (vol->offset[0] + i * vol->spacing[0]);
//        xip[i*3+0] = x * (pmat->matrix[0] + pmat->ic[0] * pmat->matrix[8]);
//        xip[i*3+1] = x * (pmat->matrix[4] + pmat->ic[1] * pmat->matrix[8]);
//        xip[i*3+2] = x * pmat->matrix[8];
//        }
//        for (j = 0; j < vol->dim[1]; j++) {
//        double y = (double) (vol->offset[1] + j * vol->spacing[1]);
//        yip[j*3+0] = y * (pmat->matrix[1] + pmat->ic[0] * pmat->matrix[9]);
//        yip[j*3+1] = y * (pmat->matrix[5] + pmat->ic[1] * pmat->matrix[9]);
//        yip[j*3+2] = y * pmat->matrix[9];
//        }
//        for (k = 0; k < vol->dim[2]; k++) {
//        double z = (double) (vol->offset[2] + k * vol->spacing[2]);
//        zip[k*3+0] = z * (pmat->matrix[2] + pmat->ic[0] * pmat->matrix[10])
//            + pmat->ic[0] * pmat->matrix[11] + pmat->matrix[3];
//        zip[k*3+1] = z * (pmat->matrix[6] + pmat->ic[1] * pmat->matrix[10])
//            + pmat->ic[1] * pmat->matrix[11] + pmat->matrix[7];
//        zip[k*3+2] = z * pmat->matrix[10] + pmat->matrix[11];
//        }

        /* Precompute partial projections here */
        for (i = 0; i < volume.Size(0); i++) {
            double x = (double) (origin[0] + i * spacing[0]);
            xip[i*3+0] = x * (proj_matrix[0] + ic[0] * proj_matrix[8]);
            xip[i*3+1] = x * (proj_matrix[4] + ic[1] * proj_matrix[8]);
            xip[i*3+2] = x * proj_matrix[8];
        }
        for (j = 0; j < volume.Size(1); j++) {
            double y = (double) (origin[1] + j * spacing[1]);
            yip[j*3+0] = y * (proj_matrix[1] + ic[0] * proj_matrix[9]);
            yip[j*3+1] = y * (proj_matrix[5] + ic[1] * proj_matrix[9]);
            yip[j*3+2] = y * proj_matrix[9];
        }
        for (k = 0; k < volume.Size(2); k++) {
            double z = (double) (origin[2] + k * spacing[2]);
            zip[k*3+0] = z * (proj_matrix[2] + ic[0] * proj_matrix[10])
                + ic[0] * proj_matrix[11] + proj_matrix[3];
            zip[k*3+1] = z * (proj_matrix[6] + ic[1] * proj_matrix[10])
                + ic[1] * proj_matrix[11] + proj_matrix[7];
            zip[k*3+2] = z * proj_matrix[10] + proj_matrix[11];
        }

//    //		printf("project_volume_onto_image_c:2\n");
//        /* Main loop */
//    #pragma omp parallel for
//        for (k = 0; k < vol->dim[2]; k++) {
//        plm_long p = k * vol->dim[1] * vol->dim[0];
//        plm_long j;
//        for (j = 0; j < vol->dim[1]; j++) {
//            plm_long i;
//            double acc2[3];
//            vec3_add3 (acc2, &zip[3*k], &yip[3*j]);
//            for (i = 0; i < vol->dim[0]; i++) {
//            double dw;
//            double acc3[3];
//            vec3_add3 (acc3, acc2, &xip[3*i]);
//            dw = 1 / acc3[2];
//            acc3[0] = acc3[0] * dw;
//            acc3[1] = acc3[1] * dw;
//            img[p++] +=
//                dw * dw * get_pixel_value_c (cbi, acc3[1], acc3[0]);
//            }
//        }
//        }
//        free (xip);
//        free (yip);
//        free (zip);

//        std::cout << volume.Size(0) << " " << volume.Size(1) << " " << volume.Size(2) << std::endl;

          /* Main loop */
    #pragma omp parallel for
        for (k = 0; k < volume.Size(2); k++) {
        int long p = k * volume.Size(1) * volume.Size(0);
        int long j;
        for (j = 0; j < volume.Size(1); j++) {
            int long i;
            double acc2[3];
//            static inline void vec3_add3 (double* v1, const double* v2, const double* v3) {
//                v1[0] = v2[0] + v3[0]; v1[1] = v2[1] + v3[1]; v1[2] = v2[2] + v3[2];
//            }
//            vec3_add3 (acc2, &zip[3*k], &yip[3*j]);
            acc2[0] = zip[3*k]+yip[3*j];
            acc2[1] = zip[3*k+1]+yip[3*j+1];
            acc2[2] = zip[3*k+2]+yip[3*j+2];
            // hopefully correct

            for (i = 0; i < volume.Size(0); i++) {
            double dw;
            double acc3[3];
//            vec3_add3 (acc3, acc2, &xip[3*i]);
            acc3[0] = acc2[0]+xip[3*i];
            acc3[1] = acc2[1]+xip[3*i+1];
            acc3[2] = acc2[2]+xip[3*i+2];

            dw = 1 / acc3[2];
            acc3[0] = acc3[0] * dw;
            acc3[1] = acc3[1] * dw;

//            std::cout << "p: " << p << std::endl;
            img[p++] +=
                dw * dw * get_pixel_value_c (cbi, acc3[1], acc3[0]);
            //            volume[p++] +=
            //                dw * dw * get_pixel_value_c (cbi, acc3[1], acc3[0]);
            }
        }
        }
        free (xip);
        free (yip);
        free (zip);
}

/* Reference implementation is the most straightforward implementation,
    also it is the slowest */
void FDKbp::project_volume_onto_image_reference (
    kipl::base::TImage<float, 2> &cbi,
    double *proj_matrix,
    double *nrm
)
{
    int i, j, k, p;
    double vp[4];   /* vp = voxel position */
//    float* img = (float*) vol->img; // voxel data of the reconstructed volume -> è il mio volume.
    float *img = volume.GetDataPtr(); // do i need this?

//    std::cout << "ciao proj matrix: " << proj_matrix[8] << std::endl;


    const size_t SizeZ  = volume.Size(0)>>2;
    __m128 column[2048]; // not used for now..

//    std::cout << "Volume Size: ";
//    std::cout << volume.Size(0) << " " << volume.Size(1) << " " << volume.Size(2) << std::endl;
//    std::cout << SizeZ << std::endl;



//    Proj_matrix *pmat = cbi->pmat; /* projection matrix 3D -> 2D in homogenous coordinates */
//    // projection matrices.. to be added in fdkreconbase (probably)

    p = 0;
    vp[3] = 1.0;
    /* Loop k (slices), j (rows), i (cols) */


//    float spacing[3] = {mConfig.ProjectionInfo.fResolution[0], mConfig.ProjectionInfo.fResolution[0],mConfig.ProjectionInfo.fResolution[0]};
     double ic[2] = {mConfig.ProjectionInfo.fpPoint[0], mConfig.ProjectionInfo.fpPoint[1]};
//    double ic[2] = {mConfig.ProjectionInfo.fCenter, mConfig.ProjectionInfo.fpPoint[1]};

//    std::cout << "Debug on projection cb:" << std::endl;
//    std::cout << cbi.Size(0) << " " << cbi.Size(1) << std::endl;
//    std::cout << "Debug on piercing point: " << std::endl;
//    std::cout << ic[0] << " " << ic[1] << std::endl;
//    std::cout << "Debug on projecton roi: " << std:: endl;
//    std::cout << mConfig.ProjectionInfo.roi[0] << " " << mConfig.ProjectionInfo.roi[1] << " " << mConfig.ProjectionInfo.roi[2]
//                                                  << " " << mConfig.ProjectionInfo.roi[3] << std::endl;


  //  float spacing[3] = {0.098f, 0.098f, 1.0f};

    float spacing[3];
    spacing[0] = mConfig.MatrixInfo.fVoxelSize[0];
    spacing[1] = mConfig.MatrixInfo.fVoxelSize[1];
    spacing[2] = mConfig.MatrixInfo.fVoxelSize[2];

    float origin[3]; /* = {-12.475, -12.75, -24.975};*/

    if (mConfig.MatrixInfo.bUseVOI) {
        origin[0] = - ((mConfig.MatrixInfo.nDims[0]-1)/2*spacing[0]-mConfig.MatrixInfo.voi[0]*spacing[0]-spacing[0]/2);
        origin[1] = - ((mConfig.MatrixInfo.nDims[1]-1)/2*spacing[1]-mConfig.MatrixInfo.voi[2]*spacing[1]-spacing[1]/2);
        origin[2] = - ((mConfig.MatrixInfo.nDims[2]-1)/2*spacing[2]-mConfig.MatrixInfo.voi[4]*spacing[2]-spacing[2]/2);
    }

    else {
        origin[0] = - ((mConfig.MatrixInfo.nDims[0]-1)/2*spacing[0]-spacing[0]/2);
        origin[1] = - ((mConfig.MatrixInfo.nDims[1]-1)/2*spacing[1]-spacing[1]/2);
        origin[2] = - ((mConfig.MatrixInfo.nDims[2]-1)/2*spacing[2]-spacing[2]/2);
    }



    for (k = 0; k < volume.Size(2); k++) {

//        vp[2] = (double) (vol->origin[2] + k * vol->spacing[2]);
        vp[2] = (double) (origin[2] + k * spacing[2]);
//        std::cout<< "k: " << k << std::endl;

    for (j = 0; j < volume.Size(1); j++) {
        vp[1] = (double) (origin[1] + j * spacing[1]);
//        std::cout << "j: " << j << std::endl;

        for (i = 0; i < volume.Size(0); i++) {

//            std::cout << "i: " << i << std::endl;
            double ip[3];        /* ip = image position */
            double s;            /* s = projection of vp onto s axis */
            vp[0] = (double) (origin[0] + i * spacing[0]);
//            mat43_mult_vec3 (ip, pmat->matrix, vp);  /* ip = matrix * vp */
            /* ip = matrix * vp */
            ip[0] = proj_matrix[0]*vp[0]+proj_matrix[1]*vp[1]+proj_matrix[2]*vp[2]+proj_matrix[3]*vp[3];
            ip[1] = proj_matrix[4]*vp[0]+proj_matrix[5]*vp[1]+proj_matrix[6]*vp[2]+proj_matrix[7]*vp[3];
            ip[2] = proj_matrix[8]*vp[0]+proj_matrix[9]*vp[1]+proj_matrix[10]*vp[2]+proj_matrix[11]*vp[3];

//            ip[0] = pmat->ic[0] + ip[0] / ip[2];
//            ip[1] = pmat->ic[1] + ip[1] / ip[2];
                    ip[0] = ic[0] + ip[0] / ip[2];
                    ip[1] = ic[1] + ip[1] / ip[2];

            /* Distance on central axis from voxel center to source */
            /* pmat->nrm is normal of panel */

//            s = vec3_dot (pmat->nrm, vp);

            s = nrm[0]*vp[0]+nrm[1]*vp[1]+nrm[2]*vp[2];

            /* Conebeam weighting factor */
            s = mConfig.ProjectionInfo.fSOD - s;
            s = mConfig.ProjectionInfo.fSOD * mConfig.ProjectionInfo.fSOD / (s * s);


//            volume[p++] += scale * s * get_pixel_value_b (cbi, ip[1], ip[0]);

            img[p++] += s * get_pixel_value_b (cbi, ip[1], ip[0]);
        }


    }
    }

}

/* Nearest neighbor interpolation of intensity value on image */
 float FDKbp::get_pixel_value_b (kipl::base::TImage<float, 2> &cbi, double r, double c)
{
    int rr, cc;

    rr = (int)(r);
//    if (rr < 0 || rr >= cbi->dim[1]) return 0.0;
    if (rr < 0 || rr >= cbi.Size(1)) return 0.0;
    cc = (int)(c);
//    if (cc < 0 || cc >= cbi->dim[0]) return 0.0;
    if (cc < 0 || cc >= cbi.Size(0)) return 0.0;
    return cbi[rr*cbi.Size(0) + cc];
}


 /* get_pixel_value_c seems to be no faster than get_pixel_value_b,
    despite having two fewer compares. */
float FDKbp::get_pixel_value_c (kipl::base::TImage<float,2> &cbi, double r, double c)
 {
     int rr, cc;

// #if defined (commentout)
//     r += 0.5;
//     if (r < 0) return 0.0;
//     rr = (int) r;
//     if (rr >= cbi->dim[1]) return 0.0;

//     c += 0.5;
//     if (c < 0) return 0.0;
//     cc = (int) c;
//     if (cc >= cbi->dim[0]) return 0.0;
// #endif

//     r += 0.5;
//     if (r < 0) return 0.0;
//     if (r >= (double) cbi->dim[1]) return 0.0;
//     rr = (int) r;

//     c += 0.5;
//     if (c < 0) return 0.0;
//     if (c >= (double) cbi->dim[0]) return 0.0;
//     cc = (int) c;

//     return cbi->img[rr*cbi->dim[0] + cc];

     r += 0.5;
     if (r < 0) return 0.0;
     if (r >= (double) cbi.Size(1)) return 0.0;
     rr = (int) r;

     c += 0.5;
     if (c < 0) return 0.0;
     if (c >= (double) cbi.Size(0)) return 0.0;
     cc = (int) c;

//     std::cout << cbi[rr*cbi.Size(0) + cc] << std::endl;

     return cbi[rr*cbi.Size(0) + cc];

 }

//! In-place ramp filter for greyscale images
//! \param data The pixel data of the image
//! \param width The width of the image
//! \param height The height of the image
//! \template_param T The type of pixel in the image
//void
//ramp_filter (
//    float *data,
//    unsigned int width,
//    unsigned int height
//)


void FDKbp::ramp_filter(kipl::base::TImage<float, 2> &img){

    unsigned int i, r, c;
    unsigned int N;

    unsigned int width = img.Size(0);
    unsigned int height = img.Size(1);

    fftw_complex *in;
    fftw_complex *fft;
    fftw_complex *ifft;
    fftw_plan fftp;
    fftw_plan ifftp;
    double *ramp;
    ramp = (double*) malloc (width * sizeof(double));

    if (!ramp) {
        printf ("Error allocating memory for ramp\n"); // to substitute with an exception
    }

    N = width * height;
    in = (fftw_complex*) fftw_malloc (sizeof(fftw_complex) * N);
    fft = (fftw_complex*) fftw_malloc (sizeof(fftw_complex) * N);
    ifft = (fftw_complex*) fftw_malloc (sizeof(fftw_complex) * N);

    if (!in || !fft || !ifft) {
        printf("Error allocating memory for fft\n");
    }

    float *data = img.GetDataPtr();

    for (r = 0; r < MARGIN; ++r)
        memcpy (data + r * width, data + MARGIN * width,
        width * sizeof(float));

    for (r = height - MARGIN; r < height; ++r)
        memcpy (data + r * width, data + (height - MARGIN - 1) * width,
        width * sizeof(float));

    for (r = 0; r < height; ++r) {
        for (c = 0; c < MARGIN; ++c)
            data[r * width + c] = data[r * width + MARGIN];
        for (c = width - MARGIN; c < width; ++c)
            data[r * width + c] = data[r * width + width - MARGIN - 1];
    }

    /* Fill in ramp filter in frequency space */
    for (i = 0; i < N; ++i) {
        in[i][0] = (double)(data[i]);
        // comment from here, check what happens here to understand the attenuation coefficients....
//        in[i][0] /= 65535;
//        in[i][0] = (in[i][0] == 0 ? 1 : in[i][0]);
//        in[i][0] = -log (in[i][0]);
        // to here, to erase -log operation on projections
        in[i][1] = 0.0;
    }

    /* Add padding */
    for (i = 0; i < width / 2; ++i)
        ramp[i] = i;

    for (i = width / 2; i < (unsigned int) width; ++i)
        ramp[i] = width - i;

    /* Roll off ramp filter */
    for (i = 0; i < width; ++i)
        ramp[i] *= (cos (i * DEGTORAD * 360 / width) + 1) / 2;

    for (r = 0; r < height; ++r)
    {
    fftp = fftw_plan_dft_1d (width, in + r * width, fft + r * width,
        FFTW_FORWARD, FFTW_ESTIMATE);
    if (!fftp) {
        printf ("Error creating fft plan\n");
    }
    ifftp = fftw_plan_dft_1d (width, fft + r * width, ifft + r * width,
        FFTW_BACKWARD, FFTW_ESTIMATE);
    if (!ifftp) {
        printf ("Error creating ifft plan\n");
    }

        fftw_execute (fftp);

        // Apply ramp
        for (c = 0; c < width; ++c) {
            fft[r * width + c][0] *= ramp[c];
            fft[r * width + c][1] *= ramp[c];
        }

        // Add apodization

        fftw_execute (ifftp);

    fftw_destroy_plan (fftp);
    fftw_destroy_plan (ifftp);
    }

    for (i = 0; i < N; ++i)
        ifft[i][0] /= width;

    for (i = 0; i < N; ++i)
        data[i] = (float)(ifft[i][0]);

    fftw_free (in);
    fftw_free (fft);
    fftw_free (ifft);
    free (ramp);


}



