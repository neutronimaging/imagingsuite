/*
  This is a SWIG interface description for the nxdataset functions and
  some helper code.
 
  copyright: GPL

  Mark Koennecke, October 2002
*/
%module nxdataset
%{
#include "nxdataset.h"

#define MAXDIM 7

void *create_nxds(int rank, int type, int dim0, int dim1, int dim2, 
	int dim3, int dim4, int dim5,int dim6){
	int dim[MAXDIM],i;

	dim[0] = dim0;
	dim[1] = dim1;
	dim[2] = dim2;
	dim[3] = dim3;
	dim[4] = dim4;
	dim[5] = dim5;
	dim[6] = dim6;

	return createNXDataset(rank,type,dim);
}
void *create_text_nxds(char *name){
	return (void *)createTextNXDataset(name);
}

void drop_nxds(void *ptr){
	dropNXDataset( (pNXDS) ptr);
}

int get_nxds_rank(void *ptr){
	return getNXDatasetRank((pNXDS) ptr);
}

int get_nxds_type(void *ptr){
	return getNXDatasetType((pNXDS) ptr);
}

int get_nxds_dim(void *ptr, int which){
	return getNXDatasetDim((pNXDS) ptr, which);
}

double get_nxds_value(void *ptr,int dim0, int dim1, int dim2, 
	int dim3, int dim4, int dim5,int dim6){
	int64_t dim[MAXDIM];

	dim[0] = dim0;
	dim[1] = dim1;
	dim[2] = dim2;
	dim[3] = dim3;
	dim[4] = dim4;
	dim[5] = dim5;
	dim[6] = dim6;

	return getNXDatasetValue((pNXDS)ptr,dim);
}

char *get_nxds_text(void *ptr){
	return getNXDatasetText((pNXDS) ptr);
}

int  put_nxds_value(void *ptr, double value, int dim0, int dim1, int dim2, 
	int dim3, int dim4, int dim5,int dim6){
	int64_t dim[MAXDIM];

	dim[0] = dim0;
	dim[1] = dim1;
	dim[2] = dim2;
	dim[3] = dim3;
	dim[4] = dim4;
	dim[5] = dim5;
	dim[6] = dim6;

	return putNXDatasetValue((pNXDS)ptr,dim,value);
}


%}

extern void *create_nxds(int rank, int type, int dim0=0,
	                 int dim1=0,int dim2=0,int dim3=0,
                         int dim4=0, int dim5=0, int dim6=0);
extern void *create_text_nxds(char *name);

extern void drop_nxds(void *ptr);


extern int get_nxds_rank(void *ptr);

extern  int get_nxds_type(void *ptr);

extern int get_nxds_dim(void *ptr, int which);

extern double get_nxds_value(void *ptr, int dim0=0,
	                 int dim1=0,int dim2=0,int dim3=0,
                         int dim4=0, int dim5=0, int dim6=0);

extern char *get_nxds_text(void *ptr);

extern int put_nxds_value(void *ptr, double value,int dim0=0,
	                 int dim1=0,int dim2=0,int dim3=0,
                         int dim4=0, int dim5=0, int dim6=0);






