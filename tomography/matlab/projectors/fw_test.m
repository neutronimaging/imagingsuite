img=phantom(512);

subplot(1,2,1), imagesc(img), axis image

phi=45;

p1=radon(img,phi);
p2=fw_dist(img,phi);

subplot(1,2,2); plot(1:length(p1),p1,1:length(p2), p2)