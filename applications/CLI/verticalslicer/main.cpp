//<LICENSE>

#include <iostream>
#include <sstream>
#include <map>
#include <vector>

#include <analyzefileext.h>
#include <base/timage.h>
#include <base/KiplException.h>
#include <base/kiplenums.h>
#include <logging/logger.h>
#include <strings/filenames.h>

#include <imagereader.h>
#include <imagewriter.h>
#include <readerexception.h>

kipl::logging::Logger logger("Vertical slicer");
int process(int argc, char *argv[]);

int showHelp();

int parseArguments(vector<std::string> qargs, std::map<std::string, std::string> &pars);

int main(int argc, char *argv[])
{
    process(argc,argv);
    return 0;
}

int process(int argc, char *argv[])
{
    std::ostringstream msg;
    std::vector<std::string> qargs;
    std::copy_n(argv,argc,std::back_inserter(qargs));

    std::map<std::string,std::string> args;
    parseArguments(qargs,args);

    int cnt=qargs.size();
    if (cnt==1) 
    {
        logger.message("No arguments");
        showHelp();
        return 0;
    }

    ImageReader reader;

    std::string srcfname=args["infile"];
    size_t first=std::stoi(args["first"]);
    size_t last=std::stoi(args["last"]);

    auto dims=reader.imageSize("",srcfname,first,1.0);

    size_t slicex=dims[1]/2;
    size_t slicey=dims[0]/2;

    kipl::base::TImage<float,2> img;

    std::vector<size_t> dimsXZ={dims[0],last-first+1};
    kipl::base::TImage<float,2> sliceXZ(dimsXZ);

    std::vector<size_t> dimsYZ={dims[1],last-first+1};
    kipl::base::TImage<float,2> sliceYZ(dimsYZ);

    for (size_t i=first; i<=last; ++i) 
    {
        img=reader.Read("",srcfname,i,kipl::base::ImageFlipNone,kipl::base::ImageRotateNone,1.0,{});
        std::copy_n(img.GetLinePtr(slicex),img.Size(0),sliceXZ.GetLinePtr(i-first));

        float *pLine = sliceYZ.GetLinePtr(i-first);
        float *pImg  = img.GetDataPtr()+slicey;
        for (size_t j=0; j<img.Size(1); ++j) 
        {
            pLine[j] = pImg[j*img.Size(0)];
        }
    }

    ImageWriter writer;

    std::string path;
    std::string name;
    std::vector<std::string> ext;
    kipl::strings::filenames::StripFileName(args["outfile"],path,name,ext);
    std::string nameXZ=path+name+"_XZ"+ext.back();
    writer.write(sliceXZ,nameXZ);
    std::string nameYZ=path+name+"_YZ"+ext.back();
    writer.write(sliceYZ,nameYZ);

    return 0;
}


int parseArguments(vector<string> qargs, std::map<std::string,std::string> &pars)
{
    auto item=qargs.begin();
    ++item;
    for ( ; item!= qargs.end() ; ++item) 
    {
        if ((*item)[0]!='-')
            throw kipl::base::KiplException("Invalid argument",__FILE__,__LINE__);
        if (*item=="-i") 
        {
            ++item;
            if (item==qargs.end())
                throw kipl::base::KiplException("Too few arguments",__FILE__,__LINE__);

            pars.insert(std::make_pair("infile",*item));
        }

        if (*item=="-o") 
        {
            ++item;
            if (item==qargs.end())
                throw kipl::base::KiplException("Too few arguments for -o",__FILE__,__LINE__);

            pars.insert(std::make_pair("outfile",*item));
        }

        if (*item=="-first") 
        {
            ++item;
            if (item==qargs.end())
                throw kipl::base::KiplException("Too few arguments",__FILE__,__LINE__);

            pars.insert(std::make_pair("first",*item));
        }

        if (*item=="-last") 
        {
            ++item;
            if (item==qargs.end())
                throw kipl::base::KiplException("Too few arguments",__FILE__,__LINE__);

            pars.insert(std::make_pair("last",*item));
        }

        if (*item=="-rot") 
        {
            ++item;
            if (item==qargs.end())
                throw kipl::base::KiplException("Too few arguments",__FILE__,__LINE__);
            std::string rotstring="ImageRotate"+(*item == "0" ? "None" : *item);
            pars.insert(std::make_pair(std::string("rotate"),rotstring));
        }
    }

    for (const auto &par : pars) 
    {
        std::cout<<par.first<<"="<<par.second<<std::endl;
    }

    return static_cast<int>(pars.size());
}

int showHelp()
{
    std::cout<<"Vertical slicer\n";
    std::cout<<"Usage: verticalslicer [args] \n";
    std::cout<<"(c) Anders Kaestner, 2018\n";
    std::cout<<"Arguments:\n";
    std::cout<<"-i <file_#####.ext> : The image file to split can be either a multi-frame fits or vivaseq\n";
    std::cout<<"-o <slicename.ext> : The file mask of the output images. If skiped the base name of the input is used with index. \n";
    std::cout<<"-first <value> : first frame default is first=0\n";
    std::cout<<"-last <value> : last frame, default is the last available frame\n";
    std::cout<<"-rot <value> : rotate the image in 90 deg steps\n";

    return 0;
}
