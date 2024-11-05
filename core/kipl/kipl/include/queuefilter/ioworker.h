

#ifndef __IOWORKER_H
#define __IOWORKER_H

#include <image/image.h>
#include <queuefilter/basequeueworker.h>
#include <mpi.h>
#include <misc/parenc.h>

namespace QueueFilter {
template<class ImgType>
class ioworker: public BaseQueueWorker<ImgType>
{
	public:
		ioworker(const string &fname, bool IO_mode, bool File_mode,ostream &os); 
		virtual ~ioworker() {}
		
		virtual int proc(Image::CImage<ImgType,3> &img) {
			if (io_mode)
				return load(img);
			else 
				return save(img);
			
			return -1;
		}
		
		virtual int reset() {return 0;}
		virtual int insert(Image::CImage<ImgType,2> &img, Image::CImage<ImgType,2> &res) {return -2;}
		virtual int size() {return 0;}
		virtual int kernelsize() {return 0;}
		virtual int GetWorkerParameters(vector<string> & parnames, vector<double> & parvals);
		
	protected:
		int save(Image::CImage<ImgType,3> &img);
		int load(Image::CImage<ImgType,3> &img);
		int makeFilename(string & fname, string &vname);
		string filename;
		bool io_mode;
		bool file_mode;
};

template<class ImgType>
ioworker<ImgType>::ioworker(const string &fname, 
							bool IO_mode, 
							bool File_mode,
							ostream &os) : BaseQueueWorker<ImgType>(os)
{
	io_mode=IO_mode; // Save
	file_mode=File_mode; // operate with slices
	this->procblock=true; // accept image as 3D, ignore 2D insertion
	filename=fname;
	this->workername="ioworker";
}

template<class ImgType>
int ioworker<ImgType>::save(Image::CImage<ImgType,3> &img)
{
	string savename,varname;
	makeFilename(savename,varname);
	
	if (file_mode) 
		Fileio::SliceWriteMAT(img,savename.c_str(),varname.c_str(),0,img.SizeZ());
	else 
		Fileio::WriteMAT(img,savename.c_str(), varname.c_str());
		
	return 0;
}

template<class ImgType>
int ioworker<ImgType>::load(Image::CImage<ImgType,3> &img)
{
	string loadname,varname;
	makeFilename(loadname,varname);
	
	if (file_mode) 
		Fileio::SliceReadMAT(img,loadname.c_str(),0,img.SizeZ());
	else 
		Fileio::ReadMAT(img,loadname.c_str());

	return 0;
}

template<class ImgType>
int ioworker<ImgType>::makeFilename(string & fname, string &vname)
{
	int flag;
	string basename;
	
	string fn,pa;
	vector<string> ext;
	ostringstream namestr, vnamestr;
	
	Parenc::StripFileName(filename,pa,fn,ext);
	
	MPI_Initialized(&flag);
	if (flag) {	
		int rank;
		MPI_Comm_rank(MPI_COMM_WORLD,&rank);
		namestr<<pa<<fn<<"_w"<<rank;
		vnamestr<<fn<<"_w"<<rank<<ends;
	}
	else {
		namestr<<pa<<fn;
		vnamestr<<fn<<ends;
	}

	if (file_mode) {
		namestr<<"_s*";
		vnamestr<<"_s*";
	}
		
	vector<string>::iterator it;
	
	for (it=ext.begin(); it!=ext.end(); it++)
		namestr<<*it;
		
	namestr<<ends;
	vnamestr<<ends;
	
	fname=namestr.str();
	vname=vnamestr.str();
	
	return 0;
}

template<class ImgType>
int ioworker<ImgType>::GetWorkerParameters(vector<string> & parnames, vector<double> & parvals)
{
	parnames.clear();
	parvals.clear();

	parnames.push_back("io_mode"); parvals.push_back(io_mode);
	parnames.push_back("file_mode"); parvals.push_back(file_mode);
	return 1;
}

}


#endif
