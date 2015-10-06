function f=recon(sino,phi)
f(size(sino,1),size(sino,1))=0;
sino=gradient(sino')';  % a better filter is needed...
for i=1:length(phi)
    if (i==1)
        f=imrotate(f,phi(i),'bilinear','crop'); 
    else
        f=imrotate(f,phi(i)-phi(i-1),'bilinear','crop'); 
    end
    bp=sino(:,i)*ones(1,size(sino,1));
    f=f+bp;
end