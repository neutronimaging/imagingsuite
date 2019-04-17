//<LICENCE>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <limits>

#include <zlib.h>

#include "../../../include/io/core/matlabio.h"
#include "../../../include/base/KiplException.h"


using namespace std;
namespace kipl { namespace io {

int KIPLSHARED_EXPORT GetMATDims(char const * const filename,size_t * dims)
{
    return 0;
}

namespace core {
	/// Enum for the supported data type in a MAT file
	typedef enum {
		mi_UNKNOWN=(unsigned int)0,
		mi_INT8,
		mi_UINT8,
		mi_INT16,
		mi_UINT16,
		mi_INT32,
		mi_UINT32,
		mi_SINGLE,
		mi_RES1,
		mi_DOUBLE,
		mi_RES2,
		mi_RES3,
		mi_INT64,
		mi_UINT64,
		mi_MATRIX,
		mi_COMPRESSED,
		mi_UTF8,
		mi_UTF16,
		mi_UTF32	
	} miDataType;

	
	const size_t datatypesize[]={0,0,0,0,0,0,sizeof(double), sizeof(float),1,1,2,2,4,4};
	
	miDataType mxmitype[]={mi_UNKNOWN,
		mi_UNKNOWN,
		mi_UNKNOWN,
		mi_UNKNOWN,
		mi_UNKNOWN,
		mi_UNKNOWN,//,mi_UNKNOWN,mi_UNKNOWN,
		mi_DOUBLE,
		mi_SINGLE,
		mi_INT8,
		mi_UINT8,
		mi_INT16,
		mi_UINT16,
		mi_INT32,
		mi_UINT32,
		mi_INT64,
		mi_UINT64,
		mi_MATRIX,
		mi_COMPRESSED,
		mi_UTF8,
		mi_UTF16,
		mi_UTF32	};
	mxClassID mimxtype[]= {
		mxUNKNOWN_CLASS,
		mxINT8_CLASS,
		mxUINT8_CLASS,
		mxINT16_CLASS,
		mxUINT16_CLASS,
		mxINT32_CLASS,
		mxUINT32_CLASS,
		mxSINGLE_CLASS,
		mxUNKNOWN_CLASS,
		mxDOUBLE_CLASS,
		mxUNKNOWN_CLASS,
		mxUNKNOWN_CLASS,
		mxINT64_CLASS,
		mxUINT64_CLASS,
		mxUNKNOWN_CLASS,
		mxUNKNOWN_CLASS,
		mxUNKNOWN_CLASS,
		mxUNKNOWN_CLASS,
		mxUNKNOWN_CLASS};
	
	mxClassID GetMatlabClassID(float a)                { return mxSINGLE_CLASS;}
	mxClassID GetMatlabClassID(double a)               { return mxDOUBLE_CLASS;}
	mxClassID GetMatlabClassID(char a)                 { return mxINT8_CLASS;}
	mxClassID GetMatlabClassID(unsigned char a)       { return mxUINT8_CLASS;}
	mxClassID GetMatlabClassID(short a)                { return mxINT16_CLASS;}
	mxClassID GetMatlabClassID(unsigned short a)      { return mxUINT16_CLASS;}
	mxClassID GetMatlabClassID(int a)                  { return mxINT32_CLASS;}
	mxClassID GetMatlabClassID(unsigned int a)        { return mxUINT32_CLASS;}
	mxClassID GetMatlabClassID(long long a)           { return mxINT64_CLASS;}
	mxClassID GetMatlabClassID(unsigned long long a) { return mxUINT64_CLASS;}
	
	/// Array containing the sizes of each MAT data type 
	const size_t mitypesize[15]={0,
		sizeof(char), sizeof(unsigned char), 
		sizeof(short), sizeof(unsigned short),
		sizeof(int), sizeof(unsigned int),
		sizeof(float), sizeof(int),
		sizeof(double), sizeof(int), sizeof(int),
		2*sizeof(int), 2*sizeof(unsigned int),
		sizeof(int)};

	/// Struct used for the file header of a MAT file
	typedef struct matfh {
		/// Initial comment field
		char text[116];
		/// Offset to subsystem data
		unsigned int subsys_data_offset[2];
		/// Matlab version 
		unsigned short version;
		/// Endian descriptor
		char endian[2];
		
	} MATFile_header;

	/// Data tag used in MAT files
	typedef struct dtag {
		miDataType type;
		unsigned int size;
	} DataTag;
	
	/// Tag used to describe an array in MAT files
	typedef struct fhead {
		DataTag arraytag;
		unsigned int flags;
		unsigned int unused;
	} ArrayFlags;
	
	size_t sizeofMAT(mxClassID id)
	{
		switch (id) {
		case mxUNKNOWN_CLASS  : return  0;
		case mxCELL_CLASS     : return 0;
		case mxSTRUCT_CLASS   : return 0;
		case mxLOGICAL_CLASS  : return 1;
		case mxCHAR_CLASS     : return sizeof(char);
		case mxSPARSE_CLASS   : return 0;		// OBSOLETE! DO NOT USE 
		case mxDOUBLE_CLASS   : return sizeof(double);
		case mxSINGLE_CLASS   : return sizeof(float);
		case mxINT8_CLASS     : return sizeof(char);
		case mxUINT8_CLASS    : return sizeof(unsigned char);
		case mxINT16_CLASS    : return sizeof(short);
		case mxUINT16_CLASS   : return sizeof(unsigned short);
		case mxINT32_CLASS    : return sizeof(int);
		case mxUINT32_CLASS   : return sizeof(unsigned int);
		case mxINT64_CLASS    : return sizeof(long long);		// place holder - future enhancements 
		case mxUINT64_CLASS   : return sizeof(unsigned long long);		// place holder - future enhancements 
		case mxFUNCTION_CLASS : return 0;
		case mxOPAQUE_CLASS   : return 0;
		case mxOBJECT_CLASS   : return 0;
		}
		return 0;
	}

    int KIPLSHARED_EXPORT ReadMATmatrix(char **data, size_t *dims, int &NDim, mxClassID  & type, char  **varname, const char *fname)
	{
		FILE *inf;
		stringstream s;
        if ((inf=fopen(fname,"rb"))==nullptr) {
			s.str("");
			s<<"ReadMATmatrix: File opening error: "<<fname;
			throw kipl::base::KiplException(s.str(),__FILE__,__LINE__);
		}

		MATFile_header head;
		DataTag tag;
		size_t i;
		
		fread(&head,sizeof(MATFile_header),1,inf);
		fread(&tag,sizeof(tag),1,inf);

        Bytef *buffer=nullptr;
        Bytef *cbuffer=nullptr;
		Bytef *pData;
		DataTag &t=tag;
		
		try {
			cbuffer=new Bytef[tag.size];
		}
		catch (const std::bad_alloc & E) {
			s.str("");
			s<<"Bad alloc in ReadMATmatrix (cbuffer), "<<E.what();
			throw kipl::base::KiplException(s.str(),__FILE__,__LINE__);
		}
		unsigned long readsize;
		readsize=fread(cbuffer,sizeof(char),tag.size,inf);
		fclose(inf);

		unsigned long offset=0;
		uLongf destLen=tag.size<<1;
		if (tag.type==mi_COMPRESSED) {
			try {
				buffer=new Bytef[destLen];
			}
			catch (const std::bad_alloc & E) {
				s.str("");
				s<<"Bad alloc in ReadMATmatrix (buffer), "<<E.what();
				throw kipl::base::KiplException(s.str(),__FILE__,__LINE__);
			}
			int status;

			status=uncompress(buffer, &destLen, cbuffer,tag.size);

			while (status!=Z_OK) {
				switch (status) {
				case Z_MEM_ERROR:
					s.str("");
					s<<"ReadMATmatrix (Zlib: Out of memory)";
					throw kipl::base::KiplException(s.str(),__FILE__,__LINE__);
					break;
				case Z_BUF_ERROR: 
					delete [] buffer;
					destLen=destLen<<1;
					buffer=new Bytef[destLen];
					
					break;
				case Z_DATA_ERROR:
					s.str("");
					s<<"ReadMATmatrix (Zlib: Corrupted data)";
					throw kipl::base::KiplException(s.str(),__FILE__,__LINE__);
					break;
				default:
					s.str("");
					s<<"ReadMATmatrix (Zlib: Unknown error)";
					throw kipl::base::KiplException(s.str(),__FILE__,__LINE__);
				}
				status=uncompress(buffer, &destLen, cbuffer,tag.size);
			}
			t=*((DataTag*) buffer);
			
			pData=buffer+sizeof(DataTag);
				
			delete [] cbuffer;
            cbuffer=nullptr;
		} 
		else {
			pData=cbuffer;
		}
		
			
		if (t.type!=mi_MATRIX) {
			s.str("");
			s<<"ReadMATmatrix: Content is not an array";
			throw kipl::base::KiplException(s.str(),__FILE__,__LINE__);
		}

		ArrayFlags &arrayflags=*((ArrayFlags*)(pData));
		offset+=sizeof(ArrayFlags);
				
		type=(mxClassID)(arrayflags.flags & 255);
		if (type<mxDOUBLE_CLASS) {
			s.str("");
			s<<"ReadMATmatrix: Unsupported datatype";
			throw kipl::base::KiplException(s.str(),__FILE__,__LINE__);
		}
		if (arrayflags.flags & 0xFF00) {
			int tmp=arrayflags.flags>>8;
			s.str("");
			if (tmp & 8) 
				cerr<<"ReadMATmatrix: Data is complex valued, only real part will be read"<<endl;
			if (tmp & 4) {
				s<<"ReadMATmatrix: Data is of Global type, not supported";
				throw kipl::base::KiplException(s.str(),__FILE__,__LINE__);
			}
			if (tmp & 2) {
				s<<"ReadMATmatrix: Data is of Logical type, not supported";
				throw kipl::base::KiplException(s.str(),__FILE__,__LINE__);
			}
		}
		
		// Get dimensions
		DataTag &dimstag=*((DataTag*)(pData+offset));
		offset+=sizeof(DataTag);
		NDim=dimstag.size/4;
		
	
		int tempdims[8];
		memcpy(tempdims,pData+offset,sizeof(int)*NDim);
		
		
		// Filling upp other dimension sizes with length 1
        int ii;
        for (ii=0; ii<NDim; ii++) {dims[ii]=tempdims[ii]; }
        for (ii=NDim; ii<8; ii++) dims[ii]=1;
		
		offset+=sizeof(int)*(NDim+NDim%2);
		
		// Get variable name
		DataTag nametag;
		if (((short*)(pData+offset))[1]) {
			nametag.type=(miDataType)*((short*)(pData+offset));
			nametag.size=*(short*)(pData+offset+2);
			offset+=4;
		} 
		else {
			memcpy(&nametag,(pData+offset),sizeof(DataTag));
			offset+=sizeof(DataTag);
		}
		
		if (*varname) delete [] (*varname);
		(*varname)=new char[nametag.size+1];
		
		memcpy(*varname,pData+offset,nametag.size);
		(*varname)[nametag.size]=0;
		
		offset+=nametag.size;
		if (offset%8)
			offset+=(8-offset%8);
		
		// Read the data
		
		DataTag &datatag=*((DataTag*)(pData+offset));
		long nelements=1;
        for (int j=0; j<NDim; j++) {
			nelements*=dims[j];
		}
	
		offset+=sizeof(DataTag);
		
		if (mimxtype[datatag.type]!=type) {
			type=mimxtype[datatag.type];
		}

		size_t datasize=nelements*datatypesize[type];
		
		if (datasize!=datatag.size) {
			s.str("");
			s<<"ReadMATmatrix: Size error for the datablock (comp size="
				<<datasize<<", tag size="<<datatag.size<<")";
			throw kipl::base::KiplException(s.str(),__FILE__,__LINE__);
		}
		
		if (*data) 
			delete [] (*data);

		(*data)=new char [datasize];
		
		memcpy(*data,pData+offset,datasize);
		
		if (cbuffer) delete [] cbuffer;
		if (buffer) delete [] buffer;
		
		return 1;
	}

    int KIPLSHARED_EXPORT WriteMATmatrix(void *data, const size_t * dims, size_t NDim, mxClassID type, const char *varname, const char *fname)
//	int WriteMATmatrix(void *data, const size_t * dims, size_t NDim, mxClassID type, const char *varname, const char *fname)
	{
		stringstream s;
		char infotext[124];
		char dummy[]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
		unsigned int i;
		
		size_t ndata=dims[0];
		for (i=1; i<NDim; i++) ndata*=dims[i];

		if (long(ndata)*datatypesize[type]>numeric_limits<unsigned int>::max()) {
			s.str("");
			s<<"WriteMATmatrix: Image is too large to be handled without compression";
			throw kipl::base::KiplException(s.str(),__FILE__,__LINE__);
		}
		
		strcpy(infotext,"File generated by WriteMATmatrix, PC");
		for (i=strlen(infotext); i<124; i++) infotext[i]=' ';

		FILE *of;
        if ((of=fopen(fname,"wb"))==nullptr) {
			s.str("");
			s<<"WriteMATmatrix: File opening error"<<fname;
			throw kipl::base::KiplException(s.str(),__FILE__,__LINE__);
		}

		// Info header
		fwrite(infotext, sizeof(char), 124, of);
		// Version
		unsigned short stmp;
		stmp=0x0100;
		fwrite(&stmp,sizeof(unsigned short), 1, of);

		// Endian indicator
		char ei[]="IM";
		fwrite(ei,sizeof(char),2,of);

		// ********** Matrix
		// Set data type
		int datatype=(int)mi_MATRIX; // Matlab array
		fwrite(&datatype, sizeof(int), 1, of);

		// Set datasize
		unsigned int datasize;
		int namelen=strlen(varname);
		int nametype;
		int nm;
		if (namelen<=4) {
			nm=namelen%4;
			if (nm)
				namelen=namelen+(8-nm);
			nametype=4;
		}
		else {
			nm=namelen%8;
			if (nm)
				namelen=namelen+(8-nm);
			nametype=8;
		}

		//datasize=ndata*datatypesize[type]+40+namelen+nametype;
		datasize=ndata*datatypesize[type]+32+(NDim+(NDim%2))*4+namelen+nametype;
		
		
		unsigned int m=datasize%8;
		if (m)
			datasize=datasize+(8-m);
		
		fwrite(&datasize,sizeof(unsigned int),1,of);
		long hpos=ftell(of);
		// ********** Flags
		// set length for flags
		datatype=(int)mi_UINT32;
		fwrite(&datatype,sizeof(int),1,of);
		datasize=8;
		fwrite(&datasize,sizeof(unsigned int),1,of);
		// 2 undefined bytes
		//fwrite(dummy,1,2,of);

		// set the flags
        char flags[]={static_cast<char>(type),0,0,0}; // real values, nonglobal, nonlogical, single precision
		int *iflag;
		iflag=(int *) flags;
		fwrite(iflag,sizeof(int),1,of);


		// 4 undefined bytes
		fwrite(dummy,sizeof(char),4,of);

		// ********** Dimension array
		// set length for dim array
		datatype=mi_INT32;
		//datatype=mi_UINT32;
		fwrite(&datatype,sizeof(int),1,of);
/*
		datasize=4*(NDim+(NDim%2));
		fwrite(&datasize,sizeof(unsigned int),1,of);

		// write the sizes
		for (i=0; i<(NDim+(NDim%2)); i++) 
			fwrite(&dims[i], sizeof(int),1,of);
*/

		datasize=sizeof(unsigned int)*NDim;
		fwrite(&datasize,sizeof(unsigned int),1,of);
		int tmpDims[8]={0,0,0,0,0,0,0,0};
		for (size_t i=0; i<NDim; i++)
			tmpDims[i]=static_cast<int>(dims[i]);
		
		fwrite(tmpDims, sizeof(int),NDim,of);
		
		if (NDim % 2)
			fwrite(dummy, sizeof(int),1,of);

		
		// ********** Variable name
		// set length for name array

		short sdatatype=1;
		short sdatasize=strlen(varname);
		switch(nametype) {
		case 8:
			datatype=(int)mi_INT8;
			datasize=strlen(varname);

			fwrite(&datatype,sizeof(int),1,of);
			fwrite(&datasize,sizeof(unsigned int),1,of);
			break;
		case 4:
			sdatatype=(short)mi_INT8;
			sdatasize=strlen(varname);
			datasize=sdatasize;
			fwrite(&sdatatype,sizeof(short),1,of);
			fwrite(&sdatasize,sizeof(short),1,of);
			break;
		}


		// write the name
		fwrite(varname,1,datasize,of);
		if (nm)
			fwrite(dummy,1,nametype-nm,of);

		// ********** Write data
		// set length for data array
		datatype=(int)mxmitype[type];
		fwrite(&datatype,sizeof(int),1,of);
		datasize=ndata*datatypesize[type];
		
		fwrite(&datasize,sizeof(unsigned int),1,of);

		// write the data
		const unsigned int chunk=1<<20;
		unsigned int N=datasize/chunk;
		unsigned int pos;

		for (i=0, pos=0; i<N; i++, pos+=chunk)
			fwrite((char *)data+pos,1,chunk,of);

		N=datasize%chunk;
		
		if (N)
			fwrite((char *)data+pos,1,N,of);
		
		long fpos=ftell(of);
		// 0x88 =136bytes is the length of the filehead
		//m=(fpos-0x88)%8;
		m=(fpos-hpos)%8;

		if (m)
			fwrite(dummy,1,8-m,of);

		fclose(of);

		return 0;
	}

    int KIPLSHARED_EXPORT ReadMAThead(FILE **inf, int & rows, int & cols, mxClassID  & type, char  **varname, char *fname)
	{
		char infotext[124];
		char dummy[16];

        if ((*inf=fopen(fname,"rb"))==nullptr) { cerr<<"ReadMAThead: File opening error"; exit(0);}

		// Info header
		fread(infotext, 1, 124, *inf);
		// Version
		unsigned short stmp;
		fread(&stmp,sizeof(unsigned short), 1, *inf);

		// Endian indicator
		char ei[2];
		fread(ei,1,2,*inf);

		// ********** Matrix
		// Set data type
		miDataType datatype; // Matlab array
		fread(&datatype, sizeof(int), 1, *inf);
		if (datatype!=mi_MATRIX) {cerr<<"ReadMAThead: Content is not an array"; exit(0);}
		// Set datasize
		int ndatasize;

		fread(&ndatasize,sizeof(int),1,*inf);

		// ********** Flags
		// set length for flags
		int datasize;
		fread(&datatype,sizeof(int),1,*inf);

		fread(&datasize,sizeof(int),1,*inf);
		// 2 undefined bytes
		//fwrite(dummy,1,2,of);

		// set the flags
		//char flags[]={6,0,0,0}; // real values, nonglobal, nonlogical, single precision

		//iflag=(int *) flags;
		fread(&type,sizeof(int),1,*inf);
		//if (iflag!=6) {cerr<<"Wrong array content"<<endl; exit(0);}
		if (type<mxDOUBLE_CLASS) {
			cerr<<"ReadMAThead: Unsupported datatype"<<endl;
			exit(0);
		}
		// 4 undefined bytes
		fread(dummy,1,4,*inf);

		// ********** Dimension array
		// set length for dim array
		datatype=mi_INT32;
		fread(&datatype,mitypesize[mi_INT32],1,*inf);
		datasize=8;
		fread(&datasize,sizeof(int),1,*inf);

		// write the sizes
		//	int r,c;
		fread(&rows,sizeof(int),1,*inf);
		fread(&cols,sizeof(int),1,*inf);

		// ********** Variable name
		// set length for name array
		union int2short {
					int i;
					short s[2];
				};
		int sdatasize,nm,namesize;
		int2short sdatatype;

		fread(&sdatatype,sizeof(int),1,*inf);
	
//		if ((reinterpret_cast<short*>(&sdatatype))[1]) {
//			sdatasize=reinterpret_cast<short*>(&sdatatype)[0];
		if (sdatatype.s[1]) {
			sdatasize=sdatatype.s[0];		
			nm=sdatasize%4;
			namesize=4;
		}
		else {
			fread(&sdatasize,sizeof(int),1,*inf);
			nm=sdatasize%8;
			namesize=8;
		}

		if (*varname) delete [] (*varname);
		(*varname)=new char[sdatasize+1];
		// write the sizes
		fread(*varname,1,sdatasize,*inf);
		(*varname)[sdatasize]=0;

		if (nm)
			fread(dummy,1,namesize-nm,*inf);

		// ********** Read data
		// set length for data array
		fread(&datatype,sizeof(int),1,*inf);

		fread(&datasize,sizeof(int),1,*inf);

		if (rows*cols*static_cast<int>(mitypesize[datatype])!=datasize) {
			fclose(*inf);
			cerr<<"ReadMAThead: Size/type missmatch"<<endl;
			exit(0);
		}

		return ftell(*inf);
	}

    int KIPLSHARED_EXPORT WriteMAThead(FILE **of,
		const size_t *dims, 
		size_t NDim, 
		mxClassID type, 
		const char *varname, 
		const char *fname)
	{
	char infotext[124];
		char dummy[]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
		size_t i;
		
		unsigned int ndata=(unsigned int) dims[0];
		for (i=1; i<NDim; i++) ndata*=(unsigned int) dims[i];

		strcpy(infotext,"File generated by WriteMATmatrix, PC");
		for (i=strlen(infotext); i<124; i++) infotext[i]=' ';

		//FILE *of;
        if ((*of=fopen(fname,"wb"))==nullptr) {
			cerr<<"WriteMATmatrix: File opening error"<<endl; 
			cerr<<"x "<<fname<<endl;
			exit(0);
		}

		// Info header
		fwrite(infotext, sizeof(char), 124, *of);
		// Version
		unsigned short stmp;
		stmp=0x0100;
		fwrite(&stmp,sizeof(unsigned short), 1, *of);

		// Endian indicator
		char ei[]="IM";
		fwrite(ei,sizeof(char),2,*of);

		// ********** Matrix
		// Set data type
		int datatype=(int)mi_MATRIX; // Matlab array
		fwrite(&datatype, sizeof(int), 1, *of);

		// Set datasize
		unsigned int datasize;
		int namelen=strlen(varname);
		int nametype;
		int nm;
		if (namelen<=4) {
			nm=namelen%4;
			if (nm)
				namelen=namelen+(8-nm);
			nametype=4;
		}
		else {
			nm=namelen%8;
			if (nm)
				namelen=namelen+(8-nm);
			nametype=8;
		}

		//datasize=ndata*datatypesize[type]+40+namelen+nametype;
		datasize=ndata*datatypesize[type]+32+(NDim+(NDim%2))*4+namelen+nametype;
		
		
		unsigned int m=datasize%8;
		if (m)
			datasize=datasize+(8-m);
		
		fwrite(&datasize,sizeof(unsigned int),1,*of);
	//	long hpos=ftell(*of);
		// ********** Flags
		// set length for flags
		datatype=(int)mi_UINT32;
		fwrite(&datatype,sizeof(int),1,*of);
		datasize=8;
		fwrite(&datasize,sizeof(unsigned int),1,*of);
		// 2 undefined bytes
		//fwrite(dummy,1,2,of);

		// set the flags
        char flags[]={static_cast<char>(type),0,0,0}; // real values, nonglobal, nonlogical, single precision
		int *iflag;
		iflag=(int *) flags;
		fwrite(iflag,sizeof(int),1,*of);


		// 4 undefined bytes
		fwrite(dummy,sizeof(char),4,*of);

		// ********** Dimension array
		// set length for dim array
		datatype=mi_INT32;
		//datatype=mi_UINT32;
		fwrite(&datatype,sizeof(int),1,*of);
		datasize=4*NDim;
		fwrite(&datasize,sizeof(unsigned int),1,*of);

		// write the sizes
		for (i=0; i<(NDim+(NDim%2)); i++) 
			fwrite(&dims[i], sizeof(int),1,*of);


		// ********** Variable name
		// set length for name array

		short sdatatype=1;
		short sdatasize=strlen(varname);
		switch(nametype) {
		case 8:
			datatype=(int)mi_INT8;
			datasize=strlen(varname);

			fwrite(&datatype,sizeof(int),1,*of);
			fwrite(&datasize,sizeof(unsigned int),1,*of);
			break;
		case 4:
			sdatatype=(short)mi_INT8;
			sdatasize=strlen(varname);
			datasize=sdatasize;
			fwrite(&sdatatype,sizeof(short),1,*of);
			fwrite(&sdatasize,sizeof(short),1,*of);
			break;
		}


		// write the name
		fwrite(varname,1,datasize,*of);
		if (nm)
			fwrite(dummy,1,nametype-nm,*of);

		// ********** Write data
		// set length for data array
		datatype=(int)mxmitype[type];
		fwrite(&datatype,sizeof(int),1,*of);
		datasize=ndata*datatypesize[type];
		
		fwrite(&datasize,sizeof(unsigned int),1,*of);


		//cout<<"done closing"<<endl;
		return ftell(*of);
	}

    int KIPLSHARED_EXPORT FinishWriteMAT(FILE **of, int headsize)
	{
		char dummy[]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

		long fpos=ftell(*of);

		int m=(fpos-headsize)%8;

		if (m)
			fwrite(dummy,1,8-m,*of);

		fclose(*of);
		
		return 0;
	}

	std::string ClassIDstr(mxClassID id)
	{
		switch (id) {
	case mxUNKNOWN_CLASS:
		return "Unknown";
		break;
	case mxCELL_CLASS:
		return "Cell";
		break;
	case mxSTRUCT_CLASS:
		return "Struct";
		break;
	case mxLOGICAL_CLASS:
		return "Logical";
		break;
	case mxCHAR_CLASS:
		return "char";
		break;
	case mxSPARSE_CLASS:		// OBSOLETE! DO NOT USE 
		return "Sparse";
		break;
	case mxDOUBLE_CLASS:
		return "double";
		break;
	case mxSINGLE_CLASS:
		return "float";
		break;
	case mxINT8_CLASS:
		return "char";
		break;
	case mxUINT8_CLASS:
		return "unsigned char";
		break;
	case mxINT16_CLASS:
		return "short";
		break;
	case mxUINT16_CLASS:
		return "unsigned short";
		break;
	case mxINT32_CLASS:
		return "int";
		break;
	case mxUINT32_CLASS:
		return "unsigned int";
		break;
	case mxINT64_CLASS:		// place holder - future enhancements 
		return "int64";
		break;
	case mxUINT64_CLASS:		// place holder - future enhancements 
		return "unsigned int64";
		break;
	case mxFUNCTION_CLASS:
		return "Function";
		break;
	case mxOPAQUE_CLASS:
		return "Opaque";
		break;
	case mxOBJECT_CLASS:
		return "Object";
		break;
		}
		return "Unsupported data type";
	}
	
	std::ostream & operator<<(std::ostream & s, mxClassID id)
	{
		switch (id) {
			case mxUNKNOWN_CLASS : s<<"Unknown";        break;
			case mxCELL_CLASS    : s<<"Cell";     		break;
			case mxSTRUCT_CLASS  : s<<"Struct";   		break;
			case mxLOGICAL_CLASS : s<<"Logical";  		break;
			case mxCHAR_CLASS    : s<<"char";	  		break;
			case mxSPARSE_CLASS  : s<<"Sparse";   		break;
			case mxDOUBLE_CLASS  : s<<"double";   		break;
			case mxSINGLE_CLASS  : s<<"float";    		break;
			case mxINT8_CLASS    : s<<"char";     		break;
			case mxUINT8_CLASS   : s<<"unsigned char";  break;
			case mxINT16_CLASS   : s<<"short";          break;
			case mxUINT16_CLASS  : s<<"unsigned short"; break;
			case mxINT32_CLASS   : s<<"int";            break;
			case mxUINT32_CLASS  : s<<"unsigned int";   break;
			case mxINT64_CLASS   : s<<"int64";          break;
			case mxUINT64_CLASS  : s<<"unsigned int64"; break;
			case mxFUNCTION_CLASS: s<<"Function";       break;
			case mxOPAQUE_CLASS  : s<<"Opaque";         break;
			case mxOBJECT_CLASS  : s<<"Object";         break;
		}
		
		return s;
	}

}}} // End namespace fileio
