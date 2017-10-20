/* -----------------------------------------------------------------------
   See COPYRIGHT.TXT and LICENSE.TXT for copyright and license information
   ----------------------------------------------------------------------- */
#include "plmbase_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dir_list.h"
#include "file_util.h"
#include "path_util.h"
#include "plm_path.h"
#include "proj_image.h"
#include "proj_image_dir.h"

Proj_image_dir::Proj_image_dir (const char *dir)
{
    char xml_file[_MAX_PATH];

    /* Initialize members */
    this->dir = 0;
    this->num_proj_images = 0;
    this->proj_image_list = 0;
    this->xml_file = 0;
    this->img_pat = 0;
    this->mat_pat = 0;
    this->xy_offset[0] = this->xy_offset[1] = 0;

    /* Look for ProjectionInfo.xml */
    snprintf (xml_file, _MAX_PATH, "%s/%s", dir, "ProjectionInfo.xml");
    if (file_exists (xml_file)) {
	this->xml_file = strdup (xml_file);
    }

    /* Load list of file names */
    this->load_filenames (dir);

    /* If base directory doesn't contain images, look in Scan0 directory */
    if (this->num_proj_images == 0) {
	char scan0_dir[_MAX_PATH];
	snprintf (scan0_dir, _MAX_PATH, "%s/%s", dir, "Scan0");

	/* Load list of file names */
	this->load_filenames (scan0_dir);
    }

    /* No images in either base directory or Scan 0, so give up. */
    if (this->num_proj_images == 0) {
	return;
    }

    /* Found images, try to find pattern */
    this->find_pattern ();

    /* Convert relative paths to absolute paths */
    this->harden_filenames ();
}

Proj_image_dir::~Proj_image_dir ()
{
    if (this->img_pat) free (this->img_pat);
    if (this->xml_file) free (this->xml_file);
    this->clear_filenames ();
}


/* -----------------------------------------------------------------------
   Private functions
   ----------------------------------------------------------------------- */

/* This utility function tries to guess the pattern of filenames 
   within a directory.  The patterns are of the form: 
   
     "XXXXYYYY.ZZZ"
     
   where XXXX is a prefix, YYYY is a number, and .ZZZ is the extension 
   of a known type (either .hnd, .pfm, or .raw).   Returns a pattern
   which can be used with sprintf, such as:

     "XXXX%04d.ZZZ"

   Caller must free patterns.
*/
void
Proj_image_dir::find_pattern ()
{
    int i;

    /* Search for appropriate entry */
    for (i = 0; i < this->num_proj_images; i++) {
	char *entry = this->proj_image_list[i];
	int rc;
	char prefix[2048], num[2048];

	/* Look for prefix + number at beginning of filename */
	rc = sscanf (entry, "%2047[^0-9]%2047[0-9]", prefix, num);
	if (rc == 2) {
	    char num_pat[5];
	    char *suffix;

	    /* Two cases: if num has a leading 0, we format such as 
	       %05d.  If, we format as %d. */
	    if (num[0] == '0') {
		strcpy (num_pat, "%0_d");
		num_pat[2] = '0' + ((char) strlen (num));
	    } else {
		strcpy (num_pat, "%d");
	    }

	    /* Find suffix */
	    suffix = &entry[strlen(prefix) + strlen(num)];
		
	    /* Create pattern */
	    this->img_pat = (char*) malloc (
		strlen (this->dir) + 1 + strlen (prefix) 
		+ strlen (num_pat) + strlen (suffix) + 1);
	    sprintf (this->img_pat, "%s/%s%s%s", this->dir, 
		prefix, num_pat, suffix);

	    /* Done! */
	    break;
	}
    }
    return;
}

void
Proj_image_dir::load_filenames (
    const char *dir
)
{
    int i;
    Dir_list dir_list;

    if (this->dir) {
	free (this->dir);
	this->dir = 0;
    }

    dir_list.load (dir);

    this->dir = strdup (dir);
    this->num_proj_images = 0;
    this->proj_image_list = 0;

    for (i = 0; i < dir_list.num_entries; i++) {
	char *entry = dir_list.entries[i];
	if (extension_is (entry, ".hnd") || extension_is (entry, ".pfm") 
	    || extension_is (entry, ".raw"))
	{
	    this->num_proj_images ++;
	    this->proj_image_list = (char**) realloc (
		this->proj_image_list,
		this->num_proj_images * sizeof (char*));
	    this->proj_image_list[this->num_proj_images-1] = strdup (entry);
	}
    }
}

void
Proj_image_dir::harden_filenames ()
{
    int i;

    for (i = 0; i < this->num_proj_images; i++) {
	char img_file[_MAX_PATH];
	char *entry = this->proj_image_list[i];
	snprintf (img_file, _MAX_PATH, "%s/%s", this->dir, entry);
	this->proj_image_list[i] = strdup (img_file);
	free (entry);
    }
}

void
Proj_image_dir::clear_filenames ()
{
    int i;
    for (i = 0; i < this->num_proj_images; i++) {
	char *entry = this->proj_image_list[i];
	free (entry);
    }
    if (this->proj_image_list) free (this->proj_image_list);
    this->num_proj_images = 0;
    this->proj_image_list = 0;
}

/* -----------------------------------------------------------------------
   Public functions
   ----------------------------------------------------------------------- */
void
Proj_image_dir::set_xy_offset (const double xy_offset[2])
{
    this->xy_offset[0] = xy_offset[0];
    this->xy_offset[1] = xy_offset[1];
}

void
Proj_image_dir::select (int first, int skip, int last)
{
    int i;

    if (this->num_proj_images == 0 || !this->img_pat) {
	return;
    }

    this->clear_filenames ();
    for (i = first; i <= last; i += skip) {
	char img_file[_MAX_PATH];
	snprintf (img_file, _MAX_PATH, this->img_pat, i);
	if (file_exists (img_file)) {
	    this->num_proj_images ++;
	    this->proj_image_list = (char**) realloc (
		this->proj_image_list, 
		this->num_proj_images * sizeof (char*));
	    this->proj_image_list[this->num_proj_images-1] = strdup (img_file);
	}
    }
}


  

Proj_image* 
Proj_image_dir:: load_fromBuf (
float* buf, img_labels_t* lab,const double xy_offset[2])
{
	  return new Proj_image (buf,lab, xy_offset);
}
    

Proj_image* 
Proj_image_dir::load_image (int index)
{
    if (index < 0 || index >= this->num_proj_images) {
	return 0;
    }

    /* mat file load not yet implemented -- Proj_image_dir only works 
       for hnd files */
	printf("%s\n",this->proj_image_list[index]);
    return new Proj_image (this->proj_image_list[index], this->xy_offset);

}
