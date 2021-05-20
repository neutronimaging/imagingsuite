//<LICENSE>

#include <QMessageBox>
#include <QString>
#include <sstream>

#include <logging/logger.h>
#include <strings/filenames.h>
#include <base/timage.h>
#include <io/io_tiff.h>
#include <strings/miscstring.h>
#include <strings/string2array.h>
#include <libxml/xmlreader.h>
#include "mergevolume.h"


#ifdef __GNUC__
    const std::string copystring = "cp";
#else
    const std::string copystring = "copy";
#endif


MergeVolume::MergeVolume() :
    logger("MergeVolume"),
    m_sPathA("/data/sliceA_####.tif"),
    m_sPathB("/data/sliceB_####.tif"),
    m_sPathOut("/data"),
    m_sMaskOut("NewVolume_####.tif"),
    m_nFirstA(0),
    m_nLastA(100),
    m_nStartOverlapA(90),
    m_nOverlapLength(10),
    m_nFirstB(0),
    m_nLastB(100),
    m_nFirstDest(0),
    m_nMergeOrder(0),
    m_bCropSlices(false)
{
    kipl::strings::filenames::CheckPathSlashes(m_sPathA,false);
    kipl::strings::filenames::CheckPathSlashes(m_sPathB,false);
    kipl::strings::filenames::CheckPathSlashes(m_sPathOut,false);

    m_nCropOffset[0]=0;
    m_nCropOffset[1]=0;
    m_nCrop[0]=0;
    m_nCrop[1]=0;
    m_nCrop[2]=100;
    m_nCrop[3]=100;
}

MergeVolume::~MergeVolume()
{

}

void MergeVolume::Process()
{
    if (m_bCropSlices)
        CropMerge();
    else
        CopyMerge();
}

void MergeVolume::CopyMerge()
{
    logger(kipl::logging::Logger::LogMessage,"Starting to merge volumes");

    std::string src_fname;
    std::string dst_fname;
    std::string ext;

    std::string src_maskA = m_sPathA;
    std::string src_maskB = m_sPathB;
    std::string dst_mask = m_sPathOut;
    kipl::strings::filenames::CheckPathSlashes(dst_mask,true);
    dst_mask+=m_sMaskOut;

    std::ostringstream cmd,msg;
    int cnt=m_nFirstDest;
    int i=0;
    int j=0;
    int k=0;

    double total_images=(m_nStartOverlapA-m_nFirstA)+(m_nLastB-m_nFirstB)+1;

    kipl::strings::filenames::MakeFileName(src_maskA,m_nFirstA,src_fname,ext,'#','0');
    size_t sizeA[4];
    kipl::io::GetTIFFDims(src_fname.c_str(),sizeA);
    kipl::strings::filenames::MakeFileName(src_maskB,m_nFirstB,src_fname,ext,'#','0');
    size_t sizeB[4];
    kipl::io::GetTIFFDims(src_fname.c_str(),sizeB);
    if ((sizeB[0]!=sizeA[0]) || (sizeB[1]!=sizeA[1]))
    {
        msg.str("");
        msg<<"Image A ("<<sizeA[0]<<", "<<sizeA[1]<<") is not same size as Image B ("<<sizeB[0]<<", "<<sizeB[1]<<")";
        throw kipl::base::KiplException(msg.str());
    }

    // Copy data A
    int res=0;
    //progress->set_text("Copying data A");
    for (i=m_nFirstA; i<=m_nStartOverlapA; i++, cnt++) {
        kipl::strings::filenames::MakeFileName(src_maskA,i,src_fname,ext,'#','0');
        kipl::strings::filenames::MakeFileName(dst_mask,cnt,dst_fname,ext,'#','0');
        cmd.str("");
        cmd<<copystring<<" \""<<src_fname<<"\" \""<<dst_fname<<"\"";
        logger(logger.LogMessage,cmd.str());
        res=system(cmd.str().c_str());
        if (res!=0) {
            msg.str("");
            msg<<"Could not copy "<<src_fname<<" to "<<dst_fname;
            logger(logger.LogError,msg.str());
            return ;
        }
    }

 //   progress->set_text("Mixing data sets");
    logger(logger.LogMessage,"Mixing data sets");
    // Here will the interpolation happen
    kipl::base::TImage<float,2> a,b;
    float *pA, *pB;
    float scale=1.0f/m_nOverlapLength;
    unsigned short BitPerSample=0;

    try {
        for (k=0, j=m_nFirstB; k<=m_nOverlapLength; i++,j++,k++, cnt++) {
            kipl::strings::filenames::MakeFileName(src_maskA,i,src_fname,ext,'#','0');
            kipl::io::ReadTIFF(a,src_fname.c_str());


            if (k==0)
                BitPerSample= a.info.nBitsPerSample;
            kipl::strings::filenames::MakeFileName(src_maskB,j,src_fname,ext,'#','0');
            kipl::io::ReadTIFF(b,src_fname.c_str());
            pA=a.GetDataPtr();
            pB=b.GetDataPtr();

            for (size_t m=0; m<a.Size(); m++) {
                pA[m]=(1.0f-k*scale)*pA[m]+k*scale*pB[m];
            }

            kipl::strings::filenames::MakeFileName(dst_mask,cnt,dst_fname,ext,'#','0');
            if (BitPerSample==16)
                kipl::io::WriteTIFF(a,dst_fname.c_str(),0.0f, 65535.0f);
            if (BitPerSample==32)
                kipl::io::WriteTIFF(a, dst_fname.c_str(), kipl::base::Float32);
        }
    }
    catch (kipl::base::KiplException &e) {
        msg.str("");
        msg<<"Failed to mix the data sets :"<<e.what();
        logger(logger.LogError,msg.str());

        return ;
    }
    catch (std::exception &e) {
        msg.str("");
        msg<<"Failed to mix the data sets :"<<e.what();
        logger(logger.LogError,msg.str());

        return ;
    }
    catch (...) {
        msg.str("");
        msg<<"Failed to mix the data sets unknown exception";
        logger(logger.LogError,msg.str());

        return ;
    }

    // Copy data B
    logger(logger.LogMessage,"Copying data B");
    for ( ; j<=m_nLastB; j++, cnt++) {
        kipl::strings::filenames::MakeFileName(src_maskB,j,src_fname,ext,'#','0');
        kipl::strings::filenames::MakeFileName(dst_mask,cnt,dst_fname,ext,'#','0');
        cmd.str("");
        cmd<<copystring<<" \""<<src_fname<<"\" \""<<dst_fname<<"\"";
        logger(logger.LogMessage,cmd.str());
        res=system(cmd.str().c_str());
        if (res!=0) {
            msg.str("");
            msg<<"Could not copy "<<src_fname<<" to "<<dst_fname;
            logger(logger.LogError,msg.str());

            return ;
        }
    }
}

void MergeVolume::CropMerge() {
    logger(kipl::logging::Logger::LogMessage,"Starting to merge cropped slices");

    std::string src_fname;
    std::string dst_fname;
    std::string ext;

    std::string src_maskA = m_sPathA;
    std::string dst_mask = m_sPathOut;
    kipl::strings::filenames::CheckPathSlashes(dst_mask,true);
    dst_mask+=m_sMaskOut;

    std::ostringstream cmd,msg;
    size_t cnt=m_nFirstDest;
    size_t i,j,k;
    double total_images=(m_nStartOverlapA-m_nFirstA)+(m_nLastB-m_nFirstB);
    // Copy data A
    int res=0;

    kipl::base::TImage<float,2> a,b;

//    size_t cropA[4]={static_cast<size_t>(m_nCrop[0]),
//            static_cast<size_t>(m_nCrop[1]),
//            static_cast<size_t>(m_nCrop[2]),
//            static_cast<size_t>(m_nCrop[3])};
    std::vector<size_t> cropA = {static_cast<size_t>(m_nCrop[0]),
                                 static_cast<size_t>(m_nCrop[1]),
                                 static_cast<size_t>(m_nCrop[2]),
                                 static_cast<size_t>(m_nCrop[3])};

//    size_t cropB[4]={static_cast<size_t>(m_nCrop[0]+m_nCropOffset[0]),
//            static_cast<size_t>(m_nCrop[1]+m_nCropOffset[1]),
//            static_cast<size_t>(m_nCrop[2]+m_nCropOffset[0]),
//            static_cast<size_t>(m_nCrop[3]+m_nCropOffset[1])};

    std::vector<size_t> cropB={static_cast<size_t>(m_nCrop[0]+m_nCropOffset[0]),
            static_cast<size_t>(m_nCrop[1]+m_nCropOffset[1]),
            static_cast<size_t>(m_nCrop[2]+m_nCropOffset[0]),
            static_cast<size_t>(m_nCrop[3]+m_nCropOffset[1])};

    logger(logger.LogMessage,"Copying data A");
    int bps=0;
    for (i=m_nFirstA; i<m_nStartOverlapA; i++, cnt++) {
        kipl::strings::filenames::MakeFileName(src_maskA,i,src_fname,ext,'#','0');
        kipl::strings::filenames::MakeFileName(dst_mask,cnt,dst_fname,ext,'#','0');

        bps=kipl::io::ReadTIFF(a,src_fname.c_str(),cropA, i);
        switch (bps) {
        case 8:
        case 16: kipl::io::WriteTIFF(a,dst_fname.c_str(),0.0f,65535.0f); break;
        case 32: kipl::io::WriteTIFF(a,dst_fname.c_str(), kipl::base::Float32); break;
        default: throw kipl::base::KiplException("Unhandled number of bits",__FILE__,__LINE__);
        }
    }

    logger(logger.LogMessage,"Mixing data sets");
    std::string src_maskB = m_sPathB;
    // Here will the interpolation happen

    float *pA, *pB;
    float scale=1.0f/m_nOverlapLength;
    logger(logger.LogMessage,"Copying data B");
    try {
        for (k=0, j=m_nFirstB; k<m_nOverlapLength; i++,j++,k++, cnt++) {
            kipl::strings::filenames::MakeFileName(src_maskA,i,src_fname,ext,'#','0');
            kipl::io::ReadTIFF(a,src_fname.c_str(), cropA, i);
            kipl::strings::filenames::MakeFileName(src_maskB,j,src_fname,ext,'#','0');
            kipl::io::ReadTIFF(b,src_fname.c_str(),cropB, i);
            pA=a.GetDataPtr();
            pB=b.GetDataPtr();

            for (size_t m=0; m<a.Size(); m++) {
                pA[m]=(1.0f-k*scale)*pA[m]+k*scale*pB[m];
            }

            kipl::strings::filenames::MakeFileName(dst_mask,cnt,dst_fname,ext,'#','0');
            switch (bps) {
            case 8:
            case 16: kipl::io::WriteTIFF(a,dst_fname.c_str(),0.0f,65535.0f); break;
            case 32: kipl::io::WriteTIFF(a,dst_fname.c_str(), kipl::base::Float32); break;
            default: throw kipl::base::KiplException("Unhandled number of bits",__FILE__,__LINE__);
            }
        }
    }

    catch (kipl::base::KiplException &e) {
        msg.str("");
        msg<<"Failed to mix the data sets :"<<e.what();
        logger(logger.LogError,msg.str());

        return ;
    }
    catch (std::exception &e) {
        msg.str("");
        msg<<"Failed to mix the data sets :"<<e.what();
        logger(logger.LogError,msg.str());

        return ;
    }
    catch (...) {
        msg.str("");
        msg<<"Failed to mix the data sets unknown exception";

        logger(logger.LogError,msg.str());

        return ;
    }

    // Copy data B
    for ( ; j<=m_nLastB; j++, cnt++) {
        kipl::strings::filenames::MakeFileName(src_maskB,j,src_fname,ext,'#','0');
        kipl::strings::filenames::MakeFileName(dst_mask,cnt,dst_fname,ext,'#','0');
        bps=kipl::io::ReadTIFF(a,src_fname.c_str(),cropB, i);
        switch (bps) {
        case 8:
        case 16: kipl::io::WriteTIFF(a,dst_fname.c_str(),0.0f,65535.0f); break;
        case 32: kipl::io::WriteTIFF(a,dst_fname.c_str(), kipl::base::Float32); break;
        default: throw kipl::base::KiplException("Unhandled number of bits",__FILE__,__LINE__);
        }
    }
}

void MergeVolume::LoadVerticalSlice(std::string filemask,
                                    int first,
                                    int last,
                                    kipl::base::TImage<float,2> *img)
{
    std::string fname,ext;

    kipl::base::TImage<float,2> slice;
    kipl::strings::filenames::MakeFileName(filemask,first, fname, ext, '#', '0');
    if (ext!=".tif") {
        throw kipl::base::KiplException("LoadVerticalSlice can only handle tiff images",__FILE__,__LINE__);
    }


    kipl::io::ReadTIFF(slice,fname.c_str());
    size_t line=slice.Size(1)/2;
//    size_t dims[2]={slice.Size(0),static_cast<size_t>(last-first+1)};
    std::vector<size_t> dims = {slice.Size(0),static_cast<size_t>(last-first+1)};
    size_t total_offset=0;

    if (m_bCropSlices) {
        dims[0]=(m_nCrop[2]-m_nCrop[0]+1);
        total_offset=m_nCrop[0];
    }

    img->resize(dims);

    // here I check the image type
    unsigned short BitPerSample = slice.info.nBitsPerSample;

    float *pLine;


    if (BitPerSample==32)
    {
        float *data = new float[slice.Size(0)];


        for (int i=first; i<=last; i++) {
            kipl::strings::filenames::MakeFileName(filemask,i, fname, ext, '#', '0');
            logger(kipl::logging::Logger::LogVerbose,fname);
            kipl::io::ReadTIFFLine(data,line,fname.c_str());
            pLine=img->GetLinePtr(i-first);
            for (size_t j=0; j<=img->Size(0); j++) {
                pLine[j]=static_cast<float>(data[j+total_offset]);
            }
     //       progress->set_fraction(static_cast<double>(i-first)/total_images);

        }

        delete [] data;
    }

    if (BitPerSample==16)
    {
        unsigned short *data = new unsigned short[slice.Size(0)];


        for (int i=first; i<=last; i++) {
            kipl::strings::filenames::MakeFileName(filemask,i, fname, ext, '#', '0');
            logger(kipl::logging::Logger::LogVerbose,fname);
            kipl::io::ReadTIFFLine(data,line,fname.c_str()); // THIS IS THE ERROR.. GO ON FROM THERE. READTIFFLINE
            pLine=img->GetLinePtr(i-first);
            for (size_t j=0; j<=img->Size(0); j++) {
                pLine[j]=static_cast<float>(data[j+total_offset]);
            }
     //       progress->set_fraction(static_cast<double>(i-first)/total_images);

        }

        delete [] data;
    }



}

std::string MergeVolume::WriteXML(size_t indent)
{
    std::ostringstream xml;

    xml<<std::setw(indent)<<" "<<"<merge>\n";
        xml<<std::setw(indent+4)<<" "<<"<path_a>"<<m_sPathA<<"</path_a>\n";
        xml<<std::setw(indent+4)<<" "<<"<path_b>"<<m_sPathB<<"</path_b>\n";
        xml<<std::setw(indent+4)<<" "<<"<path_dest>"<<m_sPathOut<<"</path_dest>\n";

        xml<<std::setw(indent+4)<<" "<<"<first_a>"<<m_nFirstA<<"</first_a>\n";
        xml<<std::setw(indent+4)<<" "<<"<last_a>"<<m_nLastA<<"</last_a>\n";
        xml<<std::setw(indent+4)<<" "<<"<startoverlap_a>"<<m_nStartOverlapA<<"</startoverlap_a>\n";
        xml<<std::setw(indent+4)<<" "<<"<overlaplength>"<<m_nOverlapLength<<"</overlaplength>\n";

        xml<<std::setw(indent+4)<<" "<<"<first_b>"<<m_nFirstB<<"</first_b>\n";
        xml<<std::setw(indent+4)<<" "<<"<last_b>"<<m_nLastB<<"</last_b>\n";
        xml<<std::setw(indent+4)<<" "<<"<first_dest>"<<m_nFirstDest<<"</first_dest>\n";

        xml<<std::setw(indent+4)<<" "<<"<cropslices>"<<kipl::strings::bool2string(m_bCropSlices)<<"</cropslices>\n";
        xml<<std::setw(indent+4)<<" "<<"<crop>"<<m_nCrop[0]<<" "<<m_nCrop[1]<<" "<<m_nCrop[2]<<" "<<m_nCrop[3]<<"</crop>\n";
        xml<<std::setw(indent+4)<<" "<<"<cropboffset>"<<m_nCropOffset[0]<<" "<<m_nCropOffset[1]<<"</cropboffset>\n";

    xml<<std::setw(indent)<<" "<<"</merge>\n";
    return xml.str();
}

void MergeVolume::ParseXML(string fname)
{
    xmlTextReaderPtr reader;
    const xmlChar *name;
    const xmlChar *value;
    int ret;
    std::string sName;
    std::string sValue;
    std::ostringstream msg;

    reader = xmlReaderForFile(fname.c_str(), nullptr, 0);
    if (reader != nullptr) {
        ret = xmlTextReaderRead(reader);
        name = xmlTextReaderConstName(reader);

        if (name==nullptr) {
            throw kipl::base::KiplException("Unexpected contents in parameter file",__FILE__,__LINE__);
        }

        sName=reinterpret_cast<const char *>(name);
        msg.str(""); msg<<"Found "<<sName<<" expect merge";
        logger(kipl::logging::Logger::LogMessage,msg.str());
        if (sName!="merge") {
            msg.str();
            msg<<"Unexpected project contents in parameter file ("<<sName<<"!=merge)";
            logger(kipl::logging::Logger::LogMessage,msg.str());
            throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
        }

        logger.verbose("Got project");

        ret = xmlTextReaderRead(reader);

        while (ret == 1) {
            if (xmlTextReaderNodeType(reader)==1) {
                name = xmlTextReaderConstName(reader);
                ret=xmlTextReaderRead(reader);
                value = xmlTextReaderConstValue(reader);
                if (name==nullptr) {
                    throw kipl::base::KiplException("Unexpected contents in parameter file",__FILE__,__LINE__);
                }
                sName=reinterpret_cast<const char *>(name);

                if (value!=nullptr)
                    sValue=reinterpret_cast<const char *>(value);
                else {
                    msg.str("");
                    msg<<"Parameter "<<sValue<<" does not have a value";
                    logger.warning(msg.str());
                    sValue="Empty";
                }

                if (sName=="path_a")
                    m_sPathA   = sValue;

                if (sName=="path_b")
                    m_sPathB   = sValue;

                if (sName=="path_dest")
                    m_sPathOut = sValue;

                if (sName=="first_a")
                    m_nFirstA  = std::stoi(sValue);

                if (sName=="last_a")
                    m_nLastA   = std::stoi(sValue);

                if (sName=="first_b")
                    m_nFirstB  = std::stoi(sValue);

                if (sName=="last_b")
                    m_nLastB   = std::stoi(sValue);

                if (sName=="startoverlap_a")
                    m_nStartOverlapA = std::stoi(sValue);

                if (sName=="overlaplength")
                    m_nOverlapLength = std::stoi(sValue);

                if (sName=="first_dest")
                    m_nFirstDest = std::stoi(sValue);

                if (sName=="cropslices")
                    m_bCropSlices = kipl::strings::string2bool(sValue);

                if (sName=="crop")
                    kipl::strings::String2Array(sValue,m_nCrop,4);

                if (sName=="cropboffset")
                    kipl::strings::String2Array(sValue,m_nCropOffset,2);

            }
            ret = xmlTextReaderRead(reader);
        }
        xmlFreeTextReader(reader);
        if (ret != 0) {
            std::stringstream str;
            str<<"MergeVolume failed to parse "<<fname;
            throw kipl::base::KiplException(str.str(),__FILE__,__LINE__);
        }
    } else {
        std::stringstream str;
        str<<"Module config could not open "<<fname;
        throw kipl::base::KiplException(str.str(),__FILE__,__LINE__);
    }
    logger.verbose("Parsing parameter file done");

}
