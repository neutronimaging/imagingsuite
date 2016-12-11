//
// This file is part of the i KIPL image processing library by Anders Kaestner
// (c) 2008 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author$
// $Date$
// $Rev$
//

#ifndef SEGMENTATIONSEGMENTATIONBASE_H
#define SEGMENTATIONSEGMENTATIONBASE_H
#include <string>
#include <vector>
#include <iostream>
#include <map>

#include <base/timage.h>
#include <base/textractor.h>
#include <strings/parenc.h>
#include <io/io_matlab.h>
#include <logging/logger.h>

/// \brief Grouping classification and segmentation related classes
namespace kipl { namespace segmentation {

/// \brief Provides an interface for classes that do classification tasks
/// @author Anders Kaestner
template<class ImgType, class SegType, size_t NDim>
class SegmentationBase{
protected:
	/// Stream for processing output
	kipl::logging::Logger logger;
public:
	/// \brief Constructor that links the logstream to its resource
	///	\param os Stream resource for processing output
    SegmentationBase(std::string sName): logger(sName), name(sName), quiet(true) {}
	
	/// Empty constructor
    virtual ~SegmentationBase() {}
    
    /// \brief Sets the number of classes to find
    /// \param NC Number of the classes to find
    virtual int setClasses(int NC) {nClasses=NC; return 0;}
    
    /// Returns the number of classes supported by the segmenter
    int getClasses() {return nClasses; }
    
    /// \brief Interface function that is intended to perform the segmentation
    ///	\param img Input graylevel image
    ///	\param seg Segmented output image
    virtual int operator()(kipl::base::TImage<ImgType,NDim> & img, kipl::base::TImage<SegType,NDim> &seg)=0;
    /// Tells the name of the current class
    const string & getName() {return name;}

    /// Set the parameters for the segmentation
    virtual void setParameters(std::map<std::string,std::string> parameters) = 0;
    
    void saveProgressImages(const string name); 
    
    virtual bool Quiet(bool q) {quiet=q; return 0;}

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
	int saveSlice(const kipl::base::TImage<T,NDim> & img, const std::string subname, int num)
	{
		char tmpstr[256];
		sprintf(tmpstr,"%s%d",subname.c_str(),num);
		
		return saveSlice(img, tmpstr);
	}
	
	template <class T>
	int saveSlice(const kipl::base::TImage<T,NDim> & img, const std::string subname)
	{
		if (!fname.empty()) {
			kipl::base::TImage<T,2> slice;
			
			std::string vname=fname+subname;
			std::string outname=path+vname+".mat";
			
			slice=kipl::base::ExtractSlice(img,kipl::base::ImagePlaneXY,img.Size(2)>>1);
			
			kipl::io::WriteMAT(slice,outname.c_str(),vname.c_str());
		}
		return 0;
	}
	
	
};

template<class ImgType, class SegType, size_t NDim>
void SegmentationBase<ImgType,SegType,NDim>::saveProgressImages(const string name) 
{
	if (!name.empty())
		kipl::strings::filenames::StripFileName(name,path,fname,ext);
	else {
		path.clear(); 
		fname.clear(); 
		ext.clear();
	}
}

}}

#endif
