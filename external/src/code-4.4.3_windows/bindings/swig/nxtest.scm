;--------------------------------------------------------------------------
; scheme reading of a NeXus file
;
; this version for mzscheme
;
; Mark Koennecke, October 2002
;--------------------------------------------------------------------------

(load-extension "nxscheme.so")
(load "asciidblib.scm") ; for string-split

;------------------- print attributes -------------------------------------
(define (print-attrib fd)
  (let ( (att (nx-getnextattr fd #\@)) )
    (if (< 2 (string-length att))
	(let ( (splitlist (string-split att (list #\@))) )
	  (display (list-ref splitlist 0))
	  (display "=")
	  (display (get-nxds-text (nx-getattr fd (list-ref splitlist 0)
			       (string->number (list-ref splitlist 2))
			       (string->number (list-ref splitlist 1)))))
	  (newline)
	  (print-attrib fd) ) ) ) )
;----------------------- print a 2d dataset---------------------------
(define (print2d dataset)
  (letrec ( (ydim (get-nxds-dim dataset 1))
	 (printRow (lambda (dataset yval xdim count)
	     (if (>= count xdim)
		 (newline)
		 (begin
		   (display (get-nxds-value dataset count yval))
		   (display "  ")
		   (printRow dataset yval xdim (+ count 1))
                 )) ))
	 (printWhole (lambda (dataset ydim count)
	       (let ( (xdim (get-nxds-dim dataset 0)) )
		 (if (>= count ydim)
		     (newline)
		     (begin
		       (printRow dataset count xdim 0)
		       (printWhole dataset ydim (+ count 1)) ) ) ) )) )
    (printWhole dataset ydim 0) ) )
;======================================================================
(define fd (nx-open "nxinter.hdf" (nxacc-read)))
(print-attrib fd)

(nx-opengroup fd "fish" "NXentry")
(nx-opendata fd "fish")
(let
    ( (ds (nx-getdata fd)) )
  (print2d ds)
  (drop-nxds ds) )
(nx-closedata fd)
(nx-closegroup fd)
(nx-close fd)

