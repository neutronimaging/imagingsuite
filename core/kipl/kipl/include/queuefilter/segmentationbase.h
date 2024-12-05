

#ifndef SEGMENTATIONSEGMENTATIONBASE_H
#define SEGMENTATIONSEGMENTATIONBASE_H

#include <image/image.h>
#include <string>
#include <vector>
#include <iostream>
#include <misc/parenc.h>
#include <visualization/imgviewer.h>
#include <io/io_matlab.h>

/** \brief Grouping classification and segmentation related classes
*/
namespace Segmentation {

/** \brief Provides an interface for classes that do classification tasks
@author Anders Kaestner
*/
template<class ImgType, class SegType, int NDim>
class SegmentationBase{
protected:
	/// Stream for processing output
	std::ostream &logstream;
public:
	/** \brief Constructor that links the logstream to its resource
		\param os Stream resource for processing output
	*/
    SegmentationBase(std::ostream & os=std::cout): logstream(os) {
    		quiet=true; 
    		name="SegmentationBase";
    		#ifdef USE_VIEWER
                fig=nullptr;
			#endif
    		}
    
    
	
	/// Empty constructor
    ~SegmentationBase() {}
    
    /** \brief Sets the number of classes to find
    \param NC Number of the classes to find
    */
    int setClasses(int NC) {nClasses=NC; return 0;}
    
    /// Returns the number of classes supported by the segmenter
    int getClasses() {return nClasses; }
    
    /** \brief Interface function that is intended to perform the segmentation
    	\param img Input graylevel image
    	\param seg Segmented output image
    */
    virtual int operator()(const Image::CImage<ImgType,NDim> & img, Image::CImage<SegType,NDim> &seg)=0;
    /// Tells the name of the current class
    const string & getName() {return name;}
    
    int saveProgressImages(const string &name); 
    
    virtual bool Quiet(bool q) {quiet=q; return 0;}
#ifdef USE_VIEWER
    /// Pointer to a CImgViewer instance for visualizing the progress of the segmentation
	Visualization::CImgViewer *fig;
#endif
protected:
	/// Number of classes to find
	int nClasses;
	/// Name of the class, is initialized by the constructor
	string name;
	
	/// Switch to control the meassage output
	bool quiet;
	
	std::string path;
	std::string fname;
	std::vector<std::string> ext;
	
	template <class T>
	int saveSlice(const CImage<T,NDim> & img, const std::string & subname, int num)
	{
		char tmpstr[256];
		sprintf(tmpstr,"%s%d",subname.c_str(),num);
		
		return saveSlice(img, tmpstr);
	}
	
	template <class T>
	int saveSlice(const CImage<T,NDim> & img, const std::string & subname)
	{
		if (!fname.empty()) {
			Image::CImage<T,2> slice;
			
			std::string vname=fname+subname;
			std::string outname=path+vname+".mat";
			
			img.ExtractSlice(slice,Image::plane_XY,img.SizeZ()/2);
			
			Fileio::WriteMAT(slice,outname.c_str(),vname.c_str());
		}
		return 0;
	}
	
};

template<class ImgType, class SegType, int NDim>
int SegmentationBase<ImgType,SegType,NDim>::saveProgressImages(const string &name) 
{
	if (!name.empty())
		return Parenc::StripFileName(name,path,fname,ext);
	else {
		path.clear(); 
		fname.clear(); 
		ext.clear();
	}
	
	return 0;
}

}

#endif
