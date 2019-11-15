#include <iostream>
#include <fstream>
#include <string>
#include <base/timage.h>
#include <io/io_tiff.h>
#include <strings/filenames.h>
#include <base/trotate.h>

kipl::base::TImage<unsigned short,2> ReadDat(std::string fname, size_t x, size_t y, size_t idx);

int main(int argc, char *argv[])
{
    cout<<"Converting andor dat to tiff"<<endl;

    string fname=argv[1];
    int x=atoi(argv[2]);
    int y=atoi(argv[3]);
    int idx=atoi(argv[4]);
    int first=atoi(argv[5]);
    string outname=argv[6];
    kipl::base::TImage<unsigned short,2> img;

    img=ReadDat(fname,x,y,idx);


    string savename,ext;
    kipl::strings::filenames::MakeFileName(outname,first+idx,savename,ext,'#','0');
    kipl::base::TRotate<unsigned short> r;
    img=r.Rotate90(img);
    cout<<savename<<endl;
    kipl::io::WriteTIFF(img,savename.c_str());

    return 0;
}

union converter
{
  // for reading
  char c;

  // for convertion
  unsigned char b;
  struct
  {
    unsigned int hi: 4;
    unsigned int lo: 4;
  } nibbles;
};

kipl::base::TImage<unsigned short,2> ReadDat(std::string fname, size_t x, size_t y, size_t idx)
{
    size_t dims[2]={x+(x%16),y};
    kipl::base::TImage<unsigned short,2> img(dims);

    ifstream datfile(fname.c_str(), ios_base::binary);

    unsigned short *pImg=img.GetDataPtr();
    size_t offset=(img.Size()+img.Size(0))*1.5*idx;
    datfile.seekg(offset);

    size_t N=img.Size();
    converter data[3];

    for (size_t i=0; i<N; i+=2)
    {
        datfile.get(data[0].c);
        datfile.get(data[1].c);
        datfile.get(data[2].c);

        pImg[i]=data[1].nibbles.hi + (data[0].b<<4);
        pImg[i+1]=data[1].nibbles.lo + (data[2].b<<4);
    }

    dims[0]=x;

    kipl::base::TImage<unsigned short,2>  res(dims);

    for (size_t i=0; i<img.Size(1); i++) {
        memcpy(res.GetLinePtr(i),img.GetLinePtr(i),res.Size(0)*sizeof(unsigned short));
    }

    return res;
}


