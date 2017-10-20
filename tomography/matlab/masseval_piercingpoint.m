%% Mass evaluation of piercing points

fmask='';   % filemask with full path to the images
fidx=1:100;
dc=fitsread('dc_00001.fits'); % adjust the path of the dc image...

for i=fidx
    fname=sprintf('%s_%05d.fits',fmask,i);
    img=fitsread(fname);
    img=img-dc; img(img<1)=1;
    
    [x(i),y(i)]=findpp(img); 
end

plot([x y]);

