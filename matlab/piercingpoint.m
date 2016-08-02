%%
%dc=fitsread('/data/data3/P20160248/02_rawdata/13_DistanceCalibration2/dc_00001.fits');
%ob=fitsread('/data/data3/P20160248/02_rawdata/13_DistanceCalibration2/ob_sod207_odd331_sdd538.fits');
dc=fitsread('/Users/data/P20160248/02_rawdata/13_DistanceCalibration2/dc_00001.fits');
ob=fitsread('/Users/data/P20160248/02_rawdata/13_DistanceCalibration2/ob_sod207_odd331_sdd538.fits');

ob=ob-dc;
ob(ob<1)=1;

%%

mob=medfilt2(ob,[31 1]);
[dx,dy]=gradient(mob);

%%
s=mean(dx,1);
subplot(2,2,1),
plot(s)
th=15; % Threshold to identify the steps, please adapt to data
cs=cumsum((th<abs(s)).*s);
offs=ones(size(ob,1),1)*cs;
subplot(2,2,2), 
plot(cs)

subplot(2,2,3)
imagesc(ob), axis image, caxis([6000 8000])
subplot(2,2,4)
imagesc(ob-offs), axis image, caxis([6000 8000])
cob=ob-offs;

%% COG

[x,y]=meshgrid(1:size(ob,1),1:size(ob,2));
mcob=medfilt2(cob,[5 5]);
cobx=mcob(:)'*x(:)/sum(mcob(:));
coby=mcob(:)'*y(:)/sum(mcob(:));

subplot(2,2,4)
imagesc(mcob)
hold on
plot(coby,cobx,'+r')

%% Estimate

% [1 x x2 y y2]
[x,y]=meshgrid(1:size(ob,1),1:size(ob,2));

H=[ones(numel(ob),1) x(:) x(:).^2 y(:) y(:).^2];
%%
% H x = y
% H'H x = H' y
% x= inv(H'H)H' y
HTH = inv(H'*H)*H';

est=HTH*ob(:);
%est= ob(:) \ H;