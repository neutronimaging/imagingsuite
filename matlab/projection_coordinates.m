%% 
clear all
N=32;
img=zeros(N);
m_cy=N/2;
R=m_cy;
theta=225;
theta_rad=theta*pi/180;
clear width p0 p1 pc
width=zeros(1,N);
pc=width;
p1=width;
p0=width;

for row = 1:N
    h=row-m_cy;
	width(row)=sqrt(R*R-h*h);   
    phi=acos(h/R); 


    %pc(row)=m_cy*(sin(theta_rad)-cos(theta_rad))+m_cy ...
    pc(row)=m_cy*(sin(theta_rad)-cos(theta_rad))+m_cy ...
        - cos(theta_rad)*h;
    dp=width(row)*sin(theta_rad);
    
    p0(row)=pc(row)-dp;
    p1(row)=pc(row)+dp;    

end
subplot(1,2,1)
plot([p0' p1' pc' ],1:N,'+'); axis square, axis tight

subplot(1,2,2)
plot((p1-p0)); axis square

p0e=[min(p0), max(p0)]
p1e=[min(p1), max(p1)]