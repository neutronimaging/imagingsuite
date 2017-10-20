;+
; NAME:
;   Laplacian
;
; PURPOSE:
;
; EXPLANATION:
;
; CALLING SEQUENCE:
;
; INPUTS:
;
; OUTPUTS:
;
; METHOD:

; HISTORY:
;-
function laplacian, A, alfa
    sz=size(A)
    if(alfa le 0 or alfa gt 1 or sz(0) ne 2) then return, A

    L=fltarr(3,3)
    L[0,0]=alfa/4.0
    L[0,1]=(1-alfa)/4.0
    L[0,2]=alfa/4.0
    L[1,0]=(1-alfa)/4.0
    L[1,1]=-1.0
    L[1,2]=(1-alfa)/4.0
    L[2,0]=alfa/4.0
    L[2,1]=(1-alfa)/4.0
    L[2,2]=alfa/4.0
    L=-L*4/(1+alfa)
    ;print,total(L)
    return, convol(float(A),L)
end