#include <iostream>
#include <fstream>
#include <string>
#include <list>

#include <QtWidgets/QApplication>
#include <QDesktopServices>

#include <base/timage.h>
#include <io/io_tiff.h>
#include <strings/filenames.h>
#include <base/trotate.h>

#include <io/io_generic.h>
#include <strings/filenames.h>
#include <base/trotate.h>

#include "convertconfig.h"
#include "mainwindow.h"

void Process(ConvertConfig & config);
int RunGUI(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    cout<<"Converting andor dat to tiff"<<endl;

    ConvertConfig config;
    std::list<std::string> args;
    if (1<argc) {
        std::copy(argv+1,argv+argc,std::back_inserter(args));
        config.ParseCLI(args);
        cout<<config<<std::endl;

        Process(config);
    }
    else {
        std::cout<<"Starting GUI..."<<std::endl;
        RunGUI(argc,argv);
    }

    return 0;
}

void Process(ConvertConfig & config)
{

    std::list<kipl::base::TImage<unsigned short,2> > imgs;
    kipl::base::TImage<unsigned short,2> img;
    kipl::base::TRotate<unsigned short> rot;

    if (config.src_mask.find('#')==std::string::npos) // single file
    {
        std::cout<<"Reading "<<config.src_mask<<std::endl;
        kipl::io::ReadGeneric(  imgs,
                                config.src_mask,
                                config.size[0],config.size[1],
                                config.offset,config.stride,
                                config.imagesperfile,
                                config.dt,config.endian,
                                {});

       std::list<kipl::base::TImage<unsigned short,2> >::iterator it;
       int cnt=0;

       std::string fname,ext;
       for (it=imgs.begin(); it!=imgs.end(); it++, cnt++) {
            kipl::strings::filenames::MakeFileName(config.dest_mask,cnt++,fname,ext,'#');
            std::cout<<"Writing "<<fname<<" from "<<config.src_mask<<std::endl;

            if (config.rotate)
                img=rot.Rotate90(*it);
            else
                img=*it;

            kipl::io::WriteTIFF(img,fname.c_str());
       }
    }
    else {
        std::string inname, outname,ext;
        int cnt=0;
        for (int i=config.first; i<=config.last; i++) {
            kipl::strings::filenames::MakeFileName(config.src_mask,i,inname,ext,'#','0',config.reverseindex);

            std::cout<<"Reading "<<inname<<std::endl;
            kipl::io::ReadGeneric(  imgs,
                                    inname,
                                    config.size[0],config.size[1],
                                    config.offset,config.stride,
                                    config.imagesperfile,
                                    config.dt,config.endian,
                                    {});

           std::list<kipl::base::TImage<unsigned short,2> >::iterator it;

           std::string fname,ext;
           for (it=imgs.begin(); it!=imgs.end(); it++, cnt++) {
                kipl::strings::filenames::MakeFileName(config.dest_mask,cnt,outname,ext,'#');
                if (config.rotate) {
                    img=rot.Rotate90(*it);
                    std::cout<<"rot"<<std::endl;
                }
                else
                    img=*it;
                std::cout<<"Writing "<<outname<<" from "<<inname<<std::endl;
                kipl::io::WriteTIFF(img,outname.c_str());
           }
        }
    }
}


int RunGUI(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow w;
    w.show();

    return app.exec();
}
