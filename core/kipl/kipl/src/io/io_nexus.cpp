#include <iostream>
#include "../../include/base/timage.h"
#include "../../include/base/KiplException.h"
#include "../../include/io/io_nexus.h"


namespace kipl { namespace io{

int KIPLSHARED_EXPORT GetNexusInfo(const char *fname, size_t *NofImg, double *ScanAngles){
//int GetNexusInfo(const char *fname, size_t *NofImg, double *ScanAngles){
    kipl::logging::Logger logger("GetNexusInfo");
    NeXus::File file(fname);
    logger.message(fname);

    vector<NeXus::AttrInfo> attr_infos = file.getAttrInfos();

    // look for default plottable data and gtet relevant information
    // check group attributes

    file.openGroup("entry", "NXentry");
    attr_infos = file.getAttrInfos(); // check how many or assuming that there is only one NXentry entry ?
    map<string, string> entries = file.getEntries();
    for (map<string,string>::const_iterator it = entries.begin();        it != entries.end(); ++it) {
        if(it->second=="NXdata") {

            file.openGroup(it->first, it->second);
            attr_infos = file.getAttrInfos();


            map<string, string> entries_data = file.getEntries();


            for (map<string,string>::const_iterator it_data = entries_data.begin();
                 it_data != entries_data.end(); it_data++) {

                    file.openData(it_data->first);
                    attr_infos = file.getAttrInfos();

                    for (vector<NeXus::AttrInfo>::iterator it_att = attr_infos.begin();
                         it_att != attr_infos.end(); it_att++) {

//                      cout << "   " << it_att->name << " = ";
//                      if (it_att->type == NeXus::CHAR) {
//                        cout << file.getStrAttr(*it_att);
//                      }
//                      cout << endl;

                      if (it_att->name=="signal"){

                          size_t img_size[3] = {static_cast<size_t>(file.getInfo().dims[2]), static_cast<size_t>(file.getInfo().dims[1]), static_cast<size_t>(file.getInfo().dims[0])};

                          NofImg[0]=0;
                          NofImg[1] = img_size[2]-1;

                          }
                      }

                    if (ScanAngles!=nullptr) {
                        if  (it_data->first=="rf"){
                            float *angles;
                            angles = new float[file.getInfo().dims[0]];
                            // want to get the angles
                            file.openData(it_data->first);
                                    file.getData(angles);
                            ScanAngles[0] = static_cast<double>(angles[0]);
                            ScanAngles[1] = static_cast<double>(angles[file.getInfo().dims[0]-1]);

        //                    break;
    //                        file.closeData();

                        }
                    }

                    file.closeData();
                }

            file.closeGroup();
            }

    }

    file.close();

    return 1;

}

std::vector<size_t> KIPLSHARED_EXPORT GetNexusDims(const std::string &fname)
{
    std::vector<size_t> dims;
    NeXus::File file(fname.c_str());

    vector<NeXus::AttrInfo> attr_infos = file.getAttrInfos();

    // look for default plottable data and gtet relevant information
    // check group attributes

    file.openGroup("entry", "NXentry");
    attr_infos = file.getAttrInfos(); // check how many or assuming that there is only one NXentry entry ?
    map<string, string> entries = file.getEntries();
    for (map<string,string>::const_iterator it = entries.begin();        it != entries.end(); ++it) {
        if(it->second=="NXdata") {

            file.openGroup(it->first, it->second);
            attr_infos = file.getAttrInfos();

            map<string, string> entries_data = file.getEntries();

            for (map<string,string>::const_iterator it_data = entries_data.begin();
                 it_data != entries_data.end(); it_data++)
            {

                file.openData(it_data->first);
                attr_infos = file.getAttrInfos();

                for (vector<NeXus::AttrInfo>::iterator it_att = attr_infos.begin();
                     it_att != attr_infos.end(); it_att++)
                {
                          if (it_att->name=="signal")
                          {
                              dims = std::vector<size_t>({  static_cast<size_t>(file.getInfo().dims[2]),
                                                            static_cast<size_t>(file.getInfo().dims[1])  });
                          }
                }
                file.closeData();
            }

            file.closeGroup();
        }

    }

    file.close();
    return dims;

}

}}
