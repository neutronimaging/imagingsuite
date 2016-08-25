%%
%dc=fitsread('/data/data3/P20160248/02_rawdata/13_DistanceCalibration2/dc_00001.fits');
%ob=fitsread('/data/data3/P20160248/02_rawdata/13_DistanceCalibration2/ob_sod207_odd331_sdd538.fits');
dc=fitsread('/Users/data/P20160248/02_rawdata/13_DistanceCalibration2/dc_00001.fits');
ob=fitsread('/Users/data/P20160248/02_rawdata/13_DistanceCalibration2/ob_sod207_odd331_sdd538.fits');

ob=ob-dc;
ob(ob<1)=1;

%%

[x,y,ppinfo]=findpp(ob);

%%
imagesc(ob), hold on, plot(x,y,'r+'), hold off
caxis([4000 10000])