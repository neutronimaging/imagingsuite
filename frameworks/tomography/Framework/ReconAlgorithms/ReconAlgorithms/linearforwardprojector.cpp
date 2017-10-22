#include "linearforwardprojector.h"
#include <math/mathconstants.h>
#include <reconalgorithmexception.h>

LinearForwardProjector::LinearForwardProjector()
{

}

LinearForwardProjector::~LinearForwardProjector()
{

}

int LinearForwardProjector::project(kipl::base::TImage<float,2> &slice, std::list<float> & angles, kipl::base::TImage<float,2> &proj)
{

    int t,r,m,n,mmin,mmax,nmin,nmax;
    int T,R,M,N;
    float sum,x_min,rhooffset,Delta_theta,Delta_rho,costheta,sintheta,Offset_Theta;
    float rho_min,theta,alpha,beta,nfloat,nf,mfloat,mf,w,rho;
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
    Offset_Theta=0.0f;

    for(t=0; angle!=angles.end() ;t++, angle++)
    {
        theta=*angle+Offset_Theta;
        theta *= fPi/180.0f;
        sintheta=sin(theta);
        costheta=cos(theta);
        rhooffset=x_min*(costheta+sintheta);

        if (abs(sintheta)>sqrt(0.5))
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
                    nf=(int)nfloat;
                    w=nfloat-nf;
                    if ((nfloat>=0) && (nfloat<(N-1)))
                        //(1-w)*img+w*(img+1)
                        sum+=slice(m,nf)+w*(slice(m,nf+1)-slice(m,nf));
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
                    mf=(int)mfloat;
                    w=mfloat-mf;
                    if ((mfloat>=0) && (mfloat<(M-1)))
                        //(1-w)*img+w*(img+1)
                        sum+=slice(mf,n) + w*(slice(mf+1,n) - slice(mf,n));
                    mfloat+=alpha;
                }
                proj(r,t)=sum*dXdcosT;

            }
        }
    }
    return 0;
}

int LinearForwardProjector::project(kipl::base::TImage<float,3> &slice, std::list<float> & angles, kipl::base::TImage<float,3> &proj)
{
    throw ReconAlgorithmException("Linear forward projection for multiple slices is not implemented",__FILE__,__LINE__);

    return 0;
}
