//<LICENCE>

#ifndef KIPL_PLOTDATA_H
#define KIPL_PLOTDATA_H

#include <string>
#include <algorithm>

namespace kipl { namespace containers {

/// \brief A container for two data arrays that are intended for x and y data in plots
template <typename Tx, typename Ty>
class PlotData {
public:
    /// \brief Basic C'tor to create an empty plot data container
    PlotData() : m_tAxisX(nullptr), m_tAxisY(nullptr), m_nNdata(0), plotName("label") {}

    /// \brief Copy C'tor
    /// \param pd A plot data instance to copy
    PlotData(const PlotData<Tx,Ty> & pd) ;

    /// \brief The D'tor clean up the memory
	~PlotData() {
		if (m_tAxisX) 
			delete [] m_tAxisX;
		if (m_tAxisY)
			delete [] m_tAxisY;
	}

    /// \brief C'tor to allocate the buffer size and it memory
    PlotData(size_t N,std::string name) : m_nNdata(N), plotName(name)
	{
		m_tAxisX=new Tx[m_nNdata];
		m_tAxisY=new Ty[m_nNdata];
        std::fill_n(m_tAxisX,m_nNdata,Tx(0));
        std::fill_n(m_tAxisY,m_nNdata,Ty(0));
	}

    /// \brief Assignment operator
    /// \param pd the instance to copy
    /// \returns a reference to itself
    const PlotData & operator=(const PlotData<Tx,Ty> & pd)
	{
		if (m_nNdata!=pd.m_nNdata) {
			if (m_tAxisX) 
				delete [] m_tAxisX;
			if (m_tAxisY)
				delete [] m_tAxisY;
			m_nNdata=pd.m_nNdata;			
			m_tAxisX=new Tx[m_nNdata];
			m_tAxisY=new Ty[m_nNdata];
		}

        std::copy_n(pd.m_tAxisX,m_nNdata,m_tAxisX);
        std::copy_n(pd.m_tAxisY,m_nNdata,m_tAxisY);
        plotName = pd.plotName;

		return *this;
	}

    /// \brief Fill the container with data from two arrays
    /// \param x the x-axis data
    /// \param y the y-axis data
    /// \param N number of points in the arrays
	void SetData(Tx const * const x, Ty const * const y, const size_t N)
	{
		if (m_nNdata!=N) {
			if (m_tAxisX) 
				delete [] m_tAxisX;
			if (m_tAxisY)
				delete [] m_tAxisY;
			
			m_nNdata=N;
			m_tAxisX=new Tx[m_nNdata];
			m_tAxisY=new Ty[m_nNdata];
		}

		memcpy(m_tAxisX,x,m_nNdata*sizeof(Tx));
		memcpy(m_tAxisY,y,m_nNdata*sizeof(Ty));
	}

	void Clear() {
		memset(m_tAxisX,0,sizeof(Tx)*m_nNdata);
		memset(m_tAxisY,0,sizeof(Ty)*m_nNdata);
	}

    Tx * GetX() {return m_tAxisX;}
    Ty * GetY() {return m_tAxisY;}
	size_t Size() {return m_nNdata;}

    void setName(std::string name) {plotName = name;}
    std::string name() {return plotName;}

private:
	Tx * m_tAxisX;
	Ty * m_tAxisY;

	size_t m_nNdata;
    std::string plotName;
};

template <typename Tx, typename Ty>
PlotData<Tx,Ty>::PlotData(const kipl::containers::PlotData<Tx,Ty> & pd) : m_nNdata(pd.m_nNdata), plotName(pd.plotName)
{
    m_nNdata=pd.m_nNdata;
    m_tAxisX=new Tx[m_nNdata];
    m_tAxisY=new Ty[m_nNdata];

    std::copy_n(pd.m_tAxisX,m_nNdata,m_tAxisX);
    std::copy_n(pd.m_tAxisY,m_nNdata,m_tAxisY);

}
}}

#endif
