//<LICENCE>

#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>
#ifdef _MSC_VER
    #include <io.h>
#endif
//#include "../../base/timage.h" 
#include "../../include/visualization/GNUPlot.h"
#include "../../include/base/KiplException.h"

using namespace std;

namespace kipl { namespace visualization {
std::map<size_t,std::FILE *> GNUPlot::figures;
#ifdef __GNUC__
GNUPlot::GNUPlot() : cm_GPpath("gnuplot"), m_nCurrentFigure(0)
{
}
#elif defined(_MSC_VER)
//GNUPlot::GNUPlot() : cm_GPpath("pgnuplot.exe /noend"), m_nCurrentFigure(0)
GNUPlot::GNUPlot() : cm_GPpath("pgnuplot.exe"), m_nCurrentFigure(0)
{
}
#else
GNUPlot::GNUPlot() 
{
	throw kipl::base::KiplException("GNU-plot is not supported on this system");
}
#endif

GNUPlot::~GNUPlot()
{
}

size_t GNUPlot::figure(size_t n)
{
	if (0<figures.count(n)) {
        std::cout<<figures[n]<<std::endl;
	}
	else {
#ifdef __GNUC__
		FILE *tmp=popen(cm_GPpath.c_str(),"w");
#else 
		FILE *tmp=_popen(cm_GPpath.c_str(),"w");
#endif
        if (tmp==nullptr)
			kipl::base::KiplException("Failed to open figure",__FILE__, __LINE__);
		else {
			figures[n] = tmp;
		}
	}
	
	m_nCurrentFigure=n;
	return n;
}

bool GNUPlot::plot(float w)
{
	if (figures.empty())
		figure(0);

	ostringstream cmd;
    cmd<<"set title 'test "<<w<<"'; plot sin("<<w<<"*x) with lines"<<std::endl;
	
	Command(cmd);
	
	return true;
}

bool GNUPlot::plot(float const * data, size_t const N, string label)
{
	if (figures.empty())
		figure(0);

	ostringstream cmd;
	ofstream outfile;
	string plotname=OpenBinaryPlotFile(outfile);

	outfile.write(reinterpret_cast<char const *>(data),N*sizeof(float));
	outfile.close();
	
	cmd<<"plot '"<<plotname<<"' binary array="<<N;
	if (!label.empty())
		cmd<<" title '"<<label<<"' ";
	cmd<<" with lines";

	Command(cmd);
	return true;
}

bool GNUPlot::plot(double const * data, size_t const N, string label)
{
	if (figures.empty())
		figure(0);

	ostringstream cmd;
	ofstream outfile;
	string plotname=OpenBinaryPlotFile(outfile);

	outfile.write(reinterpret_cast<char const *>(data),N*sizeof(double));
	outfile.close();
	
	cmd<<"plot '"<<plotname<<"' binary array="<<N<<" format=\"%%double\"";
	if (!label.empty())
		cmd<<"title '"<<label<<"' ";
	cmd<<" with lines";

	Command(cmd);
	return true;
}

bool GNUPlot::plot(float const * axis, float const * data, size_t const N, std::string label)
{
	if (figures.empty())
		figure(0);

	ostringstream cmd;
	ofstream outfile;
	string plotname=OpenBinaryPlotFile(outfile);

	outfile.write(reinterpret_cast<char const *>(axis),N*sizeof(float));
	outfile.write(reinterpret_cast<char const *>(data),N*sizeof(float));
	
	outfile.close();
	
	cmd<<"plot '"<<plotname<<"' binary array="<<N<<":"<<N;
	if (!label.empty())
		cmd<<"title '"<<label<<"' ";
	cmd<<" with lines";

	Command(cmd);
	return true;
}

bool GNUPlot::image(const kipl::base::TImage<float,2> img)
{
    return image(img.GetDataPtr(), img.dims());
}

bool GNUPlot::image(float const * data, const std::vector<size_t> & dims)
{
	ostringstream cmd;
	
	if (figures.empty())
		figure(0);

	ofstream outfile;
	string plotname=OpenBinaryPlotFile(outfile);
	
	outfile.write(reinterpret_cast<char const *>(data),dims[0]*dims[1]*sizeof(float));
	outfile.close();
	
	cmd<<"set xrange [-0.5:"<<dims[0]-0.5<<"]; set yrange [-0.5:"<<dims[1]-0.5<<"]; plot '"<<plotname<<"' binary array="<<dims[0]<<"x"<<dims[1]<<" with image";
	
	Command(cmd);
	return true;
}

bool GNUPlot::image(const kipl::base::TImage<short,2> img)
{
	float *pData=new float[img.Size()];
	short const * pImg=img.GetDataPtr();
	
	for (size_t i=0; i<img.Size(); i++)
		pData[i]=pImg[i];
	
    bool status=image(pData, img.dims());
	
	delete [] pData;
	
	return status; 
}

bool GNUPlot::image(const kipl::base::TImage<int,2> img)
{
	float *pData=new float[img.Size()];
	int const * pImg=img.GetDataPtr();
	
	for (size_t i=0; i<img.Size(); i++)
		pData[i]=static_cast<float>(pImg[i]);
	
    bool status=image(pData, img.dims());
	
	delete [] pData;
	
	return status; 
}

bool GNUPlot::image(const kipl::base::TImage<unsigned char,2> img) 
{
	float *pData=new float[img.Size()];
	unsigned char const * pImg=img.GetDataPtr();
	
	for (size_t i=0; i<img.Size(); i++)
		pData[i]=static_cast<float>(pImg[i]);
	
    bool status=image(pData, img.dims());
	
	delete [] pData;
	
	return status; 
}

bool GNUPlot::image(const kipl::base::TImage<char,2> img)
{
	float *pData=new float[img.Size()];
	char const * pImg=img.GetDataPtr();
	
	for (size_t i=0; i<img.Size(); i++)
		pData[i]=static_cast<float>(pImg[i]);
	
    bool status=image(pData, img.dims());
	
	delete [] pData;
	
	return status; 	
}


void GNUPlot::Command(ostringstream &cmd)
{
    cmd<<std::endl;

	fputs(cmd.str().c_str(),figures[m_nCurrentFigure]);
	fflush(figures[m_nCurrentFigure]);
}

void GNUPlot::Command(string cmd)
{
	cmd+="\n";
	fputs(cmd.c_str(),figures[m_nCurrentFigure]);
	fflush(figures[m_nCurrentFigure]);
}

string GNUPlot::OpenBinaryPlotFile(ofstream &outfile) 
{

//	char name[256];
//	
//	strcpy(name,"gnuplotXXXXXX");
//
//	//
//	//open temporary files for output
//	//
//
//#ifdef _MSC_VER
//	if (_mktemp(name) == nullptr)
//#else
//    if (mkstemp(name) == -1)
//#endif    
//	{
//		cerr << "Cannot create temporary file name" << endl;
//		return string("");
//	}
	std::ostringstream name;

	name<<"GNUPlotFigure_"<<m_nCurrentFigure<<".dat";
	outfile.open(name.str().c_str(), ios_base::binary | ios_base::out);

	if (outfile.bad())
	{
        std::cerr << "Cannot create temorary file" << name.str() << std::endl;
		return string("");
	}

	return name.str();
}

void GNUPlot::colormap(ColorMaps map)
{
	ostringstream cmd;
	
	cmd<<"set palette color rgbformulae ";
	switch (map) {
	case traditional:         // traditional pm3d (black-blue-red-yellow)
		cmd<<"7,5,15"; break;
	case green_red_violet:    // green-red-violet
		cmd<<"3,11,6"; break;
	case ocean: 		      // ocean (green-blue-white); try also all other permutations
		cmd<<"23,28,3"; break;
	case hot:                 // hot (black-red-yellow-white)
		cmd<<"21,22,23"; break;
	case color2gray:       // color printable on gray (black-blue-violet-yellow-white)
		cmd<<"30,31,32"; break;
	case rainbow:          // rainbow (blue-green-yellow-red)
		cmd<<"33,13,10"; break;
	case AFMhot:			  // (black-red-yellow-white)
		cmd<<"34,35,36"; break;
	};
	cmd<<"; replot";
	
	Command(cmd);
}

std::ostream & operator<<(std::ostream &s, GNUPlot::ColorMaps map)
{
	switch (map) {
		case GNUPlot::traditional:         // traditional pm3d (black-blue-red-yellow)
			s<<"traditional"; break;
		case GNUPlot::green_red_violet:    // green-red-violet
			s<<"green-red-violett"; break;
		case GNUPlot::ocean: 		      // ocean (green-blue-white); try also all other permutations
			s<<"ocean"; break;
		case GNUPlot::hot:                 // hot (black-red-yellow-white)
			s<<"hot"; break;
		case GNUPlot::color2gray:       // color printable on gray (black-blue-violet-yellow-white)
			s<<"color printable on gray"; break;
		case GNUPlot::rainbow:          // rainbow (blue-green-yellow-red)
			s<<"rainbow"; break;
		case GNUPlot::AFMhot:			  // (black-red-yellow-white)
			s<<"AFM hot"; break;
	}
	
	return s;
		
}
//#else 
//GNUPlot::GNUPlot() : cm_GPpath("/usr/bin/gnuplot"), m_nCurrentFigure(0)
//{
//}
//
//GNUPlot::~GNUPlot()
//{
//}
//
//size_t GNUPlot::figure(size_t n)
//{
//	if (0<figures.count(n)) {
//		std::cout<<figures[n]<<std::endl;
//	}
//	else {
//		
//		FILE *tmp=nullptr; //popen(cm_GPpath.c_str(),"w");
//		if (tmp==nullptr)
//			std::cerr<<"Failed to open figure"<<std::endl;
//		else {
//			figures[n] = tmp;
//			std::cout<<"Figure "<<n<<" is opened"<<std::endl;
//		}
//	}
//	
//	m_nCurrentFigure=n;
//	return n;
//}
//
//bool GNUPlot::plot(float w)
//{
//	if (figures.empty())
//		figure(0);
//
//	ostringstream cmd;
//	cmd<<"set title 'test "<<w<<"'; plot sin("<<w<<"*x) with lines"<<std::endl;
//	
//	Command(cmd);
//	
//	return true;
//}
//
//bool GNUPlot::plot(float const * data, size_t const N, string label)
//{
//	if (figures.empty())
//		figure(0);
//
//	ostringstream cmd;
//	ofstream outfile;
//	string plotname=OpenBinaryPlotFile(outfile);
//
//	outfile.write(reinterpret_cast<char const *>(data),N*sizeof(float));
//	outfile.close();
//	
//	cmd<<"plot '"<<plotname<<"' binary array="<<N;
//	if (!label.empty())
//		cmd<<"title '"<<label<<"' ";
//	cmd<<" with lines";
//
//	Command(cmd);
//	return true;
//}
//
//bool GNUPlot::plot(float const * axis, float const * data, size_t const N, std::string label)
//{
//	if (figures.empty())
//		figure(0);
//
//	ostringstream cmd;
//	ofstream outfile;
//	string plotname=OpenBinaryPlotFile(outfile);
//
//	outfile.write(reinterpret_cast<char const *>(axis),N*sizeof(float));
//	outfile.write(reinterpret_cast<char const *>(data),N*sizeof(float));
//	
//	outfile.close();
//	
//	cmd<<"plot '"<<plotname<<"' binary array="<<N<<"x2 ";
//	if (!label.empty())
//		cmd<<"title '"<<label<<"' ";
//	cmd<<" with lines";
//
//	Command(cmd);
//	return true;
//}
//
//bool GNUPlot::image(const kipl::base::TImage<float,2> img)
//{
//	return image(img.GetDataPtr(), img.Dims());
//}
//
//bool GNUPlot::image(float const * data, size_t const * dims)
//{
//	ostringstream cmd;
//	
//	if (figures.empty())
//		figure(0);
//
//	ofstream outfile;
//	string plotname=OpenBinaryPlotFile(outfile);
//
//	
//	outfile.write(reinterpret_cast<char const *>(data),dims[0]*dims[1]*sizeof(float));
//	
//	outfile.close();
//	
//	cmd<<"set xrange [-0.5:"<<dims[0]-0.5<<"]; set yrange [-0.5:"<<dims[1]-0.5<<"]; plot '"<<plotname<<"' binary array="<<dims[0]<<"x"<<dims[1]<<" with image";
//	
//	
//	Command(cmd);
//	return true;
//}
//
//void GNUPlot::Command(ostringstream &cmd)
//{
//	cmd<<std::endl;
//	fputs(cmd.str().c_str(),figures[m_nCurrentFigure]);
//	fflush(figures[m_nCurrentFigure]);
//}
//
//void GNUPlot::Command(string cmd)
//{
//	cmd+="\n";
//	fputs(cmd.c_str(),figures[m_nCurrentFigure]);
//	fflush(figures[m_nCurrentFigure]);
//}
//
//string GNUPlot::OpenBinaryPlotFile(ofstream &outfile) 
//{
//	char name[256];
//	strcpy(name,"./gnuplotXXXXXX");
//
//	//
//	//open temporary files for output
//	//
////	if (mkstemp(name) == -1)
//	if (1)
//	{
//		cerr << "Cannot create temporary file name" << endl;
//		return string("");
//	}
//	outfile.open(name, ios_base::binary | ios_base::out);
//	
//	if (outfile.bad())
//	{
//		cerr << "Cannot create temorary file" << name << endl;
//		return string("");
//	}
//	
//	return string(name);
//}
//
//void GNUPlot::colormap(ColorMaps map)
//{
//	ostringstream cmd;
//	
//	cmd<<"set palette color rgbformulae ";
//	switch (map) {
//	case traditional:         // traditional pm3d (black-blue-red-yellow)
//		cmd<<"7,5,15"; break;
//	case green_red_violet:    // green-red-violet
//		cmd<<"3,11,6"; break;
//	case ocean: 		      // ocean (green-blue-white); try also all other permutations
//		cmd<<"23,28,3"; break;
//	case hot:                 // hot (black-red-yellow-white)
//		cmd<<"21,22,23"; break;
//	case color2gray:       // color printable on gray (black-blue-violet-yellow-white)
//		cmd<<"30,31,32"; break;
//	case rainbow:          // rainbow (blue-green-yellow-red)
//		cmd<<"33,13,10"; break;
//	case AFMhot:			  // (black-red-yellow-white)
//		cmd<<"34,35,36"; break;
//	};
//	cmd<<"; replot";
//	
//	Command(cmd);
//}
//
//std::ostream & operator<<(std::ostream &s, GNUPlot::ColorMaps map)
//{
//	switch (map) {
//		case GNUPlot::traditional:         // traditional pm3d (black-blue-red-yellow)
//			s<<"traditional"; break;
//		case GNUPlot::green_red_violet:    // green-red-violet
//			s<<"green-red-violett"; break;
//		case GNUPlot::ocean: 		      // ocean (green-blue-white); try also all other permutations
//			s<<"ocean"; break;
//		case GNUPlot::hot:                 // hot (black-red-yellow-white)
//			s<<"hot"; break;
//		case GNUPlot::color2gray:       // color printable on gray (black-blue-violet-yellow-white)
//			s<<"color printable on gray"; break;
//		case GNUPlot::rainbow:          // rainbow (blue-green-yellow-red)
//			s<<"rainbow"; break;
//		case GNUPlot::AFMhot:			  // (black-red-yellow-white)
//			s<<"AFM hot"; break;
//	}
//	
//	return s;
//		
//}
//
//#endif
}}
