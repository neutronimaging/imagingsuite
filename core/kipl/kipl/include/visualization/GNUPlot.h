

#ifndef GNUPLOT_H_
#define GNUPLOT_H_

#include <cstdio>
#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include "../base/timage.h"

namespace kipl { namespace visualization {
class GNUPlot
{
	const std::string cm_GPpath;
public:
	enum ColorMaps {
		traditional,      // traditional pm3d (black-blue-red-yellow)
		green_red_violet,
		ocean,            // (green-blue-white); try also all other permutations
		hot,              // hot (black-red-yellow-white)
		color2gray,       // color printable on gray (black-blue-violet-yellow-white)
		rainbow,          // rainbow (blue-green-yellow-red)
		AFMhot			  // (black-red-yellow-white)
	};
	GNUPlot();
	virtual ~GNUPlot();
	size_t figure(size_t);
	bool plot(float w);
	bool plot(float const * const data, size_t const N, std::string label="");
	bool plot(double const * const data, size_t const N, std::string label="");
	bool plot(float const * const axis, float const * const data, size_t const N, std::string label="");
    bool image(float const * const data, const std::vector<size_t> & dims);
	bool image(const kipl::base::TImage<float,2> img);
	bool image(const kipl::base::TImage<int,2> img);
	bool image(const kipl::base::TImage<short,2> img);
	bool image(const kipl::base::TImage<unsigned char,2> img);
	bool image(const kipl::base::TImage<char,2> img);
	size_t CountFigures() {return figures.size();}
	size_t gcf() {return m_nCurrentFigure;}
	void Command(std::ostringstream &cmd);
	void Command(std::string cmd);
	void colormap(ColorMaps map);
	
protected:
	std::string OpenBinaryPlotFile(std::ofstream &outfile);
	size_t m_nCurrentFigure;
	static std::map<size_t,std::FILE *> figures;
};

std::ostream & operator<<(std::ostream &s, GNUPlot::ColorMaps map);

}}
#endif /*GNUPLOT_H_*/
