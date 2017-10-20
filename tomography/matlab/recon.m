function f=recon(sino,phi)
f(size(sino,1),size(sino,1))=0;
sf=reconfilter(sino);  % a better filter is needed...
for i=1:length(phi)
    if (i==1)
        f=imrotate(f,phi(i),'bilinear','crop'); 
    else
        f=imrotate(f,phi(i)-phi(i-1),'bilinear','crop'); 
    end
    bp=sf(:,i)*ones(1,size(sf,1));
    f=f+bp;
end

function g=reconfilter(f)

ff=fft(f);
sx=size(f,1);
sx2=floor(sx/2);
h=[0:(sx2-1) (sx-sx2):-1:1]';
h(1)=0.1;
h=h*ones(1,size(f,2));
ff=h.*ff;

g=abs(ifft(ff));