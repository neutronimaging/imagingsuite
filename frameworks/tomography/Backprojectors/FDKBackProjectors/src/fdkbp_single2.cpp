//<LICENSE>

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
#include <ReconException.h>

#include "fdkbp_single2.h"

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


FDKbp_single2::FDKbp_single2(kipl::interactors::InteractionBase *interactor) :
    FdkReconBase("muhrec","FDKbp_single2",BackProjectorModuleBase::MatrixXYZ,interactor)
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

FDKbp_single2::~FDKbp_single2()
{


}

int FDKbp_single2::Initialize()
{
    return 0;
}

int FDKbp_single2::InitializeBuffers(int width, int height)
{
    prepareFFT(width,height);
    return 0;
}

int FDKbp_single2::FinalizeBuffers()
{
    cleanupFFT();

    return 0;
}

void FDKbp_single2::multiplyMatrix(float *mat1, float *mat2, float *result, int rows, int columns, int columns1)
{

    for (int i = 0; i < rows; i++)
    {
         for (int j = 0; j < columns; j++)
         {
             float acc = 0.0f;
             for (int k = 0; k < columns1; k++)
             {
                 acc += m_idx(mat1,columns1,i,k) * m_idx(mat2,columns,k,j);
             }
             m_idx(result,columns,i,j) = acc;
         }
    }

}

void FDKbp_single2::getProjMatrix(float angles, float *nrm, float *proj_matrix)
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


    float offCenter =  (mConfig.ProjectionInfo.fpPoint[0]-mConfig.ProjectionInfo.roi[0]-mConfig.ProjectionInfo.fCenter)*mConfig.MatrixInfo.fVoxelSize[0]; // in world coordinate
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

    plt[0] =  nrm[1]*vup[2]-nrm[2]*vup[1];
    plt[1] =  nrm[2]*vup[0]-nrm[0]*vup[2];
    plt[2] =  nrm[0]*vup[1]-nrm[1]*vup[0];

    float norm_plt = sqrt(plt[0]*plt[0]+plt[1]*plt[1]+plt[2]*plt[2]);

    plt[0] /= norm_plt;
    plt[1] /= norm_plt;
    plt[2] /= norm_plt;

    pup[0] =  plt[1]*nrm[2]-plt[2]*nrm[1];
    pup[1] =  plt[2]*nrm[0]-plt[0]*nrm[2];
    pup[2] =  plt[0]*nrm[1]-plt[1]*nrm[0];

    float norm_pup = sqrt(pup[0]*pup[0]+pup[1]*pup[1]+pup[2]*pup[2]);

    pup[0] /= norm_pup;
    pup[1] /= norm_pup;
    pup[2] /= norm_pup;

    // Build extrinsic matrix - rotation part
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
    extrinsic[3]  = plt[0]*tgt[0]+plt[1]*tgt[1]+plt[2]*tgt[2]-offCenter;
    extrinsic[7]  = pup[0]*tgt[0]+pup[1]*tgt[1]+pup[2]*tgt[2];
    extrinsic[11] = nrm[0]*tgt[0]+nrm[1]*tgt[1]+nrm[2]*tgt[2]+sad;


    // -------------- Build intrinsic matrix
    intrinsic[0]  = 1.0f/mConfig.ProjectionInfo.fResolution[0];
    intrinsic[5]  = 1.0f/mConfig.ProjectionInfo.fResolution[1];
    intrinsic[10] = 1.0f/sid;

    // -------  Build Projection Geometry Matrix = intrinsic * extrinsic

    #pragma omp parallel for
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            float acc = 0.0f;
            for (int k = 0; k < 4; ++k)
            {
                acc += m_idx(intrinsic,4,i,k) * m_idx(extrinsic,4,k,j);
            }
            m_idx(proj_matrix,4,i,j) = acc;
        }
    }
}


/// Main reconstruction loop - Loop over images, and backproject each image into the volume.
size_t FDKbp_single2::reconstruct(kipl::base::TImage<float,2> &proj, float angles, size_t nProj)
{

    // missing things todo:
    // 1. use circular mask
    // 2. check parallel cycles

    float nrm[3] = {0.0, 0.0, 0.0};
    float proj_matrix[12] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}; /* Projection matrix */


    getProjMatrix(angles, nrm, proj_matrix); // panel oriented as z, to be used with project_volume_onto_image_c

    // Apply ramp filter

    ramp_filter_tuned(proj);

    // REFERENCE FDK IMPLEMENTATION:
    // project_volume_onto_image_reference (proj, &proj_matrix[0], &nrm[0]);

    // DEFAULT FDK BACK PROJECTOR:
    project_volume_onto_image_c (proj, &proj_matrix[0], nProj);

    return 0L;
}


void FDKbp_single2::project_volume_onto_image_c(kipl::base::TImage<float, 2> &cbi,
    float *proj_matrix, size_t nProj)
{
    logger(logger.LogDebug,"Started FDK back-projector");

    //long int i, j, k;

    float* img = cbct_volume.GetDataPtr();
    float *xip, *yip, *zip;
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

    size_t CBCT_roi[4];
    CBCT_roi[0] = mConfig.ProjectionInfo.roi[0];
    CBCT_roi[2] = mConfig.ProjectionInfo.roi[2];

    if (mConfig.ProjectionInfo.fpPoint[1]>=static_cast<float>(mConfig.ProjectionInfo.roi[1]) &&
            mConfig.ProjectionInfo.fpPoint[1]>=static_cast<float>(mConfig.ProjectionInfo.roi[3]))
    {
        CBCT_roi[3] = static_cast<size_t>(mConfig.ProjectionInfo.fpPoint[1]-((mConfig.ProjectionInfo.fpPoint[1]-static_cast<float>(mConfig.ProjectionInfo.roi[3]))*mConfig.MatrixInfo.fVoxelSize[0]*mConfig.ProjectionInfo.fSDD/(mConfig.ProjectionInfo.fSOD+radius))/mConfig.ProjectionInfo.fResolution[0]);

        float value = mConfig.ProjectionInfo.fpPoint[1]-((mConfig.ProjectionInfo.fpPoint[1]-static_cast<float>(mConfig.ProjectionInfo.roi[1]))*mConfig.MatrixInfo.fVoxelSize[0]*mConfig.ProjectionInfo.fSDD/(mConfig.ProjectionInfo.fSOD-radius))/mConfig.ProjectionInfo.fResolution[0];

        if (value<=0)
            CBCT_roi[1] = 0;
        else
            CBCT_roi[1] = static_cast<size_t>(mConfig.ProjectionInfo.fpPoint[1]-((mConfig.ProjectionInfo.fpPoint[1]-static_cast<float>(mConfig.ProjectionInfo.roi[1]))*mConfig.MatrixInfo.fVoxelSize[0]*mConfig.ProjectionInfo.fSDD/(mConfig.ProjectionInfo.fSOD-radius))/mConfig.ProjectionInfo.fResolution[0]);
    }

    if (mConfig.ProjectionInfo.fpPoint[1]<static_cast<float>(mConfig.ProjectionInfo.roi[1]) &&
            mConfig.ProjectionInfo.fpPoint[1]<static_cast<float>(mConfig.ProjectionInfo.roi[3]))
    {
        float value = mConfig.ProjectionInfo.fpPoint[1]
                +((static_cast<float>(mConfig.ProjectionInfo.roi[1])-mConfig.ProjectionInfo.fpPoint[1])*mConfig.MatrixInfo.fVoxelSize[0]*mConfig.ProjectionInfo.fSDD/(mConfig.ProjectionInfo.fSOD+radius))/mConfig.ProjectionInfo.fResolution[0];

        CBCT_roi[1]  = static_cast<size_t>(value);

        float value2 = mConfig.ProjectionInfo.fpPoint[1]
                +((static_cast<float>(mConfig.ProjectionInfo.roi[3])-mConfig.ProjectionInfo.fpPoint[1])*mConfig.MatrixInfo.fVoxelSize[0]*mConfig.ProjectionInfo.fSDD/(mConfig.ProjectionInfo.fSOD-radius))/mConfig.ProjectionInfo.fResolution[0];

        if (value2>=mConfig.ProjectionInfo.projection_roi[3])
            CBCT_roi[3] = mConfig.ProjectionInfo.projection_roi[3];
        else
            CBCT_roi[3] = static_cast<float>(value2);
    }

    if (mConfig.ProjectionInfo.fpPoint[1]>=static_cast<float>(mConfig.ProjectionInfo.roi[1]) &&
           mConfig.ProjectionInfo.fpPoint[1]<static_cast<float>(mConfig.ProjectionInfo.roi[3]))
    {
       float value = mConfig.ProjectionInfo.fpPoint[1]-((mConfig.ProjectionInfo.fpPoint[1]-static_cast<float>(mConfig.ProjectionInfo.roi[1]))*mConfig.MatrixInfo.fVoxelSize[0]*mConfig.ProjectionInfo.fSDD/(mConfig.ProjectionInfo.fSOD-radius))/mConfig.ProjectionInfo.fResolution[0];
       if (value<=0)
           CBCT_roi[1] = 0;
       else
           CBCT_roi[1] = static_cast<size_t>(mConfig.ProjectionInfo.fpPoint[1]-((mConfig.ProjectionInfo.fpPoint[1]-static_cast<float>(mConfig.ProjectionInfo.roi[1]))*mConfig.MatrixInfo.fVoxelSize[0]*mConfig.ProjectionInfo.fSDD/(mConfig.ProjectionInfo.fSOD-radius))/mConfig.ProjectionInfo.fResolution[0]);

       float value2 = mConfig.ProjectionInfo.fpPoint[1]+((static_cast<float>(mConfig.ProjectionInfo.roi[3])-mConfig.ProjectionInfo.fpPoint[1])*mConfig.MatrixInfo.fVoxelSize[0]*mConfig.ProjectionInfo.fSDD/(mConfig.ProjectionInfo.fSOD-radius))/mConfig.ProjectionInfo.fResolution[0];
       if (value2>=mConfig.ProjectionInfo.projection_roi[3])
           CBCT_roi[3] = mConfig.ProjectionInfo.projection_roi[3];
       else
           CBCT_roi[3] = static_cast<float>(value2);
    }

    if ( ((CBCT_roi[1]-8)>=0) && (CBCT_roi[1]!=0) )
       CBCT_roi[1] -=8;

    if ((CBCT_roi[3]+8) <= mConfig.ProjectionInfo.projection_roi[3] )
       CBCT_roi[3] +=8;

    float ic[2] = {mConfig.ProjectionInfo.fpPoint[0]-CBCT_roi[0], mConfig.ProjectionInfo.fpPoint[1]-CBCT_roi[1]};


    // Rescale image (destructive rescaling)
    sad_sid_2 = (mConfig.ProjectionInfo.fSOD * mConfig.ProjectionInfo.fSOD) / (mConfig.ProjectionInfo.fSDD * mConfig.ProjectionInfo.fSDD);

    #pragma omp parallel for
    for (long int i=0; i<static_cast<long int>(cbi.Size(0)*cbi.Size(1)); ++i)
    {
        cbi[i] *=sad_sid_2; 	// Speedup trick re: Kachelsreiss
        cbi[i] *=scale;         // User scaling
    }

    //        ramp_filter(cbi);

    xip = new float[3*volume.Size(0)];
    yip = new float[3*volume.Size(1)];
    zip = new float[3*volume.Size(2)];

    // Precompute partial projections here
    #pragma omp parallel for
    for (long int i = 0; i < static_cast<long int>(volume.Size(0)); ++i)
    {
        float x = static_cast<float>(origin[0] + i * spacing[0]);
        xip[i*3+0] = x * (proj_matrix[0] + ic[0] * proj_matrix[8]);
        xip[i*3+1] = x * (proj_matrix[4] + ic[1] * proj_matrix[8]);
        xip[i*3+2] = x * proj_matrix[8];
    }

    #pragma omp parallel for
    for (long int j = 0; j < static_cast<long int>(volume.Size(1)); ++j)
    {
        float y = static_cast<float>(origin[1] + j * spacing[1]);

        yip[j*3]   = y * (proj_matrix[1] + ic[0] * proj_matrix[9]);
        yip[j*3+1] = y * (proj_matrix[5] + ic[1] * proj_matrix[9]);
        yip[j*3+2] = y * proj_matrix[9];
    }

    float cor_tilted;

    #pragma omp parallel for
    for (long int k = 0; k < static_cast<long int>(volume.Size(2)); k++)
    {
        float z = static_cast<float>(origin[2] + k * spacing[2]);

        // not so elegant solution but it seems to work POSSIBLY ANCHE QST DA ADATTARE
        if (mConfig.ProjectionInfo.bCorrectTilt)
        {
            float pos = static_cast<float> (CBCT_roi[3])
                        -static_cast<float>(k)
                        -static_cast<float>(mConfig.ProjectionInfo.fTiltPivotPosition);

            cor_tilted     = tan(-mConfig.ProjectionInfo.fTiltAngle*dPi/180)*pos+mConfig.ProjectionInfo.fCenter;
            proj_matrix[3] = ((cor_tilted-(mConfig.ProjectionInfo.fpPoint[0]-mConfig.ProjectionInfo.roi[0]))*mConfig.MatrixInfo.fVoxelSize[0])/mConfig.ProjectionInfo.fResolution[0];

        }

        zip[k*3+0] = z * (proj_matrix[2] + ic[0] * proj_matrix[10])
                     + ic[0] * proj_matrix[11] + proj_matrix[3];
        zip[k*3+1] = z * (proj_matrix[6] + ic[1] * proj_matrix[10])
                     + ic[1] * proj_matrix[11] + proj_matrix[7];
        zip[k*3+2] = z * proj_matrix[10] + proj_matrix[11];
    }


    //           Main loop

    #pragma omp parallel for // not sure about this firstprivate
    for (long int k = 0; k < static_cast<long int>(volume.Size(2)); k++)
    {
    //            int long p = k * volume.Size(1) * volume.Size(0);
        int long j;
        float acc20=0.0f;
        float acc21=0.0f;
        float acc22=0.0f;
        int k3 = 3*k;
        int long p0 = k * volume.Size(1) * volume.Size(0);
        for (long int j = 0; j < static_cast<long int>(volume.Size(1)); j++)
        {
            int j3 = j*3;
            acc20 = zip[k3]   + yip[j3];
            acc21 = zip[k3+1] + yip[j3+1];
            acc22 = zip[k3+2] + yip[j3+2];

            int long p= p0 + j *volume.Size(0);
            for (long int i = mask[j].first+1; i <= static_cast<long int>(mask[j].second); ++i)
            {
                float dw;

                dw = 1.0f / (acc22+xip[3*i+2]);

                img[p+i] +=
                    dw * dw * get_pixel_value_c (cbi, dw*(acc21+xip[3*i+1]), dw*(acc20+xip[3*i]));
            }
        }
    }


    delete [] xip;
    delete [] yip;
    delete [] zip;
}



// Reference implementation is the most straightforward implementation, also it is the slowest
void FDKbp_single2::project_volume_onto_image_reference (
    kipl::base::TImage<float, 2> &cbi,
    float *proj_matrix,
    float *nrm
)
{
    int i, j, k, p;
    float vp[4];   /* vp = voxel position */

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
        vp[2] = (float) (origin[2] + k * spacing[2]);

        for (j = 0; j < volume.Size(1); j++)
        {
            vp[1] = (float) (origin[1] + j * spacing[1]);

            for (i = 0; i < volume.Size(0); i++)
            {

                float ip[3];        // ip = image position
                float s;            // s = projection of vp onto s axis
                vp[0] = (float) (origin[0] + i * spacing[0]);
                ip[0] = proj_matrix[0]*vp[0]+proj_matrix[1]*vp[1]+proj_matrix[2]*vp[2]+proj_matrix[3]*vp[3];
                ip[1] = proj_matrix[4]*vp[0]+proj_matrix[5]*vp[1]+proj_matrix[6]*vp[2]+proj_matrix[7]*vp[3];
                ip[2] = proj_matrix[8]*vp[0]+proj_matrix[9]*vp[1]+proj_matrix[10]*vp[2]+proj_matrix[11]*vp[3];

                ip[0] = ic[0] + ip[0] / ip[2];
                ip[1] = ic[1] + ip[1] / ip[2];

                // Distance on central axis from voxel center to source
                // pmat->nrm is normal of panel

                s = nrm[0]*vp[0]+nrm[1]*vp[1]+nrm[2]*vp[2];

                // Conebeam weighting factor
                s = mConfig.ProjectionInfo.fSOD - s;
                s = mConfig.ProjectionInfo.fSOD * mConfig.ProjectionInfo.fSOD / (s * s);

                img[p++] += s*get_pixel_value_b (cbi, ip[1], ip[0]);
            }

        }
    }

}

// Nearest neighbor interpolation of intensity value on image
 float FDKbp_single2::get_pixel_value_b (kipl::base::TImage<float, 2> &cbi, float r, float c)
{
    int rr, cc;

    rr = (int)(r);
    if (rr < 0 || rr >= cbi.Size(1))
        return 0.0f;

    cc = (int)(c);

    if (cc < 0 || cc >= cbi.Size(0))
        return 0.0f;

    return cbi[rr*cbi.Size(0) + cc];
}


 // get_pixel_value_c seems to be no faster than get_pixel_value_b, despite having two fewer compares.
float FDKbp_single2::get_pixel_value_c (kipl::base::TImage<float,2> &cbi, float r, float c)
 {
     int rr, cc;

     r += 0.5;
     if (r < 0) return 0.0;
     if (r >= (float) cbi.Size(1)) return 0.0;
     rr = (int) r;

     c += 0.5;
     if (c < 0) return 0.0;
     if (c >= (float) cbi.Size(0)) return 0.0;
     cc = (int) c;

     return cbi[rr*cbi.Size(0) + cc];

 }



void FDKbp_single2::ramp_filter(kipl::base::TImage<float, 2> &img)
{
    logger(logger.LogDebug,"Started ramp_filter");
    unsigned int i, r, c;
    unsigned int N, Npad;

    unsigned int width = img.Size(0);
    unsigned int height = img.Size(1);
    unsigned int padwidth = 2*pow(2, ceil(log(width)/log(2)));
    unsigned int pad_factor = padwidth-width;
    unsigned int padheight = height;

    std::vector<size_t> pad_dims = {padwidth, height};
    kipl::base::TImage< float,2 > padImg(pad_dims);
    padImg = 0.0; // fill with zeros

    // do zero padding
    for (size_t i=0; i<height; ++i)
    {
        memcpy(padImg.GetLinePtr(i,0)+pad_factor/2, img.GetLinePtr(i,0), sizeof(float)*width);
    }

    fftw_complex *in;
    fftw_complex *fft;
    fftw_complex *ifft;
    fftw_plan fftp;
    fftw_plan ifftp;
    float *ramp = new float[padwidth];

    if (!ramp)
    {
        throw ReconException("Error allocating memory for ramp",__FILE__,__LINE__);
    }

    N = width * height;
    Npad = padwidth*padheight;

    in   = (fftw_complex*) fftw_malloc (sizeof(fftw_complex) * Npad);
    fft  = (fftw_complex*) fftw_malloc (sizeof(fftw_complex) * Npad);
    ifft = (fftw_complex*) fftw_malloc (sizeof(fftw_complex) * Npad);


    if (!in || !fft || !ifft)
    {
        printf("Error allocating memory for fft\n");
    }

    float *data    = img.GetDataPtr();
    float *paddata = padImg.GetDataPtr();



    for (r = 0; r < MARGINF; ++r)
        memcpy (paddata + r * padwidth,
                paddata + MARGINF * padwidth,
                padwidth * sizeof(float));

    for (r = height - MARGINF; r < height; ++r)
        memcpy (paddata + r * padwidth,
                paddata + (padheight - MARGINF - 1) * padwidth,
                padwidth * sizeof(float));

    for (r = 0; r < padheight; ++r)
    {
        for (c = 0; c < MARGINF; ++c)
        {
            paddata[r * padwidth + c] = paddata[r * padwidth + MARGINF];
        }
        for (c = padwidth - MARGINF; c < padwidth; ++c)
        {
            paddata[r * padwidth + c] = paddata[r * padwidth + padwidth - MARGINF - 1];
        }
    }



    // Fill in ramp filter in frequency space
    for (i = 0; i < Npad; ++i)
    {
        in[i][0] = (float)(paddata[i]);
        in[i][1] = 0.0;
    }

    // Add padding
    for (i = 0; i < padwidth / 2; ++i)
        ramp[i] = i;


    for (i = padwidth / 2; i < (unsigned int) padwidth; ++i)
        ramp[i] = padwidth - i;

    // Roll off ramp filter
    for (i = 0; i < padwidth; ++i)
    {
        ramp[i] *= (0.54+0.46*(cos (i * DEGTORADF * 360 / padwidth) + 1));
        ramp[i] /= padwidth*0.5;
    }

    for (r = 0; r < padheight; ++r)
    {
        fftp = fftw_plan_dft_1d (padwidth, in + r * padwidth, fft + r * padwidth,
            FFTW_FORWARD, FFTW_ESTIMATE);
        if (!fftp)
        {
            printf ("Error creating fft plan\n");
        }

        ifftp = fftw_plan_dft_1d (padwidth, fft + r * padwidth, ifft + r * padwidth,
            FFTW_BACKWARD, FFTW_ESTIMATE);
        if (!ifftp)
        {
            printf ("Error creating ifft plan\n");
        }

        fftw_execute (fftp);

        // Apply ramp
        for (c = 0; c < padwidth; ++c)
        {
            fft[r * padwidth + c][0] *= ramp[c];
            fft[r * padwidth + c][1] *= ramp[c];
        }

        fftw_execute (ifftp);

        fftw_destroy_plan (fftp);
        fftw_destroy_plan (ifftp);
    }



    for (i = 0; i < Npad; ++i)
        ifft[i][0] /= padwidth;

    for (i = 0; i < Npad; ++i)
        paddata[i] = (float)(ifft[i][0]);

    // go back to original dimension

    for (i=0; i<height; ++i)
    {
        memcpy(img.GetLinePtr(i,0), padImg.GetLinePtr(i,0)+pad_factor/2, sizeof(float)*width);
    }

    fftw_free (in);
    fftw_free (fft);
    fftw_free (ifft);
    delete [] ramp;
}

void FDKbp_single2::ramp_filter_tuned(kipl::base::TImage<float, 2> &img)
{
    std::ostringstream msg;
    logger(logger.LogDebug,"Started tuned ramp_filter");
    unsigned int i, r, c;
    unsigned int N, Npad;

    unsigned int width      = img.Size(0);
    unsigned int height     = img.Size(1);
    unsigned int pad_factor = padwidth-width;
    unsigned int padheight  = height;
    // Comment AK: Using only the intended line will speed up things
    unsigned int firstLine  = 0;      //mConfig.ProjectionInfo.roi[1];
    unsigned int lastLine   = height; //mConfig.ProjectionInfo.roi[3];

    msg.str("");
    msg<<"Pad width="<<padwidth;
    logger(logger.LogDebug,msg.str());

    std::vector<size_t> pad_dims = {padwidth, height};
    kipl::base::TImage< float,2 > padImg(pad_dims);
    padImg = 0.0f; // fill with zeros

    // do zero padding
    for (size_t i=firstLine; i<lastLine; ++i)
    {
        memcpy(padImg.GetLinePtr(i)+pad_factor/2, img.GetLinePtr(i), sizeof(float)*width);
    }


    float *ramp=nullptr;

    ramp = new float[padwidth];

    if (!ramp)
    {
        throw ReconException("Error allocating memory for ramp",__FILE__,__LINE__);
    }

    N = width * height;
    Npad = padwidth*padheight;

    if (!in || !fft || !ifft)
    {
        throw ReconException("Error allocating memory for fft buffers",__FILE__,__LINE__);
    }

    float *data = img.GetDataPtr();
    float *paddata = padImg.GetDataPtr();

    for (r = 0; r < MARGINF; ++r)
        memcpy (paddata + r * padwidth,
                paddata + MARGINF * padwidth,
                padwidth * sizeof(float));

    for (r = height - MARGINF; r < height; ++r)
        memcpy (paddata + r * padwidth,
                paddata + (padheight - MARGINF - 1) * padwidth,
                padwidth * sizeof(float));

    for (r = 0; r < padheight; ++r)
    {
        for (c = 0; c < MARGINF; ++c)
        {
            paddata[r * padwidth + c] = paddata[r * padwidth + MARGINF];
        }

        for (c = padwidth - MARGINF; c < padwidth; ++c)
        {
            paddata[r * padwidth + c] = paddata[r * padwidth + padwidth - MARGINF - 1];
        }
    }

    // Fill in ramp filter in frequency space
    for (i = 0; i < Npad; ++i)
    {
        in[i][0] = (float)(paddata[i]);
        in[i][1] = 0.0;
    }


    // Add padding
    for (i = 0; i < padwidth / 2; ++i)
        ramp[i] = i;


    for (i = padwidth / 2; i < (unsigned int) padwidth; ++i)
        ramp[i] = padwidth - i;

    // Roll off ramp filter
    for (i = 0; i < padwidth; ++i)
    {
        ramp[i] *= (0.54+0.46*(cos (i * DEGTORADF * 360 / padwidth) + 1));
        ramp[i] /= padwidth*0.5;
    }

    for (r = firstLine; r < lastLine; ++r)
    {
        // Copy padded projection line to 'in' buffer
        memcpy(in_buffer,in+r*padwidth,sizeof(fftwf_complex)*padwidth);
        fftwf_execute (fftp);

        // Apply ramp
        for (c = 0; c < padwidth; ++c)
        {
            fft[c][0] *= ramp[c];
            fft[c][1] *= ramp[c];
        }

        // Add apodization. Comment AK: This already done as roll-off ramp.

        fftwf_execute (ifftp);
        memcpy(ifft+r*padwidth,ifft_buffer,sizeof(fftwf_complex)*padwidth);
    }

    data[i] = ifft[i][0];
    float rpadwidth= 1.0f/padwidth;


    for (i = 0; i < Npad; ++i)
        paddata[i] = ifft[i][0]*rpadwidth;

    // fo back to original dimension

    for (i=firstLine; i<lastLine; ++i)
    {
        memcpy(img.GetLinePtr(i,0), padImg.GetLinePtr(i,0)+pad_factor/2, sizeof(float)*width);
    }

    delete [] ramp;
}

void FDKbp_single2::prepareFFT(int width, int height)
{
    padwidth = 2*pow(2, ceil(log(width)/log(2)));
    int Npad=padwidth*height;

    in          = (fftwf_complex*) fftw_malloc (sizeof(fftwf_complex) * Npad);
    in_buffer   = (fftwf_complex*) fftw_malloc (sizeof(fftwf_complex) * padwidth);
    fft         = (fftwf_complex*) fftw_malloc (sizeof(fftwf_complex) * padwidth);
    ifft        = (fftwf_complex*) fftw_malloc (sizeof(fftwf_complex) * Npad);
    ifft_buffer = (fftwf_complex*) fftw_malloc (sizeof(fftwf_complex) * padwidth);

    fftp = fftwf_plan_dft_1d (padwidth, in_buffer, fft, FFTW_FORWARD, FFTW_ESTIMATE);
    if (!fftp)
    {
        throw ReconException("Error creating fft plan",__FILE__,__LINE__);
    }

    ifftp = fftwf_plan_dft_1d (padwidth, fft, ifft_buffer, FFTW_BACKWARD, FFTW_ESTIMATE);
    if (!ifftp)
    {
        throw ReconException("Error creating ifft plan",__FILE__,__LINE__);
    }
}

void FDKbp_single2::cleanupFFT()
{
    fftwf_destroy_plan (fftp);
    fftwf_destroy_plan (ifftp);

    fftw_free (in);
    fftw_free (fft);
    fftw_free (ifft);
    fftw_free (in_buffer);
    fftw_free (ifft_buffer);
}
