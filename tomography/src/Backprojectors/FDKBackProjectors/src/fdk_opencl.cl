/* -----------------------------------------------------------------------
   See COPYRIGHT.TXT and LICENSE.TXT for copyright and license information
   ----------------------------------------------------------------------- */
// Sweep through all voxel values and renormalize them to the Hounsfield (hu) scale.
#if defined (commentout)
__kernel void convert_to_hu_cl(
    __global float *img,
    int nvoxels
) {
    int p = get_global_id(0);

    if (p < nvoxels)
        img[p] = (149.70059880239520958083832335329 * img[p]) - 1000;
}


// Define image/texture sampling parameters
const sampler_t dev_img_sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;
#endif

#if defined (commentout)
// FDK kernel - no interpolation
__kernel void kernel_fdk(
    __global float *dev_vol,
    __read_only image2d_t dev_img,
    __constant float *dev_matrix,
    __constant float4 *nrm,
    __constant float4 *vol_offset,
    __constant float4 *vol_pix_spacing,
    __constant int4 *vol_dim,
    __constant float2 *ic,
    __constant int2 *proj_dim,
    __constant float *sad,
    __constant float *scale,
    __constant int4 *ndevice,
    int offset
) {
    uint i = get_global_id(0);
    uint j = get_global_id(1);
    uint k = get_global_id(2);

    if (i >= (*ndevice).x || j >= (*ndevice).y || k >= (*ndevice).z)
        return;

    // Index row major into the volume
    long vol_idx = i + (j * (*vol_dim).x) + (k * (*vol_dim).x * (*vol_dim).y);
    k += offset;

    // Get volume value from global memory
    float dev_vol_value = dev_vol[vol_idx];

    // Offset volume coordinates
    float4 vp;
    vp.x = (*vol_offset).x + (i * (*vol_pix_spacing).x);        // Compiler should combine into 1 FMAD.
    vp.y = (*vol_offset).y + (j * (*vol_pix_spacing).y);        // Compiler should combine into 1 FMAD.
    vp.z = (*vol_offset).z + (k * (*vol_pix_spacing).z);        // Compiler should combine into 1 FMAD.

    // Matrix multiplication
    float4 ip;
    ip.x = (dev_matrix[0] * vp.x) + (dev_matrix[1] * vp.y) + (dev_matrix[2] * vp.z) + dev_matrix[3];
    ip.y = (dev_matrix[4] * vp.x) + (dev_matrix[5] * vp.y) + (dev_matrix[6] * vp.z) + dev_matrix[7];
    ip.z = (dev_matrix[8] * vp.x) + (dev_matrix[9] * vp.y) + (dev_matrix[10] * vp.z) + dev_matrix[11];

    // Change coordinate systems
    ip.x = (*ic).x + ip.x / ip.z;
    ip.y = (*ic).y + ip.y / ip.z;

    // Retrieve pixel location from 2D image
    int2 pos;
    pos.y = convert_int_rtn(ip.x);
    pos.x = convert_int_rtn(ip.y);

    // Clip against image dimensions
    if (pos.x < 0 || pos.x >= (*proj_dim).x || pos.y < 0 || pos.y >= (*proj_dim).y)
        return;

    // Get pixel from texture memory
    float4 voxel_data = read_imagef(dev_img, dev_img_sampler, pos);

    // Dot product
    float s = ((*nrm).x * vp.x) + ((*nrm).y * vp.y) + ((*nrm).z * vp.z);

    // Conebeam weighting factor
    s = (*sad) - s;
    s = ((*sad) * (*sad)) / (s * s);

    // Place weighing factor into the volume
    dev_vol[vol_idx] = dev_vol_value + ((*scale) * s * voxel_data.x);
}


// FDK kernel - bilinear interopolation
__kernel void kernel_fdk_bilinear(
    __global float *dev_vol,
    __read_only image2d_t dev_img,
    __constant float *dev_matrix,
    __constant float4 *nrm,
    __constant float4 *vol_offset,
    __constant float4 *vol_pix_spacing,
    int4 vol_dim,
    __constant float2 *ic,
    int2 proj_dim,
    __constant float *sad,
    __constant float *scale,
    __constant int4 *ndevice,
    int offset
){
    uint i = get_global_id(0);
    uint j = get_global_id(1);
    uint k = get_global_id(2);

    if (i >= (*ndevice).x || j >= (*ndevice).y || k >= (*ndevice).z)
        return;

    // Index row major into the volume
    long vol_idx = i + (j * (*vol_dim).x) + (k * (*vol_dim).x * (*vol_dim).y);
    k += offset;

    // Get volume value from global memory
    float dev_vol_value = dev_vol[vol_idx];

    // offset volume coords
    float4 vp;
    vp.x = (*vol_offset).x + (i * (*vol_pix_spacing).x);        // Compiler should combine into 1 FMAD.
    vp.y = (*vol_offset).y + (j * (*vol_pix_spacing).y);        // Compiler should combine into 1 FMAD.
    vp.z = (*vol_offset).z + (k * (*vol_pix_spacing).z);        // Compiler should combine into 1 FMAD.

    // matrix multiply
    float4 ip;
    ip.x = (dev_matrix[0] * vp.x) + (dev_matrix[1] * vp.y) + (dev_matrix[2] * vp.z) + dev_matrix[3];
    ip.y = (dev_matrix[4] * vp.x) + (dev_matrix[5] * vp.y) + (dev_matrix[6] * vp.z) + dev_matrix[7];
    ip.z = (dev_matrix[8] * vp.x) + (dev_matrix[9] * vp.y) + (dev_matrix[10] * vp.z) + dev_matrix[11];

    // Change coordinate systems
    ip.x = (*ic).x + ip.x / ip.z;
    ip.y = (*ic).y + ip.y / ip.z;

    // Get pixel location from 2D image
    float2 pos;
    pos.y = ip.x;
    pos.x = ip.y;

    // Clip against image dimensions
    if (pos.x < 0 || pos.x >= (*proj_dim).x || pos.y < 0 || pos.y >= (*proj_dim).y)
        return;

    // Dot product
    float s = ((*nrm).x * vp.x) + ((*nrm).y * vp.y) + ((*nrm).z * vp.z);

    // Conebeam weighting factor
    s = (*sad) - s;
    s = ((*sad) * (*sad)) / (s * s);

    // Distance from interpolated pixel to [0][0] point
    float2 dist;
    dist.x = pos.x - floor(pos.x);
    dist.y = pos.y - floor(pos.y);

    // Variable used to store 4 pixel intensities
    float4 p[2][2];

    // Position in image of [0][0] in bilinear matrix
    int2 bilinear_pos;
    bilinear_pos.x = floor(pos.x);
    bilinear_pos.y = floor(pos.y);

    // Variable storing position to read image
    int2 interp_pos;

    for (int i=0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            interp_pos.x = bilinear_pos.x + i;
            interp_pos.y = bilinear_pos.y + j;

            // Checks if pixel is outside image
            if (interp_pos.x < 0)
                interp_pos.x = 0;
            else if(interp_pos.x >= (*proj_dim).x)
                interp_pos.x = (*proj_dim).x - 1;
            if (interp_pos.y < 0)
                interp_pos.y = 0;
            else if(interp_pos.y >= (*proj_dim).y)
                interp_pos.y = (*proj_dim).y - 1;

            p[i][j] = read_imagef(dev_img, dev_img_sampler, interp_pos);
        }
    }

    float intensity = p[0][0].x * (1 - dist.x) * (1  - dist.y) + p[0][1].x * (1 - dist.x) * dist.y + p[1][0].x * dist.x * (1 - dist.y) + p[1][1].x * dist.x * dist.y;

    // Place it into the volume
    dev_vol[vol_idx] = dev_vol_value + ((*scale) * s * intensity);
}


// FDK kernel - bicubic interpolation
__kernel void kernel_fdk_bicubic(
    __global float *dev_vol,
    __read_only image2d_t dev_img,
    __constant float *dev_matrix,
    __constant float4 *nrm,
    __constant float4 *vol_offset,
    __constant float4 *vol_pix_spacing,
    __constant int4 *vol_dim,
    __constant float2 *ic,
    __constant int2 *proj_dim,
    __constant float *sad,
    __constant float *scale,
    __constant int4 *ndevice,
    int offset
){
    uint i = get_global_id(0);
    uint j = get_global_id(1);
    uint k = get_global_id(2);

    if (i >= (*ndevice).x || j >= (*ndevice).y || k >= (*ndevice).z)
        return;

    // Index row major into the volume
    long vol_idx = i + (j * (*vol_dim).x) + (k * (*vol_dim).x * (*vol_dim).y);
    k += offset;

    // Get volume value from global memory
    float dev_vol_value = dev_vol[vol_idx];

    // offset volume coords
    float4 vp;
    vp.x = (*vol_offset).x + (i * (*vol_pix_spacing).x);        // Compiler should combine into 1 FMAD.
    vp.y = (*vol_offset).y + (j * (*vol_pix_spacing).y);        // Compiler should combine into 1 FMAD.
    vp.z = (*vol_offset).z + (k * (*vol_pix_spacing).z);        // Compiler should combine into 1 FMAD.

    // matrix multiply
    float4 ip;
    ip.x = (dev_matrix[0] * vp.x) + (dev_matrix[1] * vp.y) + (dev_matrix[2] * vp.z) + dev_matrix[3];
    ip.y = (dev_matrix[4] * vp.x) + (dev_matrix[5] * vp.y) + (dev_matrix[6] * vp.z) + dev_matrix[7];
    ip.z = (dev_matrix[8] * vp.x) + (dev_matrix[9] * vp.y) + (dev_matrix[10] * vp.z) + dev_matrix[11];

    // Change coordinate systems
    ip.x = (*ic).x + ip.x / ip.z;
    ip.y = (*ic).y + ip.y / ip.z;

    // Get pixel from 2D image
    float2 pos;
    pos.y = ip.x;
    pos.x = ip.y;

    // Clip against image dimensions
    if (pos.x < 0 || pos.x >= (*proj_dim).x || pos.y < 0 || pos.y >= (*proj_dim).y)
        return;

    // Dot product
    float s = ((*nrm).x * vp.x) + ((*nrm).y * vp.y) + ((*nrm).z * vp.z);

    // Conebeam weighting factor
    s = (*sad) - s;
    s = ((*sad) * (*sad)) / (s * s);

    // Distance from interpolated pixel to [0][0] point
    float2 dist;
    dist.x = pos.x - floor(pos.x);
    dist.y = pos.y - floor(pos.y);

    // Variable used to store 16 pixel intensities
    float4 p[4][4];

    // Position in image of [0][0] in bicubic matrix
    int2 bicubic_pos;
    bicubic_pos.x = floor(pos.x);
    bicubic_pos.y = floor(pos.y);

    // Variable storing position to read image
    int2 interp_pos;

    for (int i = -1; i < 3; i++)
    {
        for (int j = -1; j < 3; j++)
        {
            interp_pos.x = bicubic_pos.x + i;
            interp_pos.y = bicubic_pos.y + j;

            // Checks if pixel is outside image
            if (interp_pos.x < 0)
                interp_pos.x = 0;
            else if(interp_pos.x >= (*proj_dim).x)
                interp_pos.x = (*proj_dim).x - 1;
            if (interp_pos.y < 0)
                interp_pos.y = 0;
            else if(interp_pos.y >= (*proj_dim).y)
                interp_pos.y = (*proj_dim).y - 1;

            p[i + 1][j + 1] = read_imagef(dev_img, dev_img_sampler, interp_pos);
        }
    }

    float16 a;
    a.s0 = p[1][1].x;
    a.s1 = -   (0.5 * p[1][0].x)                     +  (0.5 * p[1][2].x);
    a.s2 = p[1][0].x -  (2.5 * p[1][1].x) +    (2 * p[1][2].x) -  (0.5 * p[1][3].x);
    a.s3 = -   (0.5 * p[1][0].x) + (1.5 * p[1][1].x) -  (1.5 * p[1][2].x) +  (0.5 * p[1][3].x);
    a.s4 = (0.5 * p[2][1].x) -  (0.5 * p[0][1].x);
    a.s5 = - (0.25 * p[2][0].x)                      + (0.25 * p[2][2].x)
        + (0.25 * p[0][0].x)                      - (0.25 * p[0][2].x);
    a.s6 = 
        (0.5 * p[2][0].x) - (1.25 * p[2][1].x) +         p[2][2].x  - (0.25 * p[2][3].x)
        -  (0.5 * p[0][0].x) + (1.25 * p[0][1].x) -         p[0][2].x  + (0.25 * p[0][3].x);
    a.s7 = 
        - (0.25 * p[2][0].x) + (0.75 * p[2][1].x) - (0.75 * p[2][2].x) + (0.25 * p[2][3].x)
        + (0.25 * p[0][0].x) - (0.75 * p[0][1].x) + (0.75 * p[0][2].x) - (0.25 * p[0][3].x);
    a.s8 =                      -  (0.5 * p[3][1].x)
        +    (2 * p[2][1].x)
        -  (2.5 * p[1][1].x)
        +         p[0][1].x;
    a.s9 =   (0.25 * p[3][0].x)                      - (0.25 * p[3][2].x)
        -         p[2][0].x                       +         p[2][2].x
        + (1.25 * p[1][0].x)                      - (1.25 * p[1][2].x)
        -  (0.5 * p[0][0].x)                      +  (0.5 * p[0][2].x);
    a.sa = -  (0.5 * p[3][0].x) + (1.25 * p[3][1].x) -         p[3][2].x  + (0.25 * p[3][3].x)
        +    (2 * p[2][0].x) -    (5 * p[2][1].x) +    (4 * p[2][2].x) -         p[2][3].x
        -  (2.5 * p[1][0].x) + (6.25 * p[1][1].x) -    (5 * p[1][2].x) + (1.25 * p[1][3].x)
        +         p[0][0].x  -  (2.5 * p[0][1].x) +    (2 * p[0][2].x) -  (0.5 * p[0][3].x);
    a.sb =   (0.25 * p[3][0].x) - (0.75 * p[3][1].x) + (0.75 * p[3][2].x) - (0.25 * p[3][3].x)
        -         p[2][0].x  +    (3 * p[2][1].x) -    (3 * p[2][2].x) +         p[2][3].x
        + (1.25 * p[1][0].x) - (3.75 * p[1][1].x) + (3.75 * p[1][2].x) - (1.25 * p[1][3].x)
        -  (0.5 * p[0][0].x) +  (1.5 * p[0][1].x) -  (1.5 * p[0][2].x) +  (0.5 * p[0][3].x);
    a.sc =                         (0.5 * p[3][1].x)
        -  (1.5 * p[2][1].x)
        +  (1.5 * p[1][1].x)
        -  (0.5 * p[0][1].x);
    a.sd = - (0.25 * p[3][0].x)                      + (0.25 * p[3][2].x)
        + (0.75 * p[2][0].x)                      - (0.75 * p[2][2].x)
        - (0.75 * p[1][0].x)                      + (0.75 * p[1][2].x)
        + (0.25 * p[0][0].x)                      - (0.25 * p[0][2].x);
    a.se =    (0.5 * p[3][0].x) - (1.25 * p[3][1].x) +         p[3][2].x  - (0.25 * p[3][3].x)
        -  (1.5 * p[2][0].x) + (3.75 * p[2][1].x) -    (3 * p[2][2].x) + (0.75 * p[2][3].x)
        +  (1.5 * p[1][0].x) - (3.75 * p[1][1].x) +    (3 * p[1][2].x) - (0.75 * p[1][3].x)
        -  (0.5 * p[0][0].x) + (1.25 * p[0][1].x) -         p[0][2].x  + (0.25 * p[0][3].x);
    a.sf = - (0.25 * p[3][0].x) + (0.75 * p[3][1].x) - (0.75 * p[3][2].x) + (0.25 * p[3][3].x)
        + (0.75 * p[2][0].x) - (2.25 * p[2][1].x) + (2.25 * p[2][2].x) - (0.75 * p[2][3].x)
        - (0.75 * p[1][0].x) + (2.25 * p[1][1].x) - (2.25 * p[1][2].x) + (0.75 * p[1][3].x)
        + (0.25 * p[0][0].x) - (0.75 * p[0][1].x) + (0.75 * p[0][2].x) - (0.25 * p[0][3].x);

    float intensity =
        a.s0                             + (a.s1 * dist.y)                            + (a.s2 * dist.y * dist.y)                            + (a.s3 * dist.y * dist.y * dist.y) +
        (a.s4 * dist.x)                   + (a.s5 * dist.x * dist.y)                   + (a.s6 * dist.x * dist.y * dist.y)                   + (a.s7 * dist.x * dist.y * dist.y * dist.y) +
        (a.s8 * dist.x * dist.x)          + (a.s9 * dist.x * dist.x * dist.y)          + (a.sa * dist.x * dist.x * dist.y * dist.y)          + (a.sb * dist.x * dist.x * dist.y * dist.y * dist.y) +
        (a.sc * dist.x * dist.x * dist.x) + (a.sd * dist.x * dist.x * dist.x * dist.y) + (a.se * dist.x * dist.x * dist.x * dist.y * dist.y) + (a.sf * dist.x * dist.x * dist.x * dist.y * dist.y * dist.y);

    // Place it into the volume
    dev_vol[vol_idx] = dev_vol_value + ((*scale) * s * intensity);
}

__kernel void 
kernel_fdk (
    __global float *dev_vol,
    __read_only image2d_t dev_img,
    __constant float *dev_matrix,
    __constant float4 *nrm,
    __constant float4 *vol_offset,
    __constant float4 *vol_pix_spacing,
    __constant int4 *vol_dim,
    __constant float2 *ic,
    __constant int2 *proj_dim,
    __constant float *sad,
    __constant float *scale,
    __constant int4 *ndevice,
    int offset
)
{
    
}
#endif

__kernel void 
fdk_kernel_nn (
    __global float *dev_vol,
    __global float *dev_img,
    __constant float *dev_matrix,
    int4 vol_dim,
    __constant float *vol_offset,
    __constant float *vol_spacing,
    int2 proj_dim,
    __constant float *nrm,
    __constant float *ic,
    const float sad,
    const float scale
)
{
    uint id = get_global_id(0);
    uint id_l = get_local_id(0);

    int k = id / vol_dim.x / vol_dim.y;
    int j = (id - (k * vol_dim.x * vol_dim.y)) / vol_dim.x;
    int i = id - k * vol_dim.x * vol_dim.y - j * vol_dim.x;

    if (k >= vol_dim.z) {
        return;
    }

    // Get volume value from global memory
    float dev_vol_value = dev_vol[id];

    // Get (x,y,z) coordinates
    float4 vp;
    vp.x = vol_offset[0] + (i * vol_spacing[0]);
    vp.y = vol_offset[1] + (j * vol_spacing[1]);
    vp.z = vol_offset[2] + (k * vol_spacing[2]);

    // Matrix multiplication
    float4 ip;
    ip.x = (dev_matrix[0] * vp.x) + (dev_matrix[1] * vp.y) + (dev_matrix[2] * vp.z) + dev_matrix[3];
    ip.y = (dev_matrix[4] * vp.x) + (dev_matrix[5] * vp.y) + (dev_matrix[6] * vp.z) + dev_matrix[7];
    ip.z = (dev_matrix[8] * vp.x) + (dev_matrix[9] * vp.y) + (dev_matrix[10] * vp.z) + dev_matrix[11];

    // Change coordinate systems
    ip.x = ic[0] + ip.x / ip.z;
    ip.y = ic[1] + ip.y / ip.z;

    // Retrieve pixel location from 2D image
    int2 pos;
    pos.y = convert_int_rtn (ip.x);
    pos.x = convert_int_rtn (ip.y);

    // Clip against image dimensions
    if (pos.x < 0 || pos.x >= proj_dim.x || pos.y < 0 || pos.y >= proj_dim.y)
    {
        return;
    }

    // Get pixel from image
    //float4 voxel_data = read_imagef(dev_img, dev_img_sampler, pos);
    float pix_val = dev_img[pos.y * proj_dim.x + pos.x];

    // Get distance to voxel projected to panel normal 
    float s = (nrm[0] * vp.x) + (nrm[1] * vp.y) + (nrm[2] * vp.z);

    // Conebeam weighting factor
    s = sad - s;
    s = (sad * sad) / (s * s);

    // Weight pixel and backproject into volume
    dev_vol[id] = dev_vol_value + (scale * s * pix_val);
}
