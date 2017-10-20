function img=drawray(N,phi,t)

img(N,N)=0;
R=floor(N/2);

[x0,y0]=rays_entries(R,phi,t);
x0=x0+R;
y0=y0+R;

phi_r=pi*phi/180.0;

dx=cos(phi_r);
dy=sin(phi_r);

x=x0(1);
y=y0(1);

%img(floor(x0(1)),floor(y0(1)))=1;
%img(floor(x0(2)),floor(y0(2)))=1;

while (x0(2)<x)
   img(floor(x)+1,floor(y)+1)=1;
   x=x-dx;
   y=y-dy;
end