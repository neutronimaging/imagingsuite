function [x,y,info]=findpp(img,marg,thres)
dims=size(img);
switch nargin
    case 1 
        marg=0.8; thres=2.5;
    case 2
        thres=2.5;
end

if (marg<0) | (1<marg)
    marg=0.8;
end

info.marg=marg;
info.thres=thres;

info.offset=floor(dims*(0.5*(1-marg)));
cropped=img(info.offset(1)+(1:floor(marg*dims(1))),info.offset(2)+(1:floor(marg*dims(2))));
info.cropped=remove_gainbias(cropped,thres);
info.mob=medfilt2(cropped,[5 5]);

% Estimate shape of the open beam as $I(x,y)=a + bx + cy + dxy + ex^2 + fy^2$
[x,y]=meshgrid(1:size(info.mob,2),1:size(info.mob,1));
x=x+marg; y=y+marg;

H=[ones(numel(x),1) x(:) y(:) x(:).*y(:) x(:).^2 y(:).^2];
%e=(cob2(:))\H;
info.e=inv(H'*H)*H'*info.mob(:);
info.est=reshape(H*info.e, size(info.mob));

% Finding the coordinates of the max
% $I(x,y) = a + b x + c y + d xy + e x^2 + f y^2$
% 
% 
% $\partial I(x,y)/\partial x = b + d y + 2e * x = 0$
%
% $\partial I(x,y)/ \partial y = c + d x + 2f * y = 0$
% 
% $\left(\begin{array}{cc}2e &d\\ d & 2f\end{array}\right) \left(\begin{array}{c}x \\y\end{array}\right)=\left(\begin{array}{c}-b\\-c\end{array}\right)$
%


H=[ 2*info.e(5) info.e(4); info.e(4) 2*info.e(6)];
k=[ -info.e(2) -info.e(3)]';

info.pos=inv(H)*k;

info.global_pos=[info.pos(1)+info.offset(2),info.pos(2)+info.offset(1)];
x=info.global_pos(1);
y=info.global_pos(2);

function img=remove_gainbias(img,thres)

if (nargin<2)
    thres=2.5;
end

[dx,dy]=gradient(img);
p=mean(medfilt2(dx,[1 9]),1);
det=thres<abs(p);
corr=ones(size(img,1),1)*cumsum(det.*p);
cob=img-corr;