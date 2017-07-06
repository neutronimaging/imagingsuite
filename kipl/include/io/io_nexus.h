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

template <class ImgType, size_t NDim> // maybe I can suppose that they are all 3D images? for starting
int ReadNexus(kipl::base::TImage<ImgType,NDim> &img, const char *fname) {



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
                      catch (const std::bad_alloc & E) {
                              msg.str("");
                              msg<<"ReadNexus file.getData caused an STL exception: "<<E.what();
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
