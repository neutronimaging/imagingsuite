#ifndef __IO_NEXUS_H
#define __IO_NEXUS_H

#include "../kipl_global.h"
#include "../base/imagecast.h"
#include "../base/timage.h"
#include "../base/imageinfo.h"
#include "../strings/filenames.h"
#include "../base/kiplenums.h"

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <cstring>
#include <sstream>
#include <vector>
#include <map>

#include <nexus/napi.h>
#include <nexus/NeXusFile.hpp>

#ifdef _MSC_VER
#include <io.h>
#else
#include <unistd.h>
#endif


using std::cout;
using std::endl;
using std::map;
using std::multimap;
using std::string;
using std::vector;

using namespace std;



namespace kipl { namespace io {

/// \brief Read the image data content from Nexus file and stores it in the data type specified by the image
///	\param src the image to be stored
///	\param fname file name of the destination file (including extension .hdf)
/// \return 1 if successful, 0 if fail
template <class ImgType, size_t NDim>
int ReadNexus(kipl::base::TImage<ImgType,NDim> &img, const char *fname, size_t number, size_t const * const nCrop) {


    std::stringstream msg;

    NeXus::File file(fname);
    vector<NeXus::AttrInfo> attr_infos = file.getAttrInfos();


    file.openGroup("entry", "NXentry");
    attr_infos = file.getAttrInfos(); // check how many or assuming that there is only one?
    map<string, string> entries = file.getEntries();


    for (map<string,string>::const_iterator it = entries.begin();it != entries.end(); ++it) {
        if(it->second=="NXdata") {
//            std::cout << " found data!"<< std::endl;
            file.openGroup(it->first, it->second);
            attr_infos = file.getAttrInfos();
//            cout << "Number of group attributes for "<< it->second <<": " << attr_infos.size() << endl;

            map<string, string> entries_data = file.getEntries();
//            cout << "Group contains " << entries_data.size() << " items" << endl;

            for (map<string,string>::const_iterator it_data = entries_data.begin();
                 it_data != entries_data.end(); it_data++) {
//                std::cout << it_data->first << ": " << it_data->second << std::endl;
                file.openData(it_data->first);
                attr_infos = file.getAttrInfos();
//                cout << "Number of data attributes for " << it_data->first <<  ": "<< attr_infos.size() <<std::endl;
                for (vector<NeXus::AttrInfo>::iterator it_att = attr_infos.begin(); it_att != attr_infos.end(); it_att++) {
//                  cout << "   " << it_att->name << " = ";
//                  if (it_att->type == NeXus::CHAR) {
//                    cout << file.getStrAttr(*it_att);
//                  }
//                  cout << endl;

                  if (it_att->name=="signal"){
//                      std::cout << "found plottable data!!" << std::endl;

                      size_t img_size[3];

                      if (nCrop==NULL)
                      {
                          img_size[0] = file.getInfo().dims[2];
                          img_size[1] = file.getInfo().dims[1];
                          img_size[2] = file.getInfo().dims[0];
                      }
                      else
                      {
                          img_size[0] = nCrop[2]-nCrop[0]; // missing exceptions
                          img_size[1] = nCrop[3]-nCrop[1];
                          img_size[2] = file.getInfo().dims[0];
                      }

                      int16_t *slab=new int16_t[img_size[0]*img_size[1]]; // assuming images are all int_16.. but possibly this can be read from nexus info..

                      vector<int> slab_start;
                      slab_start.push_back(number);
                      if (nCrop==NULL)
                      {
                          slab_start.push_back(0);
                          slab_start.push_back(0);
                      }
                      else
                      {
                          slab_start.push_back(nCrop[1]);
                          slab_start.push_back(nCrop[0]);

                      }
                      vector<int> slab_size;
                      slab_size.push_back(1);
                      slab_size.push_back(img_size[1]);
                      slab_size.push_back(img_size[0]);


                      try{
                        file.getSlab(slab, slab_start, slab_size);
                      }
                      catch (const std::bad_alloc & E) {
                              msg.str("");
                              msg<<"ReadNexus file.getSlab caused an STL exception: "<<E.what();
                              throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
                      }
                      catch (...){
                          msg.str("");
                          msg<<"ReadNexus file.getSlab caused an unknown exception: ";
                          throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
                      }




                      size_t size2D[2] = {img_size[0], img_size[1]};
                      img.Resize(size2D);
                      ImgType *pslice = img.GetDataPtr();

                      // casting to ImgType
                      for (size_t i=0; i<img.Size();++i){
                          pslice[i] = static_cast<ImgType>(slab[i]);
                      }


//                      kipl::io::WriteTIFF(img, "cropped_img.tif");


//                      break;
                      }
                  }
                file.closeData();
                }

            file.closeGroup();
            }

    }

    file.close(); // close the file. hopefully


    return 0;
}

/// \brief Read the image data content from Nexus file and stores it in the data type specified by the image
///	\param src the image to be stored
///	\param fname file name of the destination file (including extension .hdf)
/// \return 1 if successful, 0 if fail
template <class ImgType, size_t NDim>
int ReadNexusStack(kipl::base::TImage<ImgType,NDim> &img, const char *fname, size_t start, size_t end, size_t const * const nCrop) {


    std::stringstream msg;

    NeXus::File file(fname);
    vector<NeXus::AttrInfo> attr_infos = file.getAttrInfos();


    file.openGroup("entry", "NXentry");
    attr_infos = file.getAttrInfos(); // check how many or assuming that there is only one?
    map<string, string> entries = file.getEntries();


    for (map<string,string>::const_iterator it = entries.begin();it != entries.end(); ++it) {
        if(it->second=="NXdata") {
//            std::cout << " found data!"<< std::endl;
            file.openGroup(it->first, it->second);
            attr_infos = file.getAttrInfos();
//            cout << "Number of group attributes for "<< it->second <<": " << attr_infos.size() << endl;

            map<string, string> entries_data = file.getEntries();
//            cout << "Group contains " << entries_data.size() << " items" << endl;

            for (map<string,string>::const_iterator it_data = entries_data.begin();
                 it_data != entries_data.end(); it_data++) {
//                std::cout << it_data->first << ": " << it_data->second << std::endl;
                file.openData(it_data->first);
                attr_infos = file.getAttrInfos();
//                cout << "Number of data attributes for " << it_data->first <<  ": "<< attr_infos.size() <<std::endl;
                for (vector<NeXus::AttrInfo>::iterator it_att = attr_infos.begin(); it_att != attr_infos.end(); it_att++) {
//                  cout << "   " << it_att->name << " = ";
//                  if (it_att->type == NeXus::CHAR) {
//                    cout << file.getStrAttr(*it_att);
//                  }
//                  cout << endl;

                  if (it_att->name=="signal"){
//                      std::cout << "found plottable data!!" << std::endl;

                      size_t img_size[3];

                      if (nCrop==NULL)
                      {
                          img_size[0] = file.getInfo().dims[2];
                          img_size[1] = file.getInfo().dims[1];
                          img_size[2] = file.getInfo().dims[0];
                      }
                      else
                      {
                          img_size[0] = nCrop[2]-nCrop[0]; // missing exceptions
                          img_size[1] = nCrop[3]-nCrop[1];
                          img_size[2] = file.getInfo().dims[0];
                      }

                      int16_t *slab=new int16_t[img_size[0]*img_size[1]*(end-start)]; // assuming images are all int_16.. but possibly this can be read from nexus info..

                      vector<int> slab_start;
                      slab_start.push_back(start);
                      if (nCrop==NULL)
                      {
                          slab_start.push_back(0);
                          slab_start.push_back(0);
                      }
                      else
                      {
                          slab_start.push_back(nCrop[1]);
                          slab_start.push_back(nCrop[0]);

                      }
                      vector<int> slab_size;
                      slab_size.push_back(end-start);
                      slab_size.push_back(img_size[1]);
                      slab_size.push_back(img_size[0]);


                      try{
                        file.getSlab(slab, slab_start, slab_size);
                      }
                      catch (const std::bad_alloc & E) {
                              msg.str("");
                              msg<<"ReadNexus file.getSlab caused an STL exception: "<<E.what();
                              throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
                      }
                      catch (...){
                          msg.str("");
                          msg<<"ReadNexus file.getSlab caused an unknown exception: ";
                          throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
                      }




                      size_t size3D[3] = {img_size[0], img_size[1], end-start};
                      img.Resize(size3D);
                      ImgType *pslice = img.GetDataPtr();

                      // casting to ImgType
                      for (size_t i=0; i<img.Size();++i){
                          pslice[i] = static_cast<ImgType>(slab[i]);
                      }


//                      kipl::io::WriteTIFF(img, "cropped_img.tif");


//                      break;
                      }
                  }
                file.closeData();
                }

            file.closeGroup();
            }

    }

    file.close(); // close the file. hopefully


    return 1;
}

template <class ImgType, size_t NDim>
int ReadNexus(kipl::base::TImage<ImgType,NDim> img, const char *fname) {



    std::stringstream msg;



    NeXus::File file(fname);

    vector<NeXus::AttrInfo> attr_infos = file.getAttrInfos();

    // look for default plottable data
    // check group attributes

    file.openGroup("entry", "NXentry");
    attr_infos = file.getAttrInfos(); // check how many or assuming that there is only one?
    map<string, string> entries = file.getEntries();

    for (map<string,string>::const_iterator it = entries.begin();        it != entries.end(); ++it) {
        if(it->second=="NXdata") {
            std::cout << " found data!"<< std::endl;
            file.openGroup(it->first, it->second);
            attr_infos = file.getAttrInfos();
            cout << "Number of group attributes for "<< it->second <<": " << attr_infos.size() << endl;

            map<string, string> entries_data = file.getEntries();
            cout << "Group contains " << entries_data.size() << " items" << endl;

            for (map<string,string>::const_iterator it_data = entries_data.begin();
                 it_data != entries_data.end(); it_data++)
            {
                std::cout << it_data->first << ": " << it_data->second << std::endl;
                file.openData(it_data->first);
                attr_infos = file.getAttrInfos();
                cout << "Number of data attributes for " << it_data->first <<  ": "<< attr_infos.size() <<std::endl;
                for (vector<NeXus::AttrInfo>::iterator it_att = attr_infos.begin();
                     it_att != attr_infos.end(); it_att++) {
                  cout << "   " << it_att->name << " = ";
                  if (it_att->type == NeXus::CHAR) {
                    cout << file.getStrAttr(*it_att);
                  }
                  cout << endl;

                  if (it_att->name=="signal"){
                      std::cout << "found plottable data!!" << std::endl;

                      // here I should check on the type but I already know that is int16....
//                      vector<ImgType> result; // it does want int16_t ImgType..
                      int16_t * result;


                      size_t img_size[3] = {static_cast<size_t>(file.getInfo().dims[2]), static_cast<size_t>(file.getInfo().dims[1]),static_cast<size_t>(file.getInfo().dims[0])};
                      result = new int16_t[img_size[0]*img_size[1]*img_size[2]];

                      try {
                        file.getData(result); // or it can be done with the std::vector (same result)
                      }
                      catch(kipl::base::KiplException &e)
                      {
                          msg.str("");
                          msg<<"ReadNexus file.getData caused a KiplException: "<<e.what();
                          throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
                      }
                      catch (const std::bad_alloc & e) {
                              msg.str("");
                              msg<<"ReadNexus file.getData caused an STL exception: "<<e.what();
                              throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
                      }
                      catch (...) {
                          msg<<"An unknown exception occurred ";
                          throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);

                      }

                      img.Resize(img_size);
                      ImgType * ptmp = img.GetDataPtr();

                      for (size_t i=0; i<img.Size(); ++i){
                          ptmp[i] = static_cast<ImgType>(result[i]); // copy result to img
                      }

//                      break;
                      }
                  }
                }

            }

    }

    file.close();




    return 1;
}

/// \brief Write the reconstructed data as NeXus file in float format
///	\param img the dataset to be written
///	\param fname file name of the destination file (including extension .hdf)
/// \return 1 if successful, 0 if fail
template <class ImgType, size_t NDim>
int WriteNexusFloat(kipl::base::TImage<ImgType,NDim> img, const char *fname, float p_size) {

//    int counts[50][1000], n_t=1000, n_p=50, dims[2], i;
//    float t[1000], phi[50];

    int dims[NDim] = {img.Size(2), img.Size(0), img.Size(1)};
    int  i = 1; // not sure what that is
    float *mysize = &p_size;

    NXhandle file_id;
/*
 * Read in data using local routines to populate phi and counts
 *
 * for example you may create a getdata() function and call
 *
 *      getdata (n_t, t, n_p, phi, counts);
 */
/* Open output file and output global attributes */
    NXopen (fname, NXACC_CREATE5, &file_id);
      NXputattr (file_id, "user_name", "Jane Doe", 10, NX_CHAR);
/* Open top-level NXentry group */
      NXmakegroup (file_id, "entry", "NXentry");
      NXopengroup (file_id, "entry", "NXentry");
/* Open NXdata group within NXentry group */
        NXmakegroup (file_id, "Data1", "NXdata");
        NXopengroup (file_id, "Data1", "NXdata");

///* Output time channels */
//          NXmakedata (file_id, "time_of_flight", NX_FLOAT32, 1, &n_t);
//          NXopendata (file_id, "time_of_flight");
//            NXputdata (file_id, t);
//            NXputattr (file_id, "units", "microseconds", 12, NX_CHAR);
//          NXclosedata (file_id);
///* Output detector angles */
//          NXmakedata (file_id, "polar_angle", NX_FLOAT32, 1, &n_p);
//          NXopendata (file_id, "polar_angle");
//            NXputdata (file_id, phi);
//            NXputattr (file_id, "units", "degrees", 7, NX_CHAR);
//          NXclosedata (file_id);

/* Output data */

          NXmakedata (file_id, "signal", NX_FLOAT32, 3, dims);
          NXopendata (file_id, "signal");
            NXputdata (file_id, img.GetDataPtr());

            NXputattr (file_id, "signal", &i, 1, NX_INT32);
//            NXputattr (file_id, "axes",  "polar_angle:time_of_flight", 26, NX_CHAR);
          NXclosedata (file_id);

/*      Pixel spacing */
         NXmakedata (file_id, "x_pixel_size", NX_FLOAT32, 1, &i);
         NXopendata (file_id, "x_pixel_size");
            NXputdata(file_id, mysize);
            NXputattr(file_id,"units","mm",2,NX_CHAR);
        NXclosedata(file_id);

/* Close NXentry and NXdata groups and close file */
        NXclosegroup (file_id);
      NXclosegroup (file_id);
    NXclose (&file_id);

    return 1;
}

/// \brief Write the reconstructed data as NeXus file in16 bit format
///	\param img the dataset to be written
///	\param fname file name of the destination file (including extension .hdf)
/// \param p_size pixel size of the reconstructed datas
/// \return 1 if successful, 0 if fail
template <class ImgType, size_t NDim>
int WriteNexus16bits(kipl::base::TImage<ImgType,NDim> &img, const char *fname, ImgType lo, ImgType hi, float p_size) {

//    int counts[50][1000], n_t=1000, n_p=50, dims[2], i;
//    float t[1000], phi[50];

    int dims[NDim] = {img.Size(2), img.Size(0), img.Size(1)};
    float *mysize = &p_size;
    int  i = 1;

    NXhandle file_id;
/*
 * Read in data using local routines to populate phi and counts
 *
 * for example you may create a getdata() function and call
 *
 *      getdata (n_t, t, n_p, phi, counts);
 */

/* Open output file and output global attributes */
    NXopen (fname, NXACC_CREATE5, &file_id);
      NXputattr (file_id, "user_name", "Jane Doe", 10, NX_CHAR);
/* Open top-level NXentry group */
      NXmakegroup (file_id, "entry", "NXentry");
      NXopengroup (file_id, "entry", "NXentry");
/* Open NXdata group within NXentry group */
        NXmakegroup (file_id, "Data1", "NXdata");
        NXopengroup (file_id, "Data1", "NXdata");


/* Output data */
        kipl::base::TImage<unsigned short, NDim> img_int;

       try {
        img_int = kipl::base::ImageCaster<unsigned short, ImgType, NDim>::cast(img,lo,hi);
        }
        catch (kipl::base::KiplException &E) {
            throw kipl::base::KiplException(E.what(),__FILE__,__LINE__);
        }


          NXmakedata (file_id, "signal", NX_UINT16, 3, dims);
          NXopendata (file_id, "signal");
            NXputdata (file_id, img_int.GetDataPtr());
            NXputattr (file_id, "signal", &i, 1, NX_INT32);
//            NXputattr (file_id, "x_pixel_size", &p_size, 3, NX_FLOAT32);
//            NXputattr (file_id, "y_pixel_size", &p_size, 3, NX_FLOAT32);
//            NXputattr (file_id, "axes",  "polar_angle:time_of_flight", 26, NX_CHAR);
          NXclosedata (file_id);

/*      Pixel spacing */
       NXmakedata (file_id, "x_pixel_size", NX_FLOAT32, 1, &i);
       NXopendata (file_id, "x_pixel_size");
          NXputdata(file_id, mysize);
          NXputattr(file_id,"units","mm",2,NX_CHAR);
      NXclosedata(file_id);


/* Close NXentry and NXdata groups and close file */
        NXclosegroup (file_id);
      NXclosegroup (file_id);
    NXclose (&file_id);

    return 1;
}

/// \brief Prepare the file to be used for NeXus saving reconstructed data
///	\param fname file name of the destination file (including extension .hdf)
/// \param dims dimensions of the 3D image to be saved
/// \param p_size pixel size of the reconstructed datas
/// \return 1 if successful, 0 if fail
template <class ImgType, size_t NDim>
int PrepareNeXusFile(const char *fname, int *dims, float p_size) {
    // I have to understand where to call it!


    float *mysize = &p_size;
    int  i = 1;

    NXhandle file_id;

/* Open output file and output global attributes */
    NXopen (fname, NXACC_CREATE5, &file_id);
      NXputattr (file_id, "user_name", "Jane Doe", 10, NX_CHAR);
/* Open top-level NXentry group */
      NXmakegroup (file_id, "entry", "NXentry");
      NXopengroup (file_id, "entry", "NXentry");
/* Open NXdata group within NXentry group */
        NXmakegroup (file_id, "Data1", "NXdata");
        NXopengroup (file_id, "Data1", "NXdata");


kipl::base::TImage<ImgType, NDim> img(dims);
ImgType *pImg = img.GetDataPtr();


for (int i=0; i<img.Size(); i++)
    pImg[i] = static_cast<ImgType>(0);

          NXmakedata (file_id, "signal", NX_FLOAT32, 3, dims); // iniziamo con FLOAT32 e poi vediamo
          NXopendata (file_id, "signal");
            NXputdata (file_id, img.GetDataPtr());
            NXputattr (file_id, "signal", &i, 1, NX_INT32);
          NXclosedata (file_id);

/*      Pixel spacing */
       NXmakedata (file_id, "x_pixel_size", NX_FLOAT32, 1, &i);
       NXopendata (file_id, "x_pixel_size");
          NXputdata(file_id, mysize);
          NXputattr(file_id,"units","mm",2,NX_CHAR);
      NXclosedata(file_id);


/* Close NXentry and NXdata groups and close file */
        NXclosegroup (file_id);
      NXclosegroup (file_id);
    NXclose (&file_id);

    return 1;
}

/// todo: Add Doxygen information
template <class ImgType, size_t NDim>
int WriteNeXusStack(kipl::base::TImage<ImgType,NDim> &img, const char *fname, size_t start, size_t end) {
    std::stringstream msg;

    NeXus::File file(fname);
    vector<NeXus::AttrInfo> attr_infos = file.getAttrInfos();



    file.openGroup("entry", "NXentry");
    attr_infos = file.getAttrInfos(); // check how many or assuming that there is only one?
    map<string, string> entries = file.getEntries();


    for (map<string,string>::const_iterator it = entries.begin();it != entries.end(); ++it) {
        if(it->second=="NXdata") {

            file.openGroup(it->first, it->second);
            attr_infos = file.getAttrInfos();


            map<string, string> entries_data = file.getEntries();


            for (map<string,string>::const_iterator it_data = entries_data.begin();
                 it_data != entries_data.end(); it_data++) {

                file.openData(it_data->first);
                attr_infos = file.getAttrInfos();

                for (vector<NeXus::AttrInfo>::iterator it_att = attr_infos.begin(); it_att != attr_infos.end(); it_att++) {


                  if (it_att->name=="signal"){
//                      std::cout << "found plottable data!!" << std::endl;

                      size_t img_size[3];

                      img_size[0] = file.getInfo().dims[2];
                      img_size[1] = file.getInfo().dims[1];
                      img_size[2] = file.getInfo().dims[0];


                      ImgType *slab=new ImgType[img_size[0]*img_size[1]*(end-start)]; // assuming images are all int_16.. but possibly this can be read from nexus info..

                      vector<int> slab_start;
                      slab_start.push_back(start);
                      slab_start.push_back(0);
                      slab_start.push_back(0);

                      vector<int> slab_size;
                      slab_size.push_back(end-start);
                      slab_size.push_back(img_size[1]);
                      slab_size.push_back(img_size[0]);


                      ImgType *pslice = img.GetDataPtr();

                      // casting to ImgType
                      for (size_t i=0; i<img.Size();++i){
                          slab[i] = static_cast<ImgType>(pslice[i]);
//                          pslice[i] = static_cast<ImgType>(slab[i]);
                      }


                      try{
                        file.putSlab(slab, slab_start, slab_size); // slab_start should be the starting index to insert the data
                      }
                      catch (const std::bad_alloc & E) {
                              msg.str("");
                              msg<<"WriteNeXusStack file.getSlab caused an STL exception: "<<E.what();
                              throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
                      }
                      catch (...){
                          msg.str("");
                          msg<<"WriteNeXusStack file.getSlab caused an unknown exception: ";
                          throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
                      }

                      }
                  }
                file.closeData();
                }

            file.closeGroup();
            }

    }

    file.close(); // close the file. hopefully


    return 1;
}


/// \brief Gets the dimensions of a Nexus image without reading the image
/// \param fname file name of the image file
/// \param dims array with the dimensions
int KIPLSHARED_EXPORT GetNexusDims(const char *fname, size_t *dims);

/// \brief Gets some infos of a Nexus image without reading the image
/// \param fname file name of the image file
/// \param dims array with the dimensions
int KIPLSHARED_EXPORT GetNexusInfo(const char *fname, size_t *NofImg, double *ScanAngles);

}} // end namespaces


#endif /*__IO_NEXUS_H*/
