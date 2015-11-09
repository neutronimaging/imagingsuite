%% 
clear all
N=32;
img=zeros(N);
m_cy=N/2;
R=m_cy;
theta=45;
theta_rad=theta*pi/180;
for row = 1:N
    h=row-m_cy;
	width(row)=sqrt(R*R-h*h);   
    phi=acos(h/R); 
    pc(row)=sign(h)*h*cos(phi)+R;
  %  pc(row)=sign(h)*h*h/m_cy+m_cy;
    
    dp=width(row)*sin(theta_rad);
    
    p0(row)=pc(row)-dp;
    p1(row)=pc(row)+dp;    
end
subplot(1,2,1)
plot([p1' p0' pc' ],1:N,'+');

subplot(1,2,2)
plot((p1-p0)/cos(theta_rad))