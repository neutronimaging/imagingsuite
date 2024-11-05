

#ifndef QUEUEFILTERMPIQUEUEFILTERMANAGER_H
#define QUEUEFILTERMPIQUEUEFILTERMANAGER_H

#include <queuefilter/queuefiltermanager.h>
#include <queuefilter/basequeueworker.h>
#include <queuefilter/mpitimer.h>
#include <limits>
#include <io/io_matlab.h>
#ifdef USE_MPI
#include <mpi.h>
#endif
using namespace std;

namespace QueueFilter {
#ifdef USE_MPI
	// System tags
	const int TAG_speed=1000;		// Transmit worker cpu speed
	const int TAG_idtime=1010;		// Transmit worker cpu speed
	
	// Image tags
	const int TAG_exec_info=2000;		// Transmit sub image dims
	const int TAG_dims=2010;		// Transmit sub image dims
	const int TAG_image=2100;		// Transmit input image
	const int TAG_image_top=2110;	// Transmit top edge data
	const int TAG_image_bottom=2120; // Transmit bottom edge data
	// Filter tags
	const int TAG_filter_length=3000; // Request and receive length of filter length
	const int TAG_filter_edge=3010;	 // Request and receive length of filter edge
	const int TAG_filter_iterations=3100; // Transmit number of iterations
	const int TAG_filter_start=3200; // Broadcast message to start the processing
	
/**
This is a manager that is used by a worker process

@author Anders Kaestner
*/
template <class ImgType>
class MPIQueueFilterManager : public QueueFilter::QueueFilterManager<ImgType>
{
public:
	
    MPIQueueFilterManager(BaseQueueWorker<ImgType> & w, ostream &os=cout);

    ~MPIQueueFilterManager();

	int operator()(CImage<ImgType,3> &img, int Niterations=1);
	int operator()();

private : 
	virtual int ExchangeBoundary(CImage<float,3> &img,BaseQueueWorker<ImgType> & w);
//	virtual int ProcSingle(CImage<ImgType,3> &img, int Niterations=1);
//	virtual int ProcFactory(CImage<ImgType,3> &img);
	
	int GetProcessorSpeed();
	int PrintProcInfo();
	int MPIrank;
	int MPIsize;
	vector<int> CPUspeed;
	int CPUsum;
	int NKernel;
	MPI_Status status;
	int exec_info[10];
	vector<int> worker_heights;

	mpitimer timer_comm;
	mpitimer timer_proc;
	mpitimer timer_total;
};

template <class ImgType>
MPIQueueFilterManager<ImgType>::MPIQueueFilterManager(BaseQueueWorker<ImgType> & w, ostream & os)
 : QueueFilter::QueueFilterManager<ImgType>(w, os)
{
	timer_total.tic();
	MPI_Comm_rank(MPI_COMM_WORLD,&MPIrank);
	MPI_Comm_size(MPI_COMM_WORLD,&MPIsize);
	if (MPIrank) {
		this->logstream<<"Worker "<<MPIrank<<": Q filt manager started"<<endl;
		CPUspeed.push_back(GetProcessorSpeed());
		timer_comm.tic();
		MPI_Send(&CPUspeed.front(),1, MPI_INT,0,TAG_speed,MPI_COMM_WORLD);
		MPI_Recv(&this->id_time,1,MPI_LONG,0,TAG_idtime,MPI_COMM_WORLD,&status);
		timer_comm.toc();
		NKernel=w.kernelsize();
		
		this->mirror_top=false;
		this->mirror_bottom=false;
		
		if (MPIrank==1) {
			timer_comm.tic();
			MPI_Send(&NKernel,1, MPI_INT,0,TAG_filter_length,MPI_COMM_WORLD);
			timer_comm.toc();
			this->mirror_top=true;
		}
		 if (MPIrank==MPIsize-1) 
			this->mirror_bottom=true;
			
		this->worker.mirror_top=false;
		this->worker.mirror_bottom=false;
		this->logstream<<"Worker "<<MPIrank<<": Q filt manager initialized"<<endl;
	} 
	else {
		this->logstream<<"Manager "<<MPIrank<<": Q filt worker started"<<endl;
		CPUspeed.resize(MPIsize);
		CPUsum=0;
		int i;
		this->id_time=long(::MPI_Wtime());
		for (i=1; i<MPIsize; i++)  {
			timer_comm.tic();
			MPI_Recv(&CPUspeed[i],1,MPI_INT,i,TAG_speed,MPI_COMM_WORLD,&status);
			MPI_Send(&this->id_time,1,MPI_LONG,i,TAG_idtime,MPI_COMM_WORLD);
			timer_comm.toc();
			CPUsum+=CPUspeed[i];	
		}
	}

}


template <class ImgType>
MPIQueueFilterManager<ImgType>::~MPIQueueFilterManager()
{
	timer_total.toc();
	
	PrintProcInfo();
	
	this->logstream<<"Total processing time: "<<timer_total()<<"s"<<endl;
	this->logstream<<"Communication time: "<<timer_comm()<<"s"<<endl;
	this->logstream<<"Computation time: "<<timer_proc()<<"s"<<endl;
	this->logstream<<"Computation/Total time: "<<100-100*timer_comm()/timer_proc()<<"%"<<endl;
	
	
}

template <class ImgType>
int MPIQueueFilterManager<ImgType>::PrintProcInfo()
{
	char CPUname[MPI_MAX_PROCESSOR_NAME];
	int len;
	MPI_Get_processor_name(CPUname,&len);
	
	this->logstream<<"Legend: "<<"id_time, CPUname, ProcessorSpeed, MPIsize, MPIrank, timer_total,timer_proc,  timer_comm"<<endl;
		
	this->logstream<<"ProcInfo: "<<this->id_time<<" "
		<<CPUname<<" "
		<<GetProcessorSpeed()<<" "
		<<MPIsize<<" "
		<<MPIrank<<" "
		<<timer_total()<<" "
		<<timer_proc()<<" "
		<<timer_comm()<<endl;
		

	return 1;
}



template <class ImgType>
int MPIQueueFilterManager<ImgType>::operator()(CImage<ImgType,3> &img, int Niterations)
{
	if (MPIrank) {
		cerr<<"MPIQueueFilterManager::operator()(img,Nit) can only be used with the zero rank process"<<endl; 
		exit(1000);
	}
	timer_proc.tic();
	const unsigned int *dims=img.getDimsptr();

	worker_heights.clear();
	timer_comm.tic();
	MPI_Recv(&NKernel,1,MPI_INT,1,TAG_filter_length,MPI_COMM_WORLD,&status);
	timer_comm.toc();

	
	exec_info[0]=dims[0]; exec_info[1]=dims[1]; exec_info[2]=dims[2]/(MPIsize-1);
	exec_info[3]=Niterations;
	int i,j;
	int rest=dims[2]%(MPIsize-1);
	int exec_dimZ=exec_info[2];
	
	this->logstream<<"Distributing dim information..."<<flush;
	for (i=1 ; i<MPIsize; i++) {
		exec_info[2]=exec_dimZ+((rest--)>0);
		timer_comm.tic();
		MPI_Send(&exec_info,4, MPI_INT,i,TAG_exec_info,MPI_COMM_WORLD);
		timer_comm.toc();
		worker_heights.push_back(exec_info[2]);
	}
	this->logstream<<" done"<<endl;
	int sxy=dims[0]*dims[1];
	int layer=0;
	long datasize, datapos=0L;
	int maxint=numeric_limits<int>::max()/8;
	for (i=0; i<MPIsize-1; i++) {
		timer_comm.tic();
		datasize=long(sxy)*worker_heights[i];
		this->logstream<<"Start sending "<<i<<" "<<maxint<<endl;
		// Split data if more than maxint
		for (j=0, datapos=0L; j<(datasize/maxint) ; j++, datapos+=maxint)
			MPI_Send(img.getLineptr(0,layer)+datapos, 
				maxint, 
				MPI_FLOAT,
				i+1,
				TAG_image,
				MPI_COMM_WORLD);
	
		int rest=datasize % maxint;
		this->logstream<<"sending rest "<<rest<<" "<<datapos<<endl;	
		if (rest)
			MPI_Send(img.getLineptr(0,layer)+datapos, 
					rest, 
					MPI_FLOAT,
					i+1,
					TAG_image,
					MPI_COMM_WORLD);
		this->logstream<<"done sending"<<endl;
		timer_comm.toc();
		layer+=worker_heights[i];
	}

	MPI_Barrier(MPI_COMM_WORLD);
	
	layer=0;
	for (i=0; i<MPIsize-1; i++) {
		timer_comm.tic();

		datasize=long(sxy)*worker_heights[i];
		for (j=0, datapos=0L; j<(datasize/maxint); j++, datapos+=maxint)
			MPI_Recv(img.getLineptr(0,layer)+datapos,
				maxint,
				MPI_FLOAT,
				i+1,
				TAG_image,
				MPI_COMM_WORLD,
				&status);
		
		int rest=datasize % maxint;
		MPI_Recv(img.getLineptr(0,layer)+datapos,
			rest,
			MPI_FLOAT,
			i+1,
			TAG_image,
			MPI_COMM_WORLD,
			&status);
		timer_comm.toc();
		layer+=worker_heights[i];
	}
	timer_proc.toc();
	return 1;
}

template <class ImgType>
int MPIQueueFilterManager<ImgType>::operator()()
{
	if (!MPIrank) {
		this->logstream<<"MPIQueueFilterManager::operator()() can only be used with a non-zero rank process"<<endl; 
		exit(1000);
	}
	int i;
	timer_proc.tic();
	this->logstream<<"w("<<MPIrank<<") waiting for dimensions"<<endl;
	sleep(240);
	MPI_Recv(&exec_info,4,MPI_INT,0,TAG_exec_info,MPI_COMM_WORLD,&status);
	timer_comm.toc();
	
	this->logstream<<"w("<<MPIrank<<") dims="<<exec_info[0]<<", "<<exec_info[1]<<", "<<exec_info[2]<<endl;
	unsigned int dims[3]={exec_info[0],exec_info[1],exec_info[2]};
	CImage<float,3> img(dims);
	int sxy=exec_info[0]*exec_info[1];
	
	// Receive image data
	int intmax=numeric_limits<int>::max()/8;	
	timer_comm.tic();
	long datapos=0L;
	long datasize=long(sxy)*exec_info[2]; 
	timer_comm.tic();
	this->logstream<<"start recieve"<<endl;
	for (i=0, datapos=0L; i<datasize/intmax; i++, datapos+=intmax)
		MPI_Recv(img.getDataptr()+datapos,
			intmax,
			MPI_FLOAT,
			0,
			TAG_image,
			MPI_COMM_WORLD,
			&status);

	this->logstream<<"main receive done "<<datapos<<endl;
	int rest=datasize%intmax;
	
	MPI_Recv(img.getDataptr()+datapos,
		rest,
		MPI_FLOAT,
		0,
		TAG_image,
		MPI_COMM_WORLD,
		&status);
	timer_comm.toc();
	
	this->logstream<<"w("<<MPIrank<<") image data received"<<endl;

	MPI_Barrier(MPI_COMM_WORLD);
	this->logstream<<"w("<<MPIrank<<") starting processing"<<endl;
	
	
	// Process the image 
	if (this->worker.isFactory())
		this->ProcFactory(img);
	else
		this->ProcSingle(img,exec_info[3],this->worker);

	this->logstream<<"w("<<MPIrank<<") returning data"<<endl;
	// Return the result
	timer_comm.tic();

	for (i=0, datapos=0L; i<datasize/intmax ; i++,datapos+=intmax)
		MPI_Send(img.getDataptr()+datapos,
			intmax,
			MPI_FLOAT,
			0,
			TAG_image,
			MPI_COMM_WORLD);

	rest=datasize % intmax;

	MPI_Send(img.getDataptr(),rest,MPI_FLOAT,0,TAG_image,MPI_COMM_WORLD);
	timer_comm.toc();
	timer_proc.toc();
	this->logstream<<"w("<<MPIrank<<") returning data finished"<<endl;
	return 0;
}

template <class ImgType>
int MPIQueueFilterManager<ImgType>::ExchangeBoundary(CImage<float,3> &img, BaseQueueWorker<ImgType> & w)
{
	MPI_Status status[8];
	MPI_Request request[8];
	int request_count=0;
	
	const int nk2=w.kernelsize()/2;
	unsigned int block_dims[3]={img.SizeX(), img.SizeY(), nk2};
	CImage<float,3> Rblock_top(block_dims), Rblock_bottom(block_dims);
	CImage<float,3> Sblock_top, Sblock_bottom;
	
	if (!this->quiet)
		this->logstream<<"w("<<MPIrank<<") corresponding "<<endl;
	timer_comm.tic();
	if (!this->mirror_top) {
		if (!this->quiet)
			this->logstream<<"w("<<MPIrank<<") top ("<<this->mirror_top<<")"<<endl;
		int top_block[6]={0,0,0,img.SizeX(),img.SizeY(),nk2};
		Sblock_top=img.SubImage(top_block);
		
		MPI_Isend(Sblock_top.getDataptr(),
					Sblock_top.N(),
					MPI_FLOAT,
					MPIrank-1,
					TAG_image_top,
					MPI_COMM_WORLD,
					&request[request_count++]);

		MPI_Irecv(Rblock_top.getDataptr(),
					Rblock_top.N(), 
					MPI_FLOAT,
					MPIrank-1,
					TAG_image_bottom,
					MPI_COMM_WORLD, 
					&request[request_count++]);
	}
		
	if (!this->mirror_bottom) {
		if (!this->quiet)
			this->logstream<<"w("<<MPIrank<<") bottom ("<<this->mirror_bottom<<")"<<endl;
		int bottom_block[6]={0,0,img.SizeZ()-nk2,img.SizeX(),img.SizeY(),img.SizeZ()};
		Sblock_bottom=img.SubImage(bottom_block);
		MPI_Isend(Sblock_bottom.getDataptr(),
					Sblock_bottom.N(), 
					MPI_FLOAT,
					MPIrank+1,
					TAG_image_bottom,
					MPI_COMM_WORLD, 
					&request[request_count++]);
	
		MPI_Irecv(Rblock_bottom.getDataptr(),
					Rblock_bottom.N(), 
					MPI_FLOAT,
					MPIrank+1,
					TAG_image_top,
					MPI_COMM_WORLD, 
					&request[request_count++]);
	}
	if (!this->quiet)
		this->logstream<<"w("<<MPIrank<<") waiting for "<<request_count<<" requests"<<endl;
	MPI_Waitall(request_count,request,status);	
	timer_comm.toc();
	if (!this->quiet)
		this->logstream<<"w("<<MPIrank<<") correspondance done"<<endl;

	// Stuff the end queues
	CImage<float,2> slice;
		
	const unsigned int *dims=img.getDimsptr();
	
	this->top_boundary.clear();
	this->bottom_boundary.clear();	
	int i;
	
	for (i=nk2; i>0 ; i--) {
		if (this->mirror_bottom) // Bottom boundary
			img.ExtractSlice(slice,plane_XY,img.SizeZ()-i-1);
		else
			Rblock_bottom.ExtractSlice(slice,plane_XY,i-1);
			//Rblock_bottom.ExtractSlice(slice,plane_XY,nk2-i);
			
		this->bottom_boundary.push_back(slice);
	}
		
	for (i=1; i<=nk2 ; i++) {
		if (this->mirror_top)  // Top boundary
			img.ExtractSlice(slice,plane_XY,i);
		else 
			Rblock_top.ExtractSlice(slice,plane_XY,nk2-i);
			//Rblock_top.ExtractSlice(slice,plane_XY,nk2-i);
			
		this->top_boundary.push_back(slice);
	}	
	
	if (this->mirror_top && this->mirror_bottom) return 0;

	return 1;
}

template <class ImgType>
int MPIQueueFilterManager<ImgType>::GetProcessorSpeed()
{
	ifstream cpufile("/proc/cpuinfo");
	
	string s;
	do 
		cpufile>>s;
	while ((s!="MHz") && !cpufile.eof());
	float val;
	cpufile>>s>>val;
	
	return int(0.5+val/100);
}

#endif
}

#endif
