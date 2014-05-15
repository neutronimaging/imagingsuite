#ifndef MATLABIO_H_
#define MATLABIO_H_

#ifdef _MSC_VER
#define STDCALL __stdcall
#else
#define STDCALL
#endif

#include <cstdlib>
#include <iostream>
namespace kipl { namespace io { namespace core { 
/// Enum used to select data type to save in mat file

typedef enum {
	mxUNKNOWN_CLASS = 0,
	mxCELL_CLASS,
	mxSTRUCT_CLASS,
	mxLOGICAL_CLASS,
	mxCHAR_CLASS,
	mxSPARSE_CLASS,		// OBSOLETE! DO NOT USE 
	mxDOUBLE_CLASS,
	mxSINGLE_CLASS,
	mxINT8_CLASS,
	mxUINT8_CLASS,
	mxINT16_CLASS,
	mxUINT16_CLASS,
	mxINT32_CLASS,
	mxUINT32_CLASS,
	mxINT64_CLASS,		// place holder - future enhancements 
	mxUINT64_CLASS,		// place holder - future enhancements 
	mxFUNCTION_CLASS,
    mxOPAQUE_CLASS,
	mxOBJECT_CLASS
} mxClassID;

std::ostream & operator<<(std::ostream & s, mxClassID id);
size_t sizeofMAT(mxClassID id);

//#define __STDCALL
/** \brief Reads the first matrix of a mat file (raw read)
	Both compressed and raw data can be read
	\param data pointer to the data array
	\param dims Array containing the lengts of the supported dimensions
	\param NDim Number of dimensions
	\param type Data type of the matrix
	\param name The Matlab name of the variable
	\param fname Filename of the file to be read

  \note The function allocates memory for both data and name. This memory has to be deallocated 
  manually by delete [].
*/
int ReadMATmatrix(char **data, size_t *dims, int &NDim, mxClassID  & type, char  **varname, const char *fname);
//int __STDCALL ReadMATmatrix(char **data, int & rows, int & cols, mxClassID & type, char **name, const char *fname);

/** \brief Writes mat file with the data (raw version)
	\param data pointer to the data array
	\param dims Array containing the lengts of the supported dimensions
	\param NDim Number of dimensions
	\param type Data type of the matrix
	\param varname The Matlab name of the variable
	\param fname Filename of the file to be written

*/
int WriteMATmatrix(void *data,const size_t * dims, size_t NDim, mxClassID type, const char *varname, const char *fname);
//int __STDCALL WriteMATmatrix(void *data, int rows, int cols, mxClassID type, const char *varname, const char *fname);

/// Reads the file head of a mat file
int  ReadMAThead(FILE **inf, size_t & rows, size_t & cols, mxClassID  & type, char  **varname, const char *fname);

/// Writes the file head of a mat file
int STDCALL WriteMAThead(FILE **of, 
				 const size_t *dims, 
				 size_t NDim, 
				 mxClassID type, 
				 const char *varname, 
				 const char *fname);

/// Finishes of the writing of a mat file
int STDCALL FinishWriteMAT(FILE **of, int headsize=0x88);

/** \brief Get the text name of of MAT class id
	\param id Class ID
	\param str String containing the class description
*/
std::string ClassIDstr(mxClassID id);

mxClassID GetMatlabClassID( float a );
mxClassID GetMatlabClassID( double a );
mxClassID GetMatlabClassID( char a );
mxClassID GetMatlabClassID( unsigned char a );
mxClassID GetMatlabClassID( short a );
mxClassID GetMatlabClassID( unsigned short a );
mxClassID GetMatlabClassID( int a );
mxClassID GetMatlabClassID( unsigned int a );
//mxClassID GetMatlabClassID( long long a );
//mxClassID GetMatlabClassID( unsigned long long a );

}}} // end namespace fileio
#endif /*MATLABIO_H_*/
