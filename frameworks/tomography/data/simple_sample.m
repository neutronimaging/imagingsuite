cd ~/repos/tomography/trunk/data/

E = [100 0.75 0.75 0 0 0 
     10  0.1 0.1 0.5 0 0];
 
img=phantom(E,256);
 
imagesc(img);
 
imwrite(uint8(img),'simple_sample.tif');
 
E = [100 0.75 0.75 0 0 0];
    
img=phantom(E,256);
 
imagesc(img);
 
imwrite(uint8(img),'disk256.tif');


cd ~/repos/tomography/trunk/data/
%%
E = [100 0.05 0.05 -0.3 -0.5  0 
     200  0.1 0.1 0.5 0 0];
 
img=phantom(E,256);
 
imagesc(img);
 
imwrite(uint8(img),'twodisks256.tif');

