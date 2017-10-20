/* -----------------------------------------------------------------------
   See COPYRIGHT.TXT and LICENSE.TXT for copyright and license information
   ----------------------------------------------------------------------- */
#include "plmbase_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file_util.h"
#include "hnd_io.h"
#include "path_util.h"
#include "plm_math.h"
#include "plm_path.h"
#include "proj_image.h"
#include "proj_matrix.h"

Proj_image::Proj_image ()
{
	printf("Proj_image, empty\n");
    this->init ();
}

Proj_image::Proj_image (float* buf, img_labels_t* lab, const double xy_offset[2]){
	//printf("proj_image, buf\n");
	this->init ();
    this->xy_offset[0] = xy_offset[0];
    this->xy_offset[1] = xy_offset[1];
	this->load_buffer(buf,lab);
 }

Proj_image::Proj_image (
    const char* img_filename,
    const double xy_offset[2]
)
{
	printf("proj_image, XY\n");
    this->init ();
    this->xy_offset[0] = xy_offset[0];
    this->xy_offset[1] = xy_offset[1];
    this->load (img_filename, 0);
}

Proj_image::Proj_image (
    const char* img_filename,
    const char* mat_filename
)
{
	printf("proj_image, fname fname\n");
    this->init ();
    this->load (img_filename, mat_filename);
}

Proj_image::Proj_image (
    const std::string& img_filename,
    const std::string& mat_filename
)
{
    this->init ();
    this->load (img_filename, mat_filename);
}

Proj_image::~Proj_image ()
{
    this->clear ();
}

void
Proj_image::init ()
{
    dim[0] = dim[1] = 0;
    xy_offset[0] = xy_offset[1] = 0;
    pmat = 0;
    img = 0;
}

void
Proj_image::clear ()
{
    if (this->pmat) {
        free (this->pmat);
    }
    if (this->img) {
        free (this->img);
    }
}

bool
Proj_image::have_image ()
{
    return (this->img != 0);
}


void
	Proj_image::load_buffer(float * myBuf, img_labels_t* myLabel){

	//raw_load
    size_t rc;
    uint64_t fs;

    /*  just copy pointer */
    this->img = myBuf;

    /* Set image size */
	this->dim[1] = 1024;
	this->dim[0] = 768;

	//	mat_load
	//printf("mat_load\n");
    //FILE* fp;
    int i;
    float f;
    Proj_matrix *pmat;

	//printf("mat_load: allocating proj_matrix\n");
    /* Allocate memory */
    pmat = new Proj_matrix;
	//printf("mat_load: allocated proj_matrix\n");
    /* Open file */
    //fp = fopen (mat_filename,"r");
    //if (!fp) {
    //    fprintf (stderr, "Can't open file %s for read\n", mat_filename);
    //    exit (-1);
    //}
    /* Load image center */
    //for (i = 0; i < 2; i++) {
    //    if (1 != fscanf (fp, "%g", &f)) {
    //        fprintf (stderr, "Couldn't parse file %s as a matrix [1,%d]\n", 
    //                 mat_filename, i);
    //        exit (-1);
    //    }
    //    pmat->ic[i] = (double) f;
    //}
	//printf("mat_load: primo memcpy\n");
	memcpy(pmat->ic,myLabel->inter,2*sizeof(double));
	//printf("mat_load: secondo memcpy\n");

	//pmat->ic[0]=myLabel->inter[0];
	//pmat->ic[1]=myLabel->inter[1];

	/* Load projection matrix */
	memcpy(pmat->matrix,myLabel->totalMatrix,12*sizeof(double));
/*
    for (i = 0; i < 12; i++) {
        if (1 != fscanf (fp, "%g", &f)) {
            fprintf (stderr, "Couldn't parse file %s as a matrix [2,%d]\n", 
                     mat_filename, i);
            exit (-1);
        }
        pmat->matrix[i] = (double) f;
    }*/
    /* Load sad */
	pmat->sad = myLabel->RzRyRx[11];
    //if (1 != fscanf (fp, "%g", &f)) {
    //    fprintf (stderr, "Couldn't load sad from %s\n", mat_filename);
    //    exit (-1);
    //}
    //pmat->sad = (double) f;
    ///* Load sid */
    //if (1 != fscanf (fp, "%g", &f)) {
    //    fprintf (stderr, "Couldn't load sad from %s\n", mat_filename);
    //    exit (-1);
    //}
    //pmat->sid = (double) f;
	pmat->sid = 1 / myLabel->intrinsic_val[2];

    ///* Load nrm vector */
    //for (i = 0; i < 3; i++) {
    //    if (1 != fscanf (fp, "%g", &f)) {
    //        fprintf (stderr, "Couldn't parse file %s as a matrix [1,%d]\n", 
    //                 mat_filename, i);
    //        exit (-1);
    //    }
    //    pmat->nrm[i] = (double) f;
    //}
	memcpy(pmat->nrm,myLabel->nvect,3*sizeof(double));

    //fclose (fp);
    this->pmat = pmat;
   	//EXTRINSIC INTRINSIC NOT READED....?!?!?!
}

/* -----------------------------------------------------------------------
   Private functions
   ----------------------------------------------------------------------- */
static void
raw_load (Proj_image *proj, const char* img_filename)
{
    FILE* fp;
    size_t rc;
    uint64_t fs;

    if (!proj) return;

    /* Open file */
    fp = fopen (img_filename,"rb");
    if (!fp) {
        fprintf (stderr, "Can't open file %s for read\n", img_filename);
        exit (-1);
    }
    
    /* Malloc memory */
    fs = file_size (img_filename);
    proj->img = (float*) malloc (fs);
    if (!proj->img) {
        fprintf (stderr, "Couldn't malloc memory for input image\n");
        exit (-1);
    }

    /* Guess image size */
    switch (fs) {
    case (512*384*sizeof(float)):
        proj->dim[0] = 512;
        proj->dim[1] = 384;
        break;
    case (1024*384*sizeof(float)):
        proj->dim[0] = 1024;
        proj->dim[1] = 384;
        break;
    case (1024*768*sizeof(float)):
        //proj->dim[0] = 1024;
        //proj->dim[1] = 768;
		//gio change for cnao cbct raw (Float type)
        proj->dim[1] = 1024;
        proj->dim[0] = 768;
        break;
    case (2048*1536*sizeof(float)):
        proj->dim[0] = 2048;
        proj->dim[1] = 1536;
        break;
    default:
        proj->dim[0] = 1024;
        proj->dim[1] = fs / (1024 * sizeof(float));
        break;
    }

    /* Load pixels */
    rc = fread (proj->img, sizeof(float), proj->dim[0] * proj->dim[1], fp);
    if (rc != (size_t) (proj->dim[0] * proj->dim[1])) {
        fprintf (stderr, "Couldn't load raster data for %s\n",
                 img_filename);
        exit (-1);
    }
    fclose (fp);
}


static void
raw_save (Proj_image *proj, const char* img_filename)
{
    FILE* fp;
    
    fp = fopen (img_filename, "wb");
    if (!fp) {
        fprintf (stderr, "Can't open file %s for write\n", img_filename);
        exit (-1);
    }

    fwrite (proj->img, sizeof(float), proj->dim[0]*proj->dim[1], fp);
    fclose (fp);
}

static void
pfm_load (Proj_image *proj, const char* img_filename)
{
    FILE* fp;
    char buf[1024];
    size_t rc;

    if (!proj) return;

    fp = fopen (img_filename,"rb");
    if (!fp) {
        fprintf (stderr, "Can't open file %s for read\n", img_filename);
        exit (-1);
    }

    /* Verify that it is pfm */
    fgets (buf, 1024, fp);
    if (strncmp(buf, "Pf", 2)) {
        fprintf (stderr, "Couldn't parse file %s as an image [1]\n",
                 img_filename);
        exit (-1);
    }

    /* Get image resolution */
    fgets (buf, 1024, fp);
    if (2 != sscanf (buf, "%d %d", &proj->dim[0], &proj->dim[1])) {
        fprintf (stderr, "Couldn't parse file %s as an image [2]\n", 
                 img_filename);
        exit (-1);
    }
    /* Skip third line */
    fgets (buf, 1024, fp);

    /* Malloc memory */
    proj->img = (float*) malloc (sizeof(float) * proj->dim[0] * proj->dim[1]);
    if (!proj->img) {
        fprintf (stderr, "Couldn't malloc memory for input image\n");
        exit (-1);
    }

    /* Load pixels */
    rc = fread (proj->img, sizeof(float), proj->dim[0] * proj->dim[1], fp);
    if (rc != (size_t) (proj->dim[0] * proj->dim[1])) {
        fprintf (stderr, "Couldn't load raster data for %s\n",
                 img_filename);
        exit (-1);
    }
    fclose (fp);
}

static void
pfm_save (Proj_image *proj, const char* img_filename)
{
    FILE* fp;
    
    make_directory_recursive (img_filename);
    fp = fopen (img_filename, "wb");
    if (!fp) {
        fprintf (stderr, "Can't open file %s for write\n", img_filename);
        exit (-1);
    }

    fprintf (fp, 
        "Pf\n"
        "%d %d\n"
        "-1\n",
        proj->dim[0], proj->dim[1]);

    fwrite (proj->img, sizeof(float), proj->dim[0]*proj->dim[1], fp);
    fclose (fp);
}

static void
pgm_save (Proj_image *proj, const char* img_filename)
{
    FILE* fp;
    int i;
    
    make_directory_recursive (img_filename);
    fp = fopen (img_filename, "wb");
    if (!fp) {
        fprintf (stderr, "Can't open file %s for write\n", img_filename);
        exit (-1);
    }

    fprintf (fp, 
        "P2\n"
        "# Created by plastimatch\n"
        "%d %d\n"
        "65535\n",
        proj->dim[0],
        proj->dim[1]
    );

    for (i = 0; i < proj->dim[0]*proj->dim[1]; i++) {
        float v = proj->img[i];
        if (v >= 65535) {
            v = 65535;
        } else if (v < 0) {
            v = 0;
        }
        fprintf (fp,"%lu ", ROUND_INT(v));
        if (i % 25 == 24) {
            fprintf (fp,"\n");
        }
    }
    fclose (fp);
}

static void
mat_load (Proj_image *proj, const char* mat_filename)
{
	printf("mat_load\n");
    FILE* fp;
    int i;
    float f;
    Proj_matrix *pmat;

    if (!proj) return;

    /* Allocate memory */
    pmat = new Proj_matrix;

    /* Open file */
    fp = fopen (mat_filename,"r");
    if (!fp) {
        fprintf (stderr, "Can't open file %s for read\n", mat_filename);
        exit (-1);
    }
    /* Load image center */
    for (i = 0; i < 2; i++) {
        if (1 != fscanf (fp, "%g", &f)) {
            fprintf (stderr, "Couldn't parse file %s as a matrix [1,%d]\n", 
                     mat_filename, i);
            exit (-1);
        }
        pmat->ic[i] = (double) f;
		printf("%f\n",pmat->ic[i]);
    }
    /* Load projection matrix */
    for (i = 0; i < 12; i++) {
        if (1 != fscanf (fp, "%g", &f)) {
            fprintf (stderr, "Couldn't parse file %s as a matrix [2,%d]\n", 
                     mat_filename, i);
            exit (-1);
        }
        pmat->matrix[i] = (double) f;
		printf("%f\n",pmat->matrix[i]);
    }
    /* Load sad */
    if (1 != fscanf (fp, "%g", &f)) {
        fprintf (stderr, "Couldn't load sad from %s\n", mat_filename);
        exit (-1);
    }
    pmat->sad = (double) f;
	printf("%f\n",pmat->sad);
    /* Load sid */
    if (1 != fscanf (fp, "%g", &f)) {
        fprintf (stderr, "Couldn't load sad from %s\n", mat_filename);
        exit (-1);
    }

    pmat->sid = (double) f;
	printf("%f\n",pmat->sid);
    /* Load nrm vector */
    for (i = 0; i < 3; i++) {
        if (1 != fscanf (fp, "%g", &f)) {
            fprintf (stderr, "Couldn't parse file %s as a matrix [1,%d]\n", 
                     mat_filename, i);
            exit (-1);
        }
        pmat->nrm[i] = (double) f;
		printf("%f\n",pmat->nrm[i]);
    }
    fclose (fp);

    proj->pmat = pmat;
}

static void
mat_load_by_img_filename (Proj_image* proj, const char* img_filename)
{
    /* No mat file, so try to find automatically */
    size_t img_filename_len = strlen (img_filename);
    if (img_filename_len > 4) {
        char *mat_fn = strdup (img_filename);
        strcpy (&mat_fn[img_filename_len-4], ".txt");
        if (file_exists (mat_fn)) {
            mat_load (proj, mat_fn);
        }
        free (mat_fn);
    }
}

void
Proj_image::load_pfm (const char* img_filename, const char* mat_filename)
{
    if (!img_filename) return;

    pfm_load (this, img_filename);

    if (mat_filename) {
        mat_load (this, mat_filename);
    } else {
        mat_load_by_img_filename (this, img_filename);
    }
}

void
Proj_image::load_raw (const char* img_filename, const char* mat_filename)
{
    if (!img_filename) return;

    raw_load (this, img_filename);

    if (mat_filename) {
        mat_load (this, mat_filename);
    } else {
        mat_load_by_img_filename (this, img_filename);
    }
}

void
Proj_image::load_hnd (const char* img_filename)
{
    if (!img_filename) return;

    hnd_load (this, img_filename, this->xy_offset);
    if (this->img == 0) {
        this->clear ();
    }
}

/* -----------------------------------------------------------------------
   Public functions
   ----------------------------------------------------------------------- */
void
proj_image_create_pmat (Proj_image *proj)
{
    /* Allocate memory */
    proj->pmat = new Proj_matrix;
}

void
proj_image_create_img (Proj_image *proj, int dim[2])
{
    proj->dim[0] = dim[0];
    proj->dim[1] = dim[1];
    proj->img = (float*) malloc (sizeof(float) * proj->dim[0] * proj->dim[1]);
}

void
proj_image_debug_header (Proj_image *proj)
{
    int i;
    printf ("Image center: %g %g\n", proj->pmat->ic[0], proj->pmat->ic[1]);
    printf ("Projection matrix: ");
    for (i = 0; i < 12; i++) {
        printf ("%g ", proj->pmat->matrix[i]);
    }
    printf ("\n");
}

void
proj_image_stats (Proj_image *proj)
{
    int i, num;
    float min_val, max_val;
    double sum = 0.0;

    if (!proj) {
        printf ("No image.\n");
        return;
    }

    num = proj->dim[0]*proj->dim[1];
    if (!proj->img || num == 0) {
        printf ("No image.\n");
        return;
    }
    
    min_val = max_val = proj->img[0];
    for (i = 0; i < num; i++) {
        float v = proj->img[i];
        if (min_val > v) min_val = v;
        if (max_val < v) max_val = v;
        sum += v;
    }

    printf ("MIN %f AVE %f MAX %f NUM %d\n",
            min_val, (float) (sum / num), max_val, num);
}

void
Proj_image::load (
    const char* img_filename,
    const char* mat_filename
)
{
    char tmp[_MAX_PATH];

    /* If not specified, try to guess the mat_filename */
    if (!mat_filename || mat_filename[0] == 0) {
        strncpy (tmp, img_filename, _MAX_PATH);
        strip_extension (tmp);
        strncat (tmp, ".txt", _MAX_PATH);
        if (file_exists (tmp)) {
            mat_filename = tmp;
        }
    }

    if (extension_is (img_filename, ".pfm")) {
        load_pfm (img_filename, mat_filename);
    }
    else if (extension_is (img_filename, ".raw")) {
        load_raw (img_filename, mat_filename);
    }
    else if (extension_is (img_filename, ".hnd")) {
        load_hnd (img_filename);
    }
}


void
Proj_image::load (
    const std::string& img_filename,
    const std::string& mat_filename
)
{
    load (img_filename.c_str(), mat_filename.c_str());
}

void
proj_image_save (
    Proj_image *proj,
    const char *img_filename,
    const char *mat_filename
)
{
    if (img_filename) {
        if (extension_is (img_filename, ".pfm")) {
            pfm_save (proj, img_filename);
        }
        else if (extension_is (img_filename, ".raw")) {
            raw_save (proj, img_filename);
        }
        else if (extension_is (img_filename, ".pgm")) {
            pgm_save (proj, img_filename);
        }
#if defined (commentout)
        else if (extension_is (img_filename, "mha.")) {
            mha_save (proj, img_filename);
        }
#endif
    }
    if (mat_filename) {
        proj_matrix_save (proj->pmat, mat_filename);
    }
}

