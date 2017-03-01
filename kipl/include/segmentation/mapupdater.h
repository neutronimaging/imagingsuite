//<LICENCE>

#ifndef MAPUPDATER_H
#define MAPUPDATER_H

#include <map>
#include "multiresseg.h"
#include "../base/timage.h"
#include "../morphology/morphology.h"
#include "../math/mathconstants.h"

namespace akipl { namespace segmentation {
/// \brief Updates the pyramid levels by selecting teh class with the highest likelihood
/// @author Anders Kaestner
template <class ImgType, class SegType, size_t NDim>
class LikelihoodUpdater : public SegmentUpdateBase<ImgType,SegType,NDim>
{
public:
	/// \brief Initializes the MAPUpdater
	///
	/// \param NC Number of classes to find
	/// \param os stream for processing output
	LikelihoodUpdater(int NC=2,std::ostream &os=cout) :
	 SegmentUpdateBase<ImgType,SegType,NDim>(os)
	{
		this->nClasses=NC;
		this->name="SegmentUpdaterBase";
	}
	
	virtual ~LikelihoodUpdater() {}
	/// \brief Process function that computes the classes of the child level based on statistics from the parent
	///
	/// \param parent Graylevel image of the parent 
	/// \param parentSeg Segments of the parent
	/// \param child Graylevel image of the child
	/// \param childSeg output segments of the child level
	int update(const kipl::base::TImage<ImgType, NDim> &parent, 
				const kipl::base::TImage<SegType, NDim> & parentSeg, 
				const kipl::base::TImage<ImgType, NDim> &child, 
				kipl::base::TImage<SegType, NDim> &childSeg);
				
protected:

	
	/// \brief Support function the determines the most likely class membership
	/// \param val pixel value to classify
	/// \param apriori Apriori information of the parent class
	SegType GetClass(ImgType &val);
	

};

/// \brief Updates the pyramid levels by selecting the class with the highest likelihood. The likelihood is weighted by the apriori distribution of the parent voxel.
///
/// @author Anders Kaestner
template<class ImgType, class SegType, size_t NDim>
class MAPUpdaterApriori : public SegmentUpdateBase<ImgType,SegType,NDim>
{
public:
	
	MAPUpdaterApriori(int NC=2, std::ostream & os=std::cout) : SegmentUpdateBase<ImgType,SegType,NDim>(os)
	{
		this->nClasses=NC;
		this->name="MAPUpdaterApriori";
	}
	virtual ~MAPUpdaterApriori() {}
	/// \brief Process function that computes the classes of the child 
	///			level based on statistics from the parent
	///
	/// \param parent Graylevel image of the parent 
	/// \param parentSeg Segments of the parent
	/// \param child Graylevel image of the child
	/// \param childSeg output segments of the child level
	virtual int update(const kipl::base::TImage<ImgType, NDim> &parent, 
				const kipl::base::TImage<SegType, NDim> & parentSeg, 
				const kipl::base::TImage<ImgType, NDim> &child, 
				kipl::base::TImage<SegType, NDim> &childSeg);
protected:
	/// \brief Support function the determines the most likely class membership
	/// \param val pixel value to classify
	/// \param apriori Apriori information of the parent class
	SegType GetClass(const ImgType val,const SegType apriori);
};


/// \brief Updates the pyramid levels by selecting the class with the highest likelihood. The likelihood is weighted by the apriori distribution of the parent voxel.
/// 
/// @author Anders Kaestner
template<class ImgType, class SegType, size_t NDim>
class MAPUpdaterNeighborhood : public MAPUpdaterApriori<ImgType,SegType,NDim>
{
public:
	
	MAPUpdaterNeighborhood(int NC=2, std::ostream & os=std::cout) :
		 MAPUpdaterApriori<ImgType,SegType,NDim>(NC,os)
	{
		this->name="MAPUpdaterNeighborhood";
	}
	
	virtual ~MAPUpdaterNeighborhood() {}
	/// \brief Process function that computes the classes of the child 
	///			level based on statistics from the parent
	/// \param parent Graylevel image of the parent 
	/// \param parentSeg Segments of the parent
	/// \param child Graylevel image of the child
	/// \param childSeg output segments of the child level
	virtual int update(const kipl::base::TImage<ImgType, NDim> &parent, 
				const kipl::base::TImage<SegType, NDim> & parentSeg, 
				const kipl::base::TImage<ImgType, NDim> &child, 
				kipl::base::TImage<SegType, NDim> &childSeg);


protected:
	/// \brief Gets the class of a voxel, given local class probabilities
	/// \param val intensity value of the pixel to classify
	/// \param cntMap list of present classes and their probabilities
	virtual SegType GetClass(const ImgType val, std::map<SegType,unsigned short> &cntMap);

};

///	\brief Updates the pyramid levels by selecting the class with the highest likelihood. 
///
/// The likelihood is weighted by the class probability of the available voters. 
/// The number of voters can be set to avoid multiclass ambiguities.
/// The class probability is determined from the parent level using a 8- or 26-neighborhood
/// for 2D and 3D images respectively.
/// 
/// @author Anders Kaestner
template<class ImgType, class SegType, size_t NDim>
class MAPUpdaterNeighborhood2 : public SegmentUpdateBase<ImgType,SegType,NDim>
{
public:
	
	MAPUpdaterNeighborhood2(int NC=2, std::ostream & os=std::cout) :
		 SegmentUpdateBase<ImgType,SegType,NDim>(os)
	{
		this->nClasses=NC;
		this->nVotingClasses=2;
		this->name="MAPUpdaterNeighborhood2";
	}
	virtual ~MAPUpdaterNeighborhood2() {}
	/// \brief Process function that computes the classes of the child 
	/// 	   level based on statistics from the parent
	/// \param parent Graylevel image of the parent 
	/// \param parentSeg Segments of the parent
	/// \param child Graylevel image of the child
	/// \param childSeg output segments of the child level
	int update(const kipl::base::TImage<ImgType, NDim> &parent, 
				const kipl::base::TImage<SegType, NDim> & parentSeg, 
				const kipl::base::TImage<ImgType, NDim> &child, 
				kipl::base::TImage<SegType, NDim> &childSeg);
				
	/// \brief Set the number of voters to consider 
	/// \param n number of alowed voting classes
	int setVotingClasses(int n) {nVotingClasses=n; return 0;}
	
protected:
	/// \brief Gets the class of a voxel, given local class probabilities
	/// \param val intensity value of the pixel to classify
	/// \param cntMap list of present classes and their probabilities
	SegType GetClass(ImgType &val, std::map<SegType,unsigned short> &cntMap);
	/// Number of voting classes to consider
	int nVotingClasses;
};

/// \brief Updates the pyramid levels by selecting the class with the highest likelihood. The likelihood is weighted by the apriori distribution of the parent voxel.
///
/// @author Anders Kaestner
template<class ImgType, class SegType, size_t NDim>
class LaplacianUpdater : public SegmentUpdateBase<ImgType,SegType,NDim>
{
public:
	
	LaplacianUpdater(int NC=2, std::ostream & os=std::cout) : SegmentUpdateBase<ImgType,SegType,NDim>(os)
	{
		this->nClasses=NC;
		this->name="LaplacianUpdater";
	}
	virtual ~LaplacianUpdater() {}
	/// \brief Process function that computes the classes of the child 
	///			level based on statistics from the parent
	///
	/// \param parent Graylevel image of the parent 
	/// \param parentSeg Segments of the parent
	/// \param child Graylevel image of the child
	/// \param childSeg output segments of the child level
	virtual int update(const kipl::base::TImage<ImgType, NDim> &parent, 
				const kipl::base::TImage<SegType, NDim> & parentSeg, 
				const kipl::base::TImage<ImgType, NDim> &child, 
				kipl::base::TImage<SegType, NDim> &childSeg);
protected:
  SegType GetClass(std::map<SegType,unsigned short> &cntMap,ImgType laplacian);
  
  ImgType Laplacian(const kipl::base::TImage<ImgType, NDim> &img, 
		  													int X, 
		  													int Y, 
		  													int Z, 
		  													size_t const *const dims);
};
}}

#include "core/mapupdater.hpp"

#endif
