function p=fw_dist(img,center,phi)

p(ceil(size(img,1)*sqrt(2)+4),1)=0;

dt=cos(phi);

nstart=-1;
nstop=1;