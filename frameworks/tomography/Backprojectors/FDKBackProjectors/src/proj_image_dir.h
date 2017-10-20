/* -----------------------------------------------------------------------
   See COPYRIGHT.TXT and LICENSE.TXT for copyright and license information
   ----------------------------------------------------------------------- */
#ifndef _proj_image_dir_h_
#define _proj_image_dir_h_

//#include "plmbase_config.h"
//#include "src\ui\img_labels_t.h"

class Proj_image;

class PLMBASE_API Proj_image_dir
{
public:
    Proj_image_dir (const char *dir);
    ~Proj_image_dir ();

public:
    char *dir;             /* Dir containing images, maybe not xml file */
    int num_proj_images;
    char **proj_image_list;

    char *xml_file;
    char *img_pat;
    char *mat_pat;

    double xy_offset[2];

public:
    Proj_image* load_image (int index);
	Proj_image* load_fromBuf (float* buf, img_labels_t* lab,const double xy_offset[2]);
    
	void select (int first, int skip, int last);
    void set_xy_offset (const double xy_offset[2]);

private:
    void clear_filenames ();
    void find_pattern (void);
    void harden_filenames ();
    void load_filenames (const char *dir);
};

#endif
