function [x,y]=rays_entries(R,phi,t)

phi_r=phi*pi/180;

alpha2=asin(sqrt(R^2-t^2)/R);

x(2)=0;
y(2)=0;

if (t==0.0)
    s=1;
else
    s=sign(t);
end

x(1)=s*R*cos(phi_r+pi*0.5-alpha2);
x(2)=s*R*cos(phi_r+pi*0.5+alpha2);

y(1)=s*R*sin(phi_r+pi*0.5-alpha2);
y(2)=s*R*sin(phi_r+pi*0.5+alpha2);