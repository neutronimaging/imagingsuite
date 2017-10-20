%% Draw circle
clf
R=10;
N=100;
phi=pi*(-N:N)/N;

x=R*cos(phi);
y=R*sin(phi);

plot(x,y), axis square
hold on;

for t=-R:(R/20):R
    [x,y]=rays_entries(R,0,t); plot(x,y,x,y,'o')
end