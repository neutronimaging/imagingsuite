//<LICENCE>

#ifndef LABEL_H
#define LABEL_H

#include <deque>
#include <list>
#include <vector>
#include <algorithm>
#include <iterator>

#include "morphology.h"
#include "../base/timage.h"
#include "pixeliterator.h"
#include "../base/kiplenums.h"

namespace kipl {namespace morphology {
	/// \brief Creates a labelled image from a bi level image
	/// \param img Input image
	/// \param lbl Image containing the labelled regions
    /// \param conn Connectivity selector
    /// \param bg Background value
    ///
	/// \retval The method returns the number of labelled regions
    /// 
	/// \note If the input image is a grayscale image will the regions be determined as 
	/// neighbours having the same graylevel.
	template <class ImgType, size_t NDim>
        size_t LabelImage(kipl::base::TImage<ImgType,NDim> & img, kipl::base::TImage<int,NDim> & lbl,kipl::base::eConnectivity conn=kipl::base::conn4, ImgType bg=static_cast<ImgType>(0))
	{
        list<ptrdiff_t> stack;
		
		ImgType *pImg=img.GetDataPtr();
        lbl.resize(img.dims());
		lbl=0;
		
		int *pLbl=lbl.GetDataPtr();
		

        kipl::base::PixelIterator NG(img.dims(),conn);

        int cnt=0;
        size_t N=img.Size();
        ptrdiff_t pp,q,qq;
        ImgType t=static_cast<ImgType>(0);
		
        for (size_t p=0; p<N ; ++p) 
		{
            NG.setPosition(p);
            if ( (!pLbl[p]) && (pImg[p]!=bg) ) 
			{
                t=pImg[p];
                ++cnt;
                pLbl[p]=cnt;
                for (const auto & neighborPix : NG.neighborhood())
                {
                    pp=p + neighborPix;

                     if (pImg[pp]==t)
                     {
                        stack.push_front(pp);
                        pLbl[pp]=cnt;
                     }
                }

                while (!stack.empty()) {
                    q=stack.front();
                    stack.pop_front();
                    NG.setPosition(q);
                    for (const auto & neighborPix: NG.neighborhood())
                    {
                        qq = q + neighborPix;
                        if ((qq < 0) || (img.Size()<=qq))
                        {
                            return 0;
                        }

                        if ((pImg[qq]==t) && (!pLbl[qq]))
                        {
                            stack.push_front(qq);
                            pLbl[qq]=cnt;
                        }
                    }

                }
            }
        }
        return static_cast<size_t>(cnt);
	}
	
	/// \brief Computes the area of each labelled object in a labelled image
	/// \param a A labelled image
	/// \param n Number of regions
	/// \param area Output-vector containing the areas of the regions (first=cnt, second=a pixel in the region)
	template <class ImgType,size_t NDim>
	int LabelArea(kipl::base::TImage<ImgType,NDim> & a,size_t n,vector<pair<size_t,size_t> > & area)
	{
		area.resize(n+1);
        for (size_t i=0; i<area.size(); i++)
        {
			area[i].first=0;
			area[i].second=0;
		}

		int lbl;
		const size_t N=a.Size();
        for (size_t i=0; i<N; i++)
        {
			lbl=a[i];
			area[lbl].first++;
			area[lbl].second=i;
		}
		
		return 1;
	}
	
	/// Removes items to a list of seeds from an image
	/// \param img The image to be modified (source and target)
	/// \param remove_list a list with seed pixels 
	/// \param conn Neighborhood connectivity
	/// \param replaceval value to inscribe on the connected pixels
	template <class ImgType,size_t NDim>
    int RemoveConnectedRegion(kipl::base::TImage<ImgType,NDim> &img, const vector<size_t> &remove_list, kipl::base::eConnectivity conn, ImgType replaceval=0)
	{
		if (remove_list.empty())
			return 0;
			
        kipl::base::TImage<ImgType,NDim> tmp;
        tmp.Clone(img);
		
        kipl::base::PixelIterator NG(img.dims(),conn);

        ptrdiff_t pos,p;
		
		ImgType *pImg=img.GetDataPtr();
		ImgType *pTmp=tmp.GetDataPtr();
		
		deque<long> remove_queue;
		
        copy(remove_list.begin(),remove_list.end(),back_inserter(remove_queue));


        for (const auto & pix : remove_queue) 
		{
            pTmp[pix]=replaceval;
		}
		
        while (!remove_queue.empty())
        {
			p=remove_queue.front();
			remove_queue.pop_front();
            NG.setPosition(p);
            for (const auto &neighborPix : NG.neighborhood())
            {
                pos = p + neighborPix;
                if ((pImg[pos]!=replaceval) && (pTmp[pos]!=replaceval))
                {
                    remove_queue.push_back(pos);
                    pTmp[pos]=replaceval;
                }
			}	
		}
		
		img=tmp;
		return 1;
	}
	
	/// \brief Extracts the regions with labels given by a list
	///	\param img Input image, a labelled image
	///	\param lbl Vector containing the labels to be extracted
	///	\param keeplabel If true will the labels be copied to the resulting image otherwise is the output a bilevel image
	kipl::base::TImage<int,2> ExtractLabelledRegions(kipl::base::TImage<int,2> &img, vector<int> &lbl, bool keeplabel=true);

	/// \brief Extracs one region from a labelled image
	///	\param img Input image, a labelled image
	///	\param n The label of the region to be extracted
	///
	///	\retval A bi-level image with the region
	kipl::base::TImage<int,2> ExtractLabelledRegions(kipl::base::TImage<int,2> &img, int n);

	/// \brief Container class for labelled items 
    class LabelledItem {
	public:
		
        LabelledItem(){ResetAll();}
        LabelledItem(int id, int nDim);
		/// Copy constructor
		/// \param item object to copy
        LabelledItem(const LabelledItem &item) {Copy(item);}
		
		/// Empty destructor
        virtual ~LabelledItem() {}
		
		/// \brief Add a new pixel to the item
		/// \param x x-coordinate of the pixel
		/// \param y y-coordinate of the pixel
		/// \param z z-coordinate of the pixel (must only be provided in 3D)
		int AddPosition(int x, int y, int z=0);
		
		/// \brief Add a new pixel to the item
		///\param pos array with the coordinates
		int AddPosition(int *pos);
		
		/// Computes the COG of the item
		/// \param x x-coordinate of the item
		/// \param y y-coordinate of the item
		/// \param z z-coordinate of the item (only relevant in 3D)
		int getCOG(double &x, double &y, double &z) const ;
		
		/// Computes the COG of the item
		/// \param cogtmp array containing the COG of the item
		int getCOG(double *cogtmp) const;
		
		/// Computes the volume of the item
		int getVolume() const {return N;}
		/// Returns the ID number of the item
		int getItemID() const {return item_id;}
		
		/// Returns the dimension rank of the item
		int getNDim() const {return NDim;}
		
		/// \brief Assignment operator
		/// \param item the item to assign
        const LabelledItem & operator=(const LabelledItem &item) 
		{
			Copy(item); 
			return *this;
		}
		
		const int * getBoundingBoxptr() const {return bbox;}
		/// Switch to select pretty printing
		bool PrettyPrint;
	private:
		int CheckBBox(int x,int y,int z);
		int CheckBBox(int *pos) {return CheckBBox(pos[0],pos[1],pos[2]);}
		
		int item_id;
		int NDim;
		double sum[3];
		double sum2[3];
		mutable double cog[3];
		int ResetAll();
        int Copy(const LabelledItem & item);
		int Compute() const;
		mutable bool newinfo;
		int N;
		int bbox[6];
	};
	
	/// \brief Identifies all segmented items and generates a list with item information
	///
	/// The collector considers negative values a background. The background has index 0, hence are 
	/// the indices to the item vector incremented by one compared to the actual item index. The item objects 
	/// does however have the right label.
	
	/// \param img A labeled image 
	/// \param itemList a vector where the item information is stored
	template<class ImgType, size_t NDim>
    int CollectLabelledItemsInfo(const kipl::base::TImage<ImgType,NDim> &img, vector<LabelledItem> &itemList)
	{
		int i,x,y,z;
		
		itemList.clear();
		size_t const * const dims=img.Dims();
		int N=img.Max()+1;
        LabelledItem tmp;
		cout<<N-2<<" items are present in the sample"<<endl;
		cout<<"Preparing item list"<<endl;
		for (i=0; i<N; i++) 
		{
            tmp=LabelledItem(i-1,NDim);
			itemList.push_back(tmp);
		}

		cout<<"Scanning image"<<endl;
		int dimx = dims[0];
		int dimy = dims[1];
		int dimz = dims[2];

		for (i=0, z=0; z<dimz; ++z)
			for (y=0; y<dimy; ++y)
				for (x=0; x<dimy; ++x, ++i) 
					if (img[i]>=0)
						itemList[img[i]+1].AddPosition(x,y,z);
					else
						itemList[0].AddPosition(x,y,z);
		
		return itemList.size();
	}
	
	/// Connects prints information from an item to an ostream
	/// \param os target ostream
	/// \param item The item to print
    ostream & operator<<(ostream & os, const LabelledItem & item);
	
}}// End namespace Morphology

#endif

