function log, N, sigma
; Laplacian of Gaussian
    if N mod 2 eq 0 then N=N+1
    n2=N/2
    g=fltarr(N,N)
    log=fltarr(N,N)
    sigma=float(sigma)
    for i=-n2,n2 do $
        for j=-n2,n2 do $
           g[i+n2,j+n2]=exp(-(i^2+j^2)/(2.0*sigma^2))
    sumg=total(g)
    for i=-n2,n2 do $
        for j=-n2,n2 do $
           log[i+n2,j+n2]=(i^2+j^2-2*sigma^2)*g(i+n2,j+n2)/(2.0*3.1415926*sigma^6*sumg)
    return, log
end

pro tt
    a=log(100,7)
    window,1,xsize=800,ysize=800,title='log'
    ;surface,a
    ;show3,a
    shade_surf,a
    T = TVRD()
    ;write_bmp,'log.bmp',t
    res=DIALOG_WRITE_IMAGE(t)
    print,total(a)
end