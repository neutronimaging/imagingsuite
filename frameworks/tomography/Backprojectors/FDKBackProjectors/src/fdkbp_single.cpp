//<LICENSE>

#include <vector>
#include <sstream>
#include <string>
#include <iostream>
#include <cmath>
#include <math.h>

#ifdef __aarch64__
    	#pragma clang diagnostic push
	#pragma clang diagnostic ignored "-Wold-style-cast"
	#pragma clang diagnostic ignored "-Wcast-align"
	#pragma clang diagnostic ignored "-Wpedantic"
	#pragma clang diagnostic ignored "-W#warnings"
		#include <sse2neon/sse2neon.h>
	#pragma clang diagnostic pop
#else
    #include <xmmintrin.h>
    #include <emmintrin.h>
#endif

#include <base/timage.h>
#include <base/tpermuteimage.h>
#include <math/mathconstants.h>
#include <interactors/interactionbase.h>
#include <ReconException.h>

#include "fdkbp_single.h"

#ifndef DEGTORADF
static const float DEGTORADF = fPi/ 180.0f;
#endif

#ifndef MARGINF
static const unsigned int MARGINF = 5;
#else
#error "MARGINF IS DEFINED"
#endif


// Matrix element m[i,j] for matrix with c columns
#define m_idx(m1,c,i,j) m1[i*c+j]


FDKbp_single::FDKbp_single(kipl::interactors::InteractionBase *interactor) :
    FdkReconBase("muhrec","FDKbp_single",BackProjectorModuleBase::MatrixXYZ,interactor)
{
    publications.push_back(Publication(std::vector<std::string>({"L. A. Feldkamp","L. C. Davis","J. W. Kress"}),
                                       "Practical cone-beam algorithm",
                                       "J. Opt. Soc. Am. A",
                                       1984,
                                       1,
                                       6,
                                       "612-619",
                                       "10.1364/JOSAA.1.000612"));

    publications.push_back(Publication(std::vector<std::string>({"Gregory Sharp", "Nagarajan Kandasamy", "Harman Singh", "Michael Folkert"}),
                                       "GPU-based streaming architectures for fast cone-beam CT image reconstruction and demons deformable registration",
                                       "Physics in Medicine and Biology",
                                       2007,
                                       52,
                                       19,
                                       "5771-5783",
                                       "10.1088/0031-9155/52/19/003"));

}

FDKbp_single::~FDKbp_single()
{


}

int FDKbp_single::Initialize()
{
    return 0;
}

void FDKbp_single::multiplyMatrix(float *mat1, float *mat2, float *result, int rows, int columns, int columns1)
{
    int i,j,k;

    for (i = 0; i < rows; ++i)
    {
        for (j = 0; j < columns; ++j)
        {
            float acc = 0.0f;
            for (k = 0; k < columns1; ++k)
            {
                acc += m_idx(mat1,columns1,i,k) * m_idx(mat2,columns,k,j);
            }
            m_idx(result,columns,i,j) = acc;
        }
    }
}

void FDKbp_single::getProjMatrix(float angles, float *nrm, float *proj_matrix)
{
    // compute geometry matrix: detector oriented as the z axis (in case of no tilt), source perpendicular to x

    float extrinsic[16] = {0.0f, 0.0f, 0.0f, 0.0f,
                           0.0f, 0.0f, 0.0f, 0.0f,
                           0.0f, 0.0f, 0.0f, 0.0f,
                           0.0f, 0.0f, 0.0f, 0.0f}; // Extrinsic matrix
    float intrinsic[12] = {0.0f, 0.0f, 0.0f, 0.0f,
                           0.0f, 0.0f, 0.0f, 0.0f,
                           0.0f, 0.0f, 0.0f, 0.0f}; // Intrinsic matrix

    float sad = mConfig.ProjectionInfo.fSOD; ; // Distance: source to axis = source to object distance
    float sid = mConfig.ProjectionInfo.fSDD; // Distance: source to image = source to detector distance


    float offCenter = (mConfig.ProjectionInfo.fpPoint[0]-mConfig.ProjectionInfo.roi[0]-mConfig.ProjectionInfo.fCenter)*mConfig.MatrixInfo.fVoxelSize[0]; // in world coordinate
    float cam[3]    = {0.0f, 0.0f, 0.0f}; // Location of Camera

    if (mConfig.ProjectionInfo.eDirection == kipl::base::RotationDirCW)
    {
        cam[0] = sad*cos(-angles*fPi/180.0f);
        cam[1] = sad*sin(-angles*fPi/180.0f);
    }
    else
    {
        cam[0] = sad*cos(angles*fPi/180.0f);
        cam[1] = sad*sin(angles*fPi/180.0f);
    }

    float plt[3] = {0.0f, 0.0f, 0.0f}; // Detector orientation  Panel left (toward first column)
    float pup[3] = {0.0f, 0.0f, 0.0f}; // Panel up (toward top row)
    float vup[3] = {0.0f, 0.0f, 1.0f};

    float tgt[3] = {0.0f, 0.0f, 0.0f};

    //    Compute imager coordinate sys (nrm,pup,plt)
    //       ---------------
    //       nrm = cam - tgt
    //       plt = nrm x vup
    //       pup = plt x nrm
    //       ---------------


    nrm[0] = cam[0]-tgt[0];
    nrm[1] = cam[1]-tgt[1];
    nrm[2] = cam[2]-tgt[2];

    float norm_nrm = sqrt(nrm[0]*nrm[0]+nrm[1]*nrm[1]+nrm[2]*nrm[2]);

    nrm[0] /= norm_nrm;
    nrm[1] /= norm_nrm;
    nrm[2] /= norm_nrm;

    //            vec3_cross (plt, nrm, vup); // cross product
    //            vec3_normalize1 (plt);

    plt[0] =  nrm[1]*vup[2]-nrm[2]*vup[1];
    plt[1] =  nrm[2]*vup[0]-nrm[0]*vup[2];
    plt[2] =  nrm[0]*vup[1]-nrm[1]*vup[0];

    float norm_plt = sqrt(plt[0]*plt[0]+plt[1]*plt[1]+plt[2]*plt[2]);

    plt[0] /= norm_plt;
    plt[1] /= norm_plt;
    plt[2] /= norm_plt;

    //        vec3_cross (pup, plt, nrm);
    //        vec3_normalize1 (pup);

    pup[0] =  plt[1]*nrm[2]-plt[2]*nrm[1];
    pup[1] =  plt[2]*nrm[0]-plt[0]*nrm[2];
    pup[2] =  plt[0]*nrm[1]-plt[1]*nrm[0];

    float norm_pup = sqrt(pup[0]*pup[0]+pup[1]*pup[1]+pup[2]*pup[2]);

    pup[0] /= norm_pup;
    pup[1] /= norm_pup;
    pup[2] /= norm_pup;

    // Build extrinsic matrix - rotation part

    //        vec3_invert (&pmat->extrinsic[0]);
    //        vec3_invert (&pmat->extrinsic[4]);
    //        vec3_invert (&pmat->extrinsic[8]);

    extrinsic[0] = -1.0f*plt[0];
    extrinsic[1] = -1.0f*plt[1];
    extrinsic[2] = -1.0f*plt[2];

    extrinsic[4] = -1.0f*pup[0];
    extrinsic[5] = -1.0f*pup[1];
    extrinsic[6] = -1.0f*pup[2];

    extrinsic[8] = -1.0f*nrm[0];
    extrinsic[9] = -1.0f*nrm[1];
    extrinsic[10] = -1.0f*nrm[2];

    extrinsic[15] = 1.0f;



    //  ----------------- Build extrinsic matrix - translation part

    //        pmat->extrinsic[3] = vec3_dot (plt, tgt);
    //        pmat->extrinsic[7] = vec3_dot (pup, tgt);
    //        pmat->extrinsic[11] = vec3_dot (nrm, tgt) + pmat->sad;

    extrinsic[3]  = plt[0]*tgt[0]+plt[1]*tgt[1]+plt[2]*tgt[2]-offCenter;
    extrinsic[7]  = pup[0]*tgt[0]+pup[1]*tgt[1]+pup[2]*tgt[2];
    extrinsic[11] = nrm[0]*tgt[0]+nrm[1]*tgt[1]+nrm[2]*tgt[2]+sad;




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


    // -------------- Build intrinsic matrix


    //        m_idx (pmat->intrinsic,cols,0,0) = 1 / ps[0];
    //        m_idx (pmat->intrinsic,cols,1,1) = 1 / ps[1];
    //        m_idx (pmat->intrinsic,cols,2,2) = 1 / sid;
    intrinsic[0]  = 1.0f/mConfig.ProjectionInfo.fResolution[0];
    intrinsic[5]  = 1.0f/mConfig.ProjectionInfo.fResolution[1];
    intrinsic[10] = 1.0f/sid;

    //        printf ("INTRINSIC\n%g %g %g %g\n%g %g %g %g\n%g %g %g %g\n",
    //        intrinsic[0], intrinsic[1],
    //        intrinsic[2], intrinsic[3],
    //        intrinsic[4], intrinsic[5],
    //        intrinsic[6], intrinsic[7],
    //        intrinsic[8], intrinsic[9],
    //        intrinsic[10], intrinsic[11]);



    // -------  Build Projection Geometry Matrix = intrinsic * extrinsic
    int i,j,k;

//#pragma omp parallel for
    for (i = 0; i < 3; ++i)
    {
        for (j = 0; j < 4; ++j)
        {
            float acc = 0.0; // why not float?
            for (k = 0; k < 4; k++)
            {
                acc += m_idx(intrinsic,4,i,k) * m_idx(extrinsic,4,k,j);
            }
            m_idx(proj_matrix,4,i,j) = acc;
        }
    }
}


/// Main reconstruction loop - Loop over images, and backproject each image into the volume.
size_t FDKbp_single::reconstruct(kipl::base::TImage<float,2> &proj, float angles, size_t nProj)
{
    // missing things todo:
    // 1. use circular mask
    // 2. check parallel cycles

    float nrm[3]          = {0.0f, 0.0f, 0.0f};
    float proj_matrix[12] = {0.0f,0.0f,0.0f,0.0f,
                             0.0f,0.0f,0.0f,0.0f,
                             0.0f,0.0f,0.0f,0.0f}; // Projection matrix

    getProjMatrix(angles, nrm, proj_matrix); // panel oriented as z, to be used with project_volume_onto_image_c

    logger.verbose("Starting filter");
    filter.process(proj);
    logger.verbose("Filtering done");

    //        REFERENCE FDK IMPLEMENTATION:
    //        project_volume_onto_image_reference (proj, &proj_matrix[0], &nrm[0]);

    //       DEFAULT FDK BACK PROJECTOR:
//    project_volume_onto_image_c (proj, &proj_matrix[0], nProj);
    project_volume_onto_image_c (proj, proj_matrix, nProj);
    return 0L;
}


void FDKbp_single::project_volume_onto_image_c(kipl::base::TImage<float, 2> &cbi,
    float *proj_matrix, size_t nProj)
{
    logger.debug("Started FDK back-projector");

    long int i, j, k;

    float* img = cbct_volume.GetDataPtr();

    float sad_sid_2;
    float scale = mConfig.ProjectionInfo.fSDD/mConfig.ProjectionInfo.fSOD; // compensate for resolution that is already included in weights

    // spacing of the reconstructed volume. Maximum resolution for CBCT = detector pixel spacing/ magnification.
    // magnification = SDD/SOD

    float spacing[3]; // detector pixel spacing divided by the magnification
    spacing[0] = mConfig.MatrixInfo.fVoxelSize[0];
    spacing[1] = mConfig.MatrixInfo.fVoxelSize[1];
    spacing[2] = mConfig.MatrixInfo.fVoxelSize[2];


    float origin[3];
    float U = static_cast<float>(mConfig.ProjectionInfo.roi[2]-mConfig.ProjectionInfo.roi[0]);
    float V = static_cast<float>(mConfig.ProjectionInfo.roi[3]-mConfig.ProjectionInfo.roi[1]);

    origin[0] = -(U-mConfig.ProjectionInfo.fCenter)*spacing[0]-spacing[0]/2;
    origin[1] = -(U-mConfig.ProjectionInfo.fCenter)*spacing[1]-spacing[1]/2;
    origin[2] = -(V-(mConfig.ProjectionInfo.fpPoint[1]-mConfig.ProjectionInfo.roi[1]))*spacing[2]-spacing[2]/2;

    float radius = static_cast<float>(volume.Size(1))*mConfig.MatrixInfo.fVoxelSize[0]/2;

    size_t CBCT_roi[] ={0UL,0UL,0UL,0UL};
    CBCT_roi[0] = mConfig.ProjectionInfo.roi[0];
    CBCT_roi[2] = mConfig.ProjectionInfo.roi[2];

    if (   mConfig.ProjectionInfo.fpPoint[1] >= static_cast<float>(mConfig.ProjectionInfo.roi[1])
        && mConfig.ProjectionInfo.fpPoint[1] >= static_cast<float>(mConfig.ProjectionInfo.roi[3]))
    {
        CBCT_roi[3] = static_cast<size_t>(mConfig.ProjectionInfo.fpPoint[1]-((mConfig.ProjectionInfo.fpPoint[1]-static_cast<float>(mConfig.ProjectionInfo.roi[3]))*mConfig.MatrixInfo.fVoxelSize[0]*mConfig.ProjectionInfo.fSDD/(mConfig.ProjectionInfo.fSOD+radius))/mConfig.ProjectionInfo.fResolution[0]);
        float value = mConfig.ProjectionInfo.fpPoint[1]-((mConfig.ProjectionInfo.fpPoint[1]-static_cast<float>(mConfig.ProjectionInfo.roi[1]))*mConfig.MatrixInfo.fVoxelSize[0]*mConfig.ProjectionInfo.fSDD/(mConfig.ProjectionInfo.fSOD-radius))/mConfig.ProjectionInfo.fResolution[0];
        if(value<=0)
            CBCT_roi[1] = 0;
        else
            CBCT_roi[1] = static_cast<size_t>(mConfig.ProjectionInfo.fpPoint[1]-((mConfig.ProjectionInfo.fpPoint[1]-static_cast<float>(mConfig.ProjectionInfo.roi[1]))*mConfig.MatrixInfo.fVoxelSize[0]*mConfig.ProjectionInfo.fSDD/(mConfig.ProjectionInfo.fSOD-radius))/mConfig.ProjectionInfo.fResolution[0]);
    }

    if (   mConfig.ProjectionInfo.fpPoint[1]<static_cast<float>(mConfig.ProjectionInfo.roi[1])
        && mConfig.ProjectionInfo.fpPoint[1]<static_cast<float>(mConfig.ProjectionInfo.roi[3]))
    {
        float value = mConfig.ProjectionInfo.fpPoint[1]+((static_cast<float>(mConfig.ProjectionInfo.roi[1])-mConfig.ProjectionInfo.fpPoint[1])*mConfig.MatrixInfo.fVoxelSize[0]*mConfig.ProjectionInfo.fSDD/(mConfig.ProjectionInfo.fSOD+radius))/mConfig.ProjectionInfo.fResolution[0];
        CBCT_roi[1] = static_cast<size_t>(value);

        float value2 = mConfig.ProjectionInfo.fpPoint[1]+((static_cast<float>(mConfig.ProjectionInfo.roi[3])-mConfig.ProjectionInfo.fpPoint[1])*mConfig.MatrixInfo.fVoxelSize[0]*mConfig.ProjectionInfo.fSDD/(mConfig.ProjectionInfo.fSOD-radius))/mConfig.ProjectionInfo.fResolution[0];
        if (value2>=mConfig.ProjectionInfo.projection_roi[3])
            CBCT_roi[3] = mConfig.ProjectionInfo.projection_roi[3];
        else
            CBCT_roi[3] = static_cast<float>(value2);
    }

    if (mConfig.ProjectionInfo.fpPoint[1]>=static_cast<float>(mConfig.ProjectionInfo.roi[1]) && mConfig.ProjectionInfo.fpPoint[1]<static_cast<float>(mConfig.ProjectionInfo.roi[3]))
    {
        float value = mConfig.ProjectionInfo.fpPoint[1]-((mConfig.ProjectionInfo.fpPoint[1]-static_cast<float>(mConfig.ProjectionInfo.roi[1]))*mConfig.MatrixInfo.fVoxelSize[0]*mConfig.ProjectionInfo.fSDD/(mConfig.ProjectionInfo.fSOD-radius))/mConfig.ProjectionInfo.fResolution[0];
        if(value<=0)
            CBCT_roi[1] = 0;
        else
            CBCT_roi[1] = static_cast<size_t>(mConfig.ProjectionInfo.fpPoint[1]-((mConfig.ProjectionInfo.fpPoint[1]-static_cast<float>(mConfig.ProjectionInfo.roi[1]))*mConfig.MatrixInfo.fVoxelSize[0]*mConfig.ProjectionInfo.fSDD/(mConfig.ProjectionInfo.fSOD-radius))/mConfig.ProjectionInfo.fResolution[0]);

        float value2 = mConfig.ProjectionInfo.fpPoint[1]+((static_cast<float>(mConfig.ProjectionInfo.roi[3])-mConfig.ProjectionInfo.fpPoint[1])*mConfig.MatrixInfo.fVoxelSize[0]*mConfig.ProjectionInfo.fSDD/(mConfig.ProjectionInfo.fSOD-radius))/mConfig.ProjectionInfo.fResolution[0];
        if (value2>=mConfig.ProjectionInfo.projection_roi[3])
            CBCT_roi[3] = mConfig.ProjectionInfo.projection_roi[3];
        else
            CBCT_roi[3] = static_cast<float>(value2);
    }

//    if ((8<CBCT_roi[1]) && (CBCT_roi[1]!=0))
    if (8<CBCT_roi[1])
        CBCT_roi[1] -=8;
    if ((CBCT_roi[3]+8)<=mConfig.ProjectionInfo.projection_roi[3])
        CBCT_roi[3] +=8;




    float ic[2] = {mConfig.ProjectionInfo.fpPoint[0]-CBCT_roi[0], mConfig.ProjectionInfo.fpPoint[1]-CBCT_roi[1]};


    // Rescale image (destructive rescaling)
    //        sad_sid_2 = (pmat->sad * pmat->sad) / (pmat->sid * pmat->sid);
    //        sad_sid_2 = (mConfig.ProjectionInfo.fSOD * mConfig.ProjectionInfo.fSOD) / (mConfig.ProjectionInfo.fSDD * mConfig.ProjectionInfo.fSDD) / (nProj*0.1f);
    sad_sid_2 = (mConfig.ProjectionInfo.fSOD * mConfig.ProjectionInfo.fSOD) / (mConfig.ProjectionInfo.fSDD * mConfig.ProjectionInfo.fSDD);


    //        for (i = 0; i < cbi->dim[0]*cbi->dim[1]; i++) {
    //        cbi->img[i] *= sad_sid_2;	// Speedup trick re: Kachelsreiss
    //        cbi->img[i] *= scale;		// User scaling
    //        }

//#pragma omp parallel for
    for (i=0; i<cbi.Size(0)*cbi.Size(1); ++i)
    {
        cbi[i] *=sad_sid_2; 	// Speedup trick re: Kachelsreiss
        cbi[i] *=scale;         // User scaling
    }

    //        ramp_filter(cbi);

    float *xip = new float[3*volume.Size(0)];
    float *yip = new float[3*volume.Size(1)];
    float *zip = new float[3*volume.Size(2)];

    //        /* Precompute partial projections here */
    //        for (i = 0; i < vol->dim[0]; i++) {
    //        float x = (float) (vol->offset[0] + i * vol->spacing[0]);
    //        xip[i*3+0] = x * (pmat->matrix[0] + pmat->ic[0] * pmat->matrix[8]);
    //        xip[i*3+1] = x * (pmat->matrix[4] + pmat->ic[1] * pmat->matrix[8]);
    //        xip[i*3+2] = x * pmat->matrix[8];
    //        }
    //        for (j = 0; j < vol->dim[1]; j++) {
    //        float y = (float) (vol->offset[1] + j * vol->spacing[1]);
    //        yip[j*3+0] = y * (pmat->matrix[1] + pmat->ic[0] * pmat->matrix[9]);
    //        yip[j*3+1] = y * (pmat->matrix[5] + pmat->ic[1] * pmat->matrix[9]);
    //        yip[j*3+2] = y * pmat->matrix[9];
    //        }
    //        for (k = 0; k < vol->dim[2]; k++) {
    //        float z = (float) (vol->offset[2] + k * vol->spacing[2]);
    //        zip[k*3+0] = z * (pmat->matrix[2] + pmat->ic[0] * pmat->matrix[10])
    //            + pmat->ic[0] * pmat->matrix[11] + pmat->matrix[3];
    //        zip[k*3+1] = z * (pmat->matrix[6] + pmat->ic[1] * pmat->matrix[10])
    //            + pmat->ic[1] * pmat->matrix[11] + pmat->matrix[7];
    //        zip[k*3+2] = z * pmat->matrix[10] + pmat->matrix[11];
    //        }

    // Precompute partial projections here
//#pragma omp parallel for
    for (i = 0; i < volume.Size(0); ++i)
    {
        float x = (float) (origin[0] + i * spacing[0]);
        xip[i*3+0] = x * (proj_matrix[0] + ic[0] * proj_matrix[8]);
        xip[i*3+1] = x * (proj_matrix[4] + ic[1] * proj_matrix[8]);
        xip[i*3+2] = x *  proj_matrix[8];
    }

//#pragma omp parallel for
    for (j = 0; j < volume.Size(1); ++j)
    {
        float y = (float) (origin[1] + j * spacing[1]);
        yip[j*3+0] = y * (proj_matrix[1] + ic[0] * proj_matrix[9]);
        yip[j*3+1] = y * (proj_matrix[5] + ic[1] * proj_matrix[9]);
        yip[j*3+2] = y * proj_matrix[9];
    }

    float cor_tilted;

//#pragma omp parallel for
    for (k = 0; k < volume.Size(2); ++k)
    {
        float z = (float) (origin[2] + k * spacing[2]);

        // not so elegant solution but it seems to work POSSIBLY ANCHE QST DA ADATTARE
        if (mConfig.ProjectionInfo.bCorrectTilt)
        {
            //                    float pos = static_cast<float> (mConfig.ProjectionInfo.roi[3])-static_cast<float>(k)-static_cast<float>(mConfig.ProjectionInfo.fTiltPivotPosition);
            float pos      = static_cast<float> (CBCT_roi[3])-static_cast<float>(k)-static_cast<float>(mConfig.ProjectionInfo.fTiltPivotPosition);
            cor_tilted     = tan(-mConfig.ProjectionInfo.fTiltAngle*dPi/180)*pos+mConfig.ProjectionInfo.fCenter;
            proj_matrix[3] = ((cor_tilted-(mConfig.ProjectionInfo.fpPoint[0]-mConfig.ProjectionInfo.roi[0]))*mConfig.MatrixInfo.fVoxelSize[0])/mConfig.ProjectionInfo.fResolution[0];

        }

        zip[k*3+0] = z * (proj_matrix[2] + ic[0] * proj_matrix[10])
                     + ic[0] * proj_matrix[11] + proj_matrix[3];
        zip[k*3+1] = z * (proj_matrix[6] + ic[1] * proj_matrix[10])
                     + ic[1] * proj_matrix[11] + proj_matrix[7];
        zip[k*3+2] = z * proj_matrix[10] + proj_matrix[11];
    }

// Main backprojection loop

    logger.verbose("backproj loop");
#pragma omp parallel for // not sure about this firstprivate
    for (k = 0; k < volume.Size(2); ++k)
    {
        //            int long p = k * volume.Size(1) * volume.Size(0);
        int long j;
        for (j = 0; j < volume.Size(1); ++j)
        {
            int long i;
            float acc2[3];
            //            static inline void vec3_add3 (float* v1, const float* v2, const float* v3) {
            //                v1[0] = v2[0] + v3[0]; v1[1] = v2[1] + v3[1]; v1[2] = v2[2] + v3[2];
            //            }
            //            vec3_add3 (acc2, &zip[3*k], &yip[3*j]);
            acc2[0] = zip[3*k]   + yip[3*j];
            acc2[1] = zip[3*k+1] + yip[3*j+1];
            acc2[2] = zip[3*k+2] + yip[3*j+2];
            int long p=k * volume.Size(1) * volume.Size(0)+j *volume.Size(0);
            for (i = mask[j].first+1; i <= mask[j].second; ++i)
            {
                //                for (i = 0; i < volume.Size(0); i++) {

                float dw;
                float acc3[3];
                //            vec3_add3 (acc3, acc2, &xip[3*i]);
                // AK
                //                        acc3[0] = acc2[0]+xip[3*i];
                //                        acc3[1] = acc2[1]+xip[3*i+1];
                //                        acc3[2] = acc2[2]+xip[3*i+2];
                //                        dw = 1.0 / acc3[2];
                //                        acc3[0] = acc3[0] * dw;
                //                        acc3[1] = acc3[1] * dw;

                dw = 1.0f / (acc2[2]+xip[3*i+2]);

                img[p+i] +=
                        dw * dw * get_pixel_value_c (cbi, dw*(acc2[1]+xip[3*i+1]), dw*(acc2[0]+xip[3*i]));
                //            volume[p++] +=
                //                dw * dw * get_pixel_value_c (cbi, acc3[1], acc3[0]);
            }
        }
    }

    delete [] xip;
    delete [] yip;
    delete [] zip;
    logger.verbose("end project vol onto image");
}



// Reference implementation is the most straightforward implementation, also it is the slowest
void FDKbp_single::project_volume_onto_image_reference (
    kipl::base::TImage<float, 2> &cbi,
    float *proj_matrix,
    float *nrm
)
{
    int i, j, k, p;
    float vp[4];   /* vp = voxel position */
    //    float* img = (float*) vol->img; // voxel data of the reconstructed volume -> è il mio volume.
    float *img = volume.GetDataPtr(); // do i need this?



    const size_t SizeZ  = volume.Size(0)>>2;
    __m128 column[2048]; // not used for now..

    //    Proj_matrix *pmat = cbi->pmat; /* projection matrix 3D -> 2D in homogenous coordinates */

    p = 0;
    vp[3] = 1.0;
    /* Loop k (slices), j (rows), i (cols) */


    float ic[2] = {mConfig.ProjectionInfo.fpPoint[0], mConfig.ProjectionInfo.fpPoint[1]};


    float spacing[3];
    spacing[0] = mConfig.MatrixInfo.fVoxelSize[0];
    spacing[1] = mConfig.MatrixInfo.fVoxelSize[1];
    spacing[2] = mConfig.MatrixInfo.fVoxelSize[2];

    float origin[3];
    float U = static_cast<float>(mConfig.ProjectionInfo.roi[2]-mConfig.ProjectionInfo.roi[0]);
    float V = static_cast<float>(mConfig.ProjectionInfo.roi[3]-mConfig.ProjectionInfo.roi[1]);

    origin[0] = -(U-mConfig.ProjectionInfo.fCenter)*spacing[0]-spacing[0]/2;
    origin[1] = -(U-mConfig.ProjectionInfo.fCenter)*spacing[1]-spacing[1]/2;
    origin[2] = -(V-(mConfig.ProjectionInfo.fpPoint[1]-mConfig.ProjectionInfo.roi[1]))*spacing[2]-spacing[2]/2;


    // the following part is not updated

    for (k = 0; k < volume.Size(2); k++)
    {

        //        vp[2] = (float) (vol->origin[2] + k * vol->spacing[2]);
        vp[2] = (float) (origin[2] + k * spacing[2]);

        for (j = 0; j < volume.Size(1); j++)
        {
            vp[1] = (float) (origin[1] + j * spacing[1]);

            for (i = 0; i < volume.Size(0); i++)
            {

                float ip[3];        // ip = image position
                float s;            // s = projection of vp onto s axis
                vp[0] = (float) (origin[0] + i * spacing[0]);
                //            mat43_mult_vec3 (ip, pmat->matrix, vp);
                // ip = matrix * vp
                ip[0] = proj_matrix[0]*vp[0]+proj_matrix[1]*vp[1]+proj_matrix[2]*vp[2]+proj_matrix[3]*vp[3];
                ip[1] = proj_matrix[4]*vp[0]+proj_matrix[5]*vp[1]+proj_matrix[6]*vp[2]+proj_matrix[7]*vp[3];
                ip[2] = proj_matrix[8]*vp[0]+proj_matrix[9]*vp[1]+proj_matrix[10]*vp[2]+proj_matrix[11]*vp[3];

                //            ip[0] = pmat->ic[0] + ip[0] / ip[2];
                //            ip[1] = pmat->ic[1] + ip[1] / ip[2];
                ip[0] = ic[0] + ip[0] / ip[2];
                ip[1] = ic[1] + ip[1] / ip[2];

                // Distance on central axis from voxel center to source
                // pmat->nrm is normal of panel

                //            s = vec3_dot (pmat->nrm, vp);

                s = nrm[0]*vp[0]+nrm[1]*vp[1]+nrm[2]*vp[2];

                // Conebeam weighting factor
                s = mConfig.ProjectionInfo.fSOD - s;
                s = mConfig.ProjectionInfo.fSOD * mConfig.ProjectionInfo.fSOD / (s * s);


                //            volume[p++] += scale * s * get_pixel_value_b (cbi, ip[1], ip[0]);

                img[p++] += s*get_pixel_value_b (cbi, ip[1], ip[0]);
            }
        }
    }

}

// Nearest neighbor interpolation of intensity value on image
 float FDKbp_single::get_pixel_value_b (kipl::base::TImage<float, 2> &cbi, float r, float c)
{
    int rr, cc;

    rr = (int)(r);
    if ( (rr < 0) || (rr >= cbi.Size(1)) ) return 0.0;
    cc = (int)(c);

    if (cc < 0 || cc >= cbi.Size(0)) return 0.0;
    return cbi[rr*cbi.Size(0) + cc];
}


// get_pixel_value_c seems to be no faster than get_pixel_value_b, despite having two fewer compares.
float FDKbp_single::get_pixel_value_c (kipl::base::TImage<float,2> &cbi, float r, float c)
{
    int rr, cc;

    r += 0.5;
    if (r < 0) return 0.0;
    if (r >= static_cast<float>(cbi.Size(1)))
        return 0.0;

    rr = static_cast<int>(r);

    c += 0.5;
    if (c < 0) return 0.0;
    if (c >= static_cast<float>(cbi.Size(0)))
        return 0.0f;
    cc = static_cast<int>(c);

    return cbi[rr*cbi.Size(0) + cc];
}
