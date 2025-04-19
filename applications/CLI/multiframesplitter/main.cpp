//<LICENSE>
#include <iostream>
#include <sstream>
#include <map>

// #include <QCoreApplication>
// #include <QVector>
// #include <QString>
// #include <QDebug>

#include <analyzefileext.h>
#include <base/timage.h>
#include <base/KiplException.h>
#include <base/kiplenums.h>
#include <logging/logger.h>
#include <strings/filenames.h>

#include <imagereader.h>
#include <imagewriter.h>
#include <readerexception.h>

kipl::logging::Logger logger("MultiFrameSplitter");

int process(int argc, char *argv[]);

int showHelp();

int parseArguments(vector<std::string> qargs, std::map<string, string> &pars);

int main(int argc, char *argv[])
{
    process(argc,argv);
    return 0;
}

int process(int argc, char *argv[])
{
    std::ostringstream msg;
    vector<std::string> qargs;
    copy_n(argv,argc,std::back_inserter(qargs));

    std::map<std::string,std::string> args;
    try {
           parseArguments(qargs,args);
    } catch (kipl::base::KiplException &e) {
        logger.error(e.what());
        exit(0);
    }



    int cnt=static_cast<int>(qargs.size());
    if (cnt==1) {
        logger(logger.LogMessage,"No arguments");
        showHelp();
        return 0;
    }

    std::vector<size_t> dims;

    ImageReader reader;

    std::string srcfname=args["infile"];
    try {
        dims=reader.imageSize(srcfname,1.0f);
    } catch (ReaderException &e) {
        logger.error(e.what());
        exit(0);
    }
    catch (kipl::base::KiplException &e) {
        logger.error(e.what());
        exit(0);
    }

    if (dims.size()<3) {
        logger(logger.LogWarning,"This is a single frame image");
        return 0;
    }

    cnt=static_cast<int>(dims[2]);

    msg.str("");
    msg<<srcfname<<" has "<<cnt<<" slices";
    logger(logger.LogMessage,msg.str());

    std::string path;
    std::string fname;
    std::vector<std::string> exts;

    kipl::strings::filenames::StripFileName(srcfname,path,fname,exts);

    std::string fmask=args["outfile"];

    if (fmask.empty()) {
        fmask=path+fname+"_#####.tif";
    }

    int first = 0 ;
    int last  = cnt-1;
    if (args["first"].empty()==false)
        first = std::stoi(args["first"]);

    if (args["last"].empty()==false)
        last = std::stoi(args["last"]);

    kipl::base::eImageRotate rotate=kipl::base::ImageRotateNone;

    try {
    if (args["rotate"].empty()==false)
        string2enum(args["rotate"],rotate);
    }
    catch (kipl::base::KiplException &e) {
        logger.error(e.what());
        return -1;
    }

    // qDebug() << QString::fromStdString(fmask) << ", first="<<first<<", last="<<last<<QString::fromStdString(enum2string(rotate));

    std::string destname,ext;
    ImageWriter writer;
    kipl::base::TImage<float,2> img;
    try {
        for (int i=first; i<=last; ++i) {
            img=reader.Read(srcfname,kipl::base::ImageFlipNone,rotate,1.0,{},i);
            kipl::strings::filenames::MakeFileName(fmask,i,destname,ext,'#','0');

            writer.write(img,destname);
        }
    }
    catch (ReaderException &e) {
        logger.error(e.what());
        return -1;
    }
    catch (kipl::base::KiplException &e) {
        logger.error(e.what());
        return -1;
    }
    return cnt;
}

int parseArguments(vector<string> qargs, std::map<std::string,std::string> &pars)
{
    std::ostringstream msg;
    auto item=qargs.begin();
    ++item;
    for ( ; item!= qargs.end() ; ++item) {
        if ((*item)[0]!='-')
        {
            msg.str(""); msg<<"Invalid argument "<<(*item);
            throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
        }
        if (*item=="-i") {
            ++item;
            if (item==qargs.end())
                throw kipl::base::KiplException("Too few arguments: infile is missing",__FILE__,__LINE__);

            pars.insert(std::make_pair("infile",*item));
        }

        if (*item=="-o") {
            ++item;
            if (item==qargs.end())
                throw kipl::base::KiplException("Too few arguments: Destination mask is missing",__FILE__,__LINE__);

            pars.insert(std::make_pair("outfile",*item));
        }

        if (*item=="-first") {
            ++item;
            if (item==qargs.end())
                throw kipl::base::KiplException("Too few arguments: first slice index",__FILE__,__LINE__);

            pars.insert(std::make_pair("first",*item));
        }

        if (*item=="-last") {
            ++item;
            if (item==qargs.end())
                throw kipl::base::KiplException("Too few arguments: last slice index",__FILE__,__LINE__);

            pars.insert(std::make_pair("last",*item));
        }

        if (*item=="-rot") {
            ++item;
            if (item==qargs.end())
                throw kipl::base::KiplException("Too few arguments: rotation is missing",__FILE__,__LINE__);
            std::string rotstring="ImageRotate"+(*item == "0" ? "None" : *item);
            pars.insert(std::make_pair(std::string("rotate"),rotstring));
        }
    }

    for (auto par : pars) {
        std::cout<<par.first<<"="<<par.second<<std::endl;
    }

    return static_cast<int>(pars.size());
}

int showHelp()
{
    std::cout<<"Multi-frame image splitter\n";
    std::cout<<"Usage: multiframesplitter [args] \n";
    std::cout<<"(c) Anders Kaestner, 2025\n";
    std::cout<<"Arguments:\n";
    std::cout<<"-i <file name> : The image file to split can be either a multi-frame fits or vivaseq\n";
    std::cout<<"-o <file_#####.tif> : The file mask of the output images. If skiped the base name of the input is used with index. \n";
    std::cout<<"-first <value> : first frame default is first=0\n";
    std::cout<<"-last <value> : last frame, default is the last available frame\n";
    std::cout<<"-rot <0/90/180/270> : clockwise rotate the image, default=0\n\n";

    return 0;
}
