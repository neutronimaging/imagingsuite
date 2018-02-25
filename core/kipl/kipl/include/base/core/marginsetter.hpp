#ifndef MARGINSETTER_HPP
#define MARGINSETTER_HPP

namespace kipl {
namespace base {

template <typename T, size_t N>
void setMarginValue(kipl::base::TImage<T,N> &img, size_t width, T value)
{
    T *pLine=nullptr;
    size_t sx=img.Size(0);
    size_t sy=0;
    size_t sz=0;

    switch (N) {
    case 1:
        pLine=img.GetLinePtr(0);
        std::fill(pLine,pLine+width,value);
        std::fill(pLine+sx-width-1,pLine+sx,value);
        break;

    case 2:
        // top and bottom
        sy=img.Size(1);


        pLine=img.GetLinePtr(0);
        std::fill(pLine,pLine+sx*width,value);
        pLine=img.GetLinePtr(sy-width-1);
        std::fill(pLine,img.GetDataPtr()+img.Size(),value);
        // left and right
        for (size_t i=width; i<sy-width-1; ++i) {
            pLine=img.GetLinePtr(i);
            std::fill(pLine,pLine+width,value);
            std::fill(pLine+sx-width-1,pLine+sx,value);
        }
        break;

    case 3:
        sy=img.Size(1);
        sz=img.Size(2);

        pLine=img.GetLinePtr(0,0);
        std::fill(pLine,pLine+sx*sy*width,value);
        pLine=img.GetLinePtr(0,sz-width-1);
        std::fill(pLine,img.GetDataPtr()+img.Size(),value);

        for (size_t j=width; j<sz-width-1; ++j) {
            // front and back
            pLine=img.GetLinePtr(0,j);
            std::fill(pLine,pLine+sx*width,value);
            pLine=img.GetLinePtr(sy-width-1,j);
            std::fill(pLine,img.GetLinePtr(0,j+1),value);
            // left and right
            for (size_t i=width; i<sy-width-1; ++i) {
                pLine=img.GetLinePtr(i,j);
                std::fill(pLine,pLine+width,value);
                std::fill(pLine+sx-width-1,pLine+sx,value);
            }
        }
        break;
    default:
        break;
    }
}

}
}
#endif // MARGINSETTER_HPP
