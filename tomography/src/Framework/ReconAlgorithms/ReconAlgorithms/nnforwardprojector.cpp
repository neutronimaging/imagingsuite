#include <iostream>
#include <math/mathconstants.h>
#include <math/numfunc.h>

#include "reconalgorithmexception.h"
#include "nnforwardprojector.h"

NNForwardProjector::NNForwardProjector() :
    ForwardProjectorBase("NNForwardProjector")
{

}

NNForwardProjector::~NNForwardProjector()
{

}

//int NNForwardProjector::project(kipl::base::TImage<float,2> &slice, std::list<float> & angles, kipl::base::TImage<float,2> &proj)
//{
//    size_t dims[2]={max(slice.Size(0),slice.Size(1)), angles.size()};
//    proj.Resize(dims);

//    buildMask(slice.Dims());

//    int proj_line=0;

//    for (auto it=angles.begin(); it!=angles.end(); it++, proj_line++) {
//        // Angles
//        float theta = *it;           // Acquisition angle;
//        float theta_rad = theta*fPi/180.0f;

//        float *pProj=proj.GetLinePtr(proj_line);
//        memset(pProj,0,sizeof(float)*proj.Size(0));
//        if ((static_cast<int>((theta+45.0f)/90.0f) & 1) == 1) { // Check if rows or cols are preferred

//            const float dx=(1.0f/sin(theta_rad));    // Row position increment;

//            for (int row = 0; row < static_cast<int>(slice.Size(1)); row++) { // Iterate over the rows in the slice
//                float *pRow=slice.GetLinePtr(row);
//                float h=row-m_cy;
//                float width=sqrt(m_cy*m_cy-h*h);   // Half width

//                //float pc=m_cy*(sin(theta_rad)-cos(theta_rad))+m_cy + cos(theta_rad)*h;
//                float pc=m_cy*(sin(theta_rad)-cos(theta_rad)) - cos(theta_rad)*h;

//                float dp=width*sin(theta_rad);

//                int p0=pc-dp;
//                int p1=pc+dp;

////                if (p1<p0)
////                    swap(p0,p1);

////                const float m=m_cx - kipl::math::sgn(dx)*width;               // Position on row in slice
//                const float m=m_cx - width;               // Position on row in slice
//                for (int p=p0, i=0; p<=p1; p++,i++) {        // this is where more fancy weighting schemes are expected
//                        int rowpos=static_cast<int>(floor(m+i*dx));
//                        if ((0<=rowpos) && (rowpos<dims[1]))
//                            pProj[p]+=pRow[rowpos];
//                }
//            }
//        }
//        else {

//        }
//    }

//    return 0;
//}

int NNForwardProjector::project(kipl::base::TImage<float,2> &slice, std::list<float> & angles, kipl::base::TImage<float,2> &proj)
{
    int t,r,m,n,mmin,mmax,nmin,nmax;
    int T,R,M,N;
    float sum,x_min,rhooffset,Delta_theta,Delta_rho,costheta,sintheta;
    float rho_min,theta,alpha,beta,nfloat,mfloat,rho;
    float Delta_x,betap,eps;
    double dXsinT, dXdsinT, dXcosT, dXdcosT;

    size_t dims[2]={slice.Size(0), angles.size()};
    proj.Resize(dims);
    proj=0.0f;
    eps=1e-4;
    T=proj.Size(1);
    R=proj.Size(0);
    M=N=slice.Size(0);

    rho_min=-R/2;

    Delta_rho=1;

    x_min=-M/2;
    Delta_x=1;
    auto angle=angles.begin();

    for(t=0; angle!=angles.end() ;t++, angle++)
    {
        theta=*angle;
        theta *= fPi/180.0f;
        sintheta=sin(theta);
        costheta=cos(theta);
        rhooffset=x_min*(costheta+sintheta);
        if (sintheta>sqrt(0.5))
        {
            alpha=-costheta/sintheta;

            dXsinT=Delta_x*sintheta;
            dXdsinT=Delta_x/fabs(sintheta);

            for(r=0, rho=0; r<R; r++, rho+=Delta_rho )
            {
                beta=(rho+rho_min-rhooffset)/(dXsinT);
                betap=beta+0.5;
                sum=0.0;
                if (alpha>1e-6)
                {
                    mmin=(int)ceil(-(betap-eps)/alpha);
                    mmax=1+(int)floor((N-betap-eps)/alpha);
                }
                else { // AK
                    if (alpha<-1e-6)
                    {
                        mmin=(int)ceil((N-betap-eps)/alpha);
                        mmax=1+(int)floor(-(betap-eps)/alpha);
                    }
                    else
                    {
                        mmin=0;
                        mmax=M;
                    }
                } // AK
                if (mmin<0) mmin=0;
                if (mmin>=M) mmin=M-1;
                if (mmax>M) mmax=M;
                nfloat=beta+mmin*alpha;

                for (m=mmin;m<mmax;m++)	{
                    if ((nfloat>=0) && (nfloat<N))
                        sum+=slice(m,static_cast<int>(nfloat));
                    nfloat+=alpha;
                }
                proj(r,t)=sum*dXdsinT;

            }
        }
        else {
            alpha=-sintheta/costheta;

            dXcosT=Delta_x*costheta;
            dXdcosT=Delta_x/fabs(costheta);

            for(r=0, rho=0; r<R; r++, rho+=Delta_rho )
            {
                beta=(rho+rho_min-rhooffset)/dXcosT;
                betap=beta+0.5;
                sum=0.0;
                if (alpha>1e-6)
                {
                    nmin=(int)ceil(-(betap-eps)/alpha);
                    nmax=1+(int)floor((M-betap-eps)/alpha);
                }
                else {
                    if (alpha<-1e-6)
                    {
                        nmin=(int)ceil((M-betap-eps)/alpha);
                        nmax=1+(int)floor(-(betap-eps)/alpha);
                    }
                    else
                    {
                        nmin=0;
                        nmax=N;
                    }
                }
                if (nmin<0) nmin=0;
                if (nmin>=N) nmin=N-1;
                if (nmax>N) nmax=N;
                mfloat=beta+nmin*alpha;


                for (n=nmin;n<nmax;n++)	{
                    if ((mfloat>=0) && (mfloat<M))
                        sum+=slice(static_cast<int>(mfloat),n);
                    mfloat+=alpha;
                }
                proj(r,t)=sum*dXdcosT;

            }
        }
    }
    return 0;
}

int NNForwardProjector::project(kipl::base::TImage<float,3> &slice, std::list<float> & angles, kipl::base::TImage<float,3> &proj)
{
    throw ReconAlgorithmException("NN Forward projection for multiple slices is not implemented",__FILE__,__LINE__);

    return 0;
}
