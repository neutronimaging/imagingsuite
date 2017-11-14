//<LICENSE>

#include "stdafx.h"
#include "VolumeProject.h"

#ifdef _OPENMP
#include <omp.h>
#endif
#include <ParameterHandling.h>

#include <strings/miscstring.h>
#include <math/image_statistics.h>
#include <math/median.h>
#include <io/io_tiff.h>
#include <base/KiplException.h>

VolumeProject::VolumeProject() :
KiplProcessModuleBase("VolumeProject"),
	m_Plane(kipl::base::ImagePlaneXY),
	m_sFileName("./projection.tif"),
	m_ProjectionMethod(ProjectMean)
{

}

VolumeProject::~VolumeProject() {
}


int VolumeProject::Configure(std::map<std::string, std::string> parameters)
{
	string2enum(GetStringParameter(parameters,"plane"),m_Plane);
	string2enum(GetStringParameter(parameters,"method"),m_ProjectionMethod);
	m_sFileName = GetStringParameter(parameters,"filename");
	
	return 0;
}

std::map<std::string, std::string> VolumeProject::GetParameters()
{
	std::map<std::string, std::string> parameters;

	parameters["plane"] = enum2string(m_Plane);
	parameters["method"] = enum2string(m_ProjectionMethod);
	parameters["filename"] = m_sFileName;

	return parameters;
}

int VolumeProject::ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff)
{
	kipl::base::TImage<float,2> proj;
	switch (m_ProjectionMethod) {
	case ProjectMean:    proj=MeanProject(img); break;
	case ProjectMedian:  proj=MedianProject(img); break;
	case ProjectMin:     proj=MinProject(img); break;
	case ProjectMax: 	 proj=MaxProject(img); break;
	}

	kipl::io::WriteTIFF32(proj,m_sFileName.c_str());

	return 0;
}


kipl::base::TImage<float,2> VolumeProject::MeanProject(kipl::base::TImage<float,3> & img)
{
	float *pSlice = img.GetDataPtr();

	ptrdiff_t slices=static_cast<ptrdiff_t>(img.Size(2));

	kipl::base::TImage<float,2> proj(img.Dims());
	float *pProj=proj.GetDataPtr();
	proj=0.0f;

	for (ptrdiff_t slice=0; slice<slices; slice++) {
		pSlice=img.GetLinePtr(0,slice);
		for (size_t i=0; i<proj.Size(); i++) {
			pProj[i]+=pSlice[i];
			pProj[i]*=0.5f;
		}
	}

	return proj;
}

kipl::base::TImage<float,2> VolumeProject::MedianProject(kipl::base::TImage<float,3> & img)
{
	kipl::base::TImage<float,2> proj(img.Dims());

	size_t N=img.Size(2);

	float *pProj=proj.GetDataPtr();
	float *pImg=img.GetDataPtr();
	float *pLine=new float[N];

	for (size_t i=0; i<proj.Size(); i++) {
		GetZLine(pImg+i,pLine,N,proj.Size());

		kipl::math::median(pLine,N,pProj+i);
	}

	delete [] pLine;

	return proj;
}

kipl::base::TImage<float,2> VolumeProject::MinProject(kipl::base::TImage<float,3> & img)
{
	float *pSlice = img.GetDataPtr();

	ptrdiff_t slices=static_cast<ptrdiff_t>(img.Size(2));

	kipl::base::TImage<float,2> proj(img.Dims());
	float *pProj=proj.GetDataPtr();

	memcpy(pProj,pSlice,sizeof(float)*proj.Size());

	for (ptrdiff_t slice=1; slice<slices; slice++) {
		pSlice=img.GetLinePtr(0,slice);
		for (size_t i=0; i<proj.Size(); i++) {
			pProj[i]=min(pProj[i],pSlice[i]);
		}
	}

	return proj;
}

kipl::base::TImage<float,2> VolumeProject::MaxProject(kipl::base::TImage<float,3> & img)
{
	float *pSlice = img.GetDataPtr();

	ptrdiff_t slices=static_cast<ptrdiff_t>(img.Size(2));

	kipl::base::TImage<float,2> proj(img.Dims());
	float *pProj=proj.GetDataPtr();

	memcpy(pProj,pSlice,sizeof(float)*proj.Size());

	for (ptrdiff_t slice=1; slice<slices; slice++) {
		pSlice=img.GetLinePtr(0,slice);
		for (size_t i=0; i<proj.Size(); i++) {
			pProj[i]=max(pProj[i],pSlice[i]);
		}
	}

	return proj;
}


int VolumeProject::GetZLine(float * pSrc, float *pDest, size_t N, size_t stride)
{
	for (size_t i=0; i<N; i++) {
		pDest[i]=pSrc[i*stride];
	}

	return 0;
}

std::string enum2string(eProjectionMethod method)
{
	switch (method) {
	case ProjectMean   : return "mean";
	case ProjectMedian : return "median";
	case ProjectMin    : return "min";
	case ProjectMax    : return "max";
	default            : throw kipl::base::KiplException("Failed to convert enum to string",__FILE__, __LINE__);
	}


	return "conversion failed";
}

void string2enum(std::string str, eProjectionMethod &method)
{

	if (str=="mean")        method = ProjectMean;
	else if (str=="median") method = ProjectMedian;
	else if (str=="min")    method = ProjectMin;
	else if (str=="max")    method = ProjectMax;
	else throw kipl::base::KiplException("Failed to interpret string",__FILE__,__LINE__);


}
