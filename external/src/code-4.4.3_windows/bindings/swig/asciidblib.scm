;--------------------------------------------------------------------------
; A couple of utility functions for accessing ASCII data tables 
; 
; Mark Koennecke, December 2001
;--------------------------------------------------------------------------

; The separator to use for splitting lines into columns
(define separator #\tab)

;-------------------------------------------------------------------------
; the current row we are working at
(define currentRow '())

;------------------------------------------------------------------------
; functifyFields creates functions for each field which allows to 
; retrieve the field value by a (fieldName) syntax. This is useful for
; operator doing computations. The row to be accessed must be put into
; currentRow however before this can work.
(define functifyFields (lambda (header)
      (letrec ( (count 0)
             (funcOne (lambda (fieldList count)
                (if (eq? fieldList '())
                    '()
                    (begin
		      (eval (read (open-input-string (string-append 
                        "(define " (car fieldList) 
                        " (lambda () (list-ref currentRow " 
                        (number->string count) " )))" ))))
                      (funcOne (cdr fieldList) (+ count 1)) ) ) )) )
           (funcOne (vector-ref header 1) count) ) )) 
;-------------------------------------------------------------------------
; readheader reads the header information of a database and stores it into
; a vector. This vector will hold three things: A list of comment
; lines (recognizable by a # int the first position of the line), a list
; of field names and the dashline unchanged.
;-------------------------------------------------------------------------
 (define readheader (lambda (port) 
     (letrec ( (fline "")
               (getcomment (lambda (port)
		    (let ((l (getline port)))
                       (if (char=? (list-ref l 0) #\#)
                         (cons (list->string l) (getcomment port))
                         (begin
                           (set! fline (list->string l))
                           '() ) ) ) ) )
            (comments (getcomment port))
            (names (string-split fline (list separator) ))
            (dash (read-line port))
          )
          (vector comments names dash) ) ) )
;-------------------------------------------------------------------------
; putdbline writes a database row to the given output port. The row is
; specified as a list of field values.
;-------------------------------------------------------------------------
  (define putdbline (lambda (row port)
       (letrec ( (putfield (lambda (row port)
                   (display (list-ref row 0) port)
                   (cond
		     ( (eq? (cdr row) '())
		       (newline port)
                     )
                     (else
                        (display separator port)
                        (putfield (cdr row) port) ) ) ) ) )
          (if (not (eq? (cadr row) '()))
            (putfield row port)) ) ) )  
;-------------------------------------------------------------------------
; writeheader writes a database header to a output port
;-------------------------------------------------------------------------
  (define writeheader (lambda (h port)
     (let ( (putcomment (lambda (com port)
               (if (not (eq? com '()))
                (begin
                  (write (car com) port)
                  (newline port)
                  (putcomment (cdr com) port) ) ) ) ) )
          (putcomment (vector-ref h 0) port)
          (putdbline (vector-ref h 1) port)
          (display (vector-ref h 2) port)
          (newline port) ) ))
;-------------------------------------------------------------------------
;locatefield determines the index of a field in a list of row values
;-------------------------------------------------------------------------
  (define locatefield (lambda (header name)
      (letrec ( (count -1)
	      (findindex (lambda (list name)
                (cond
                  ( (eq? list '())
                    -1 )
                  ( (string=? (car list) name)
                    (+ count 1) )
                  (else
                    (set! count (+ count 1))
                    (findindex (cdr list) name) ) ) ) ) )
           (findindex (vector-ref header 1) name) ) ))
;-------------------------------------------------------------------------
; getname is a  convenience function for returning the i'th column name
;------------------------------------------------------------------------
  (define getname (lambda (header i)
       (list-ref (vector-ref header 1) i)))
;--------------------------------------------------------------------------
; getline reads a line of text and returns the line as a list of characters
;--------------------------------------------------------------------------
  (define getline (lambda (port)
		    (let ((c (read-char port)))
		      (cond  
		        ( (eof-object? c) c )
		        ( (char=? c #\newline) '() )
		       ; ( (char=? c #\cr) (cons #\space (getline port)) )
		        (else (cons c (getline port)) ) ) ) ) )
;-------------------------------------------------------------------------
; read-line reads a line as a string
;-------------------------------------------------------------------------
 (define read-line (lambda (port) (list->string (getline port))))
;============================================================================
; A string splitting procedure from the net. Originally written by oleg

; -- procedure: string-null? STRING
; returns false if the string is the empt string, true else
(define string-null? (lambda (txt)
  (if (> 0  (string-length txt))
	 #t
         #f ) ) )
;
(define ++ (lambda (x) (+ x 1)))
(define -- (lambda (x) (- x 1)))

; -- procedure+: string-split STRING CHARSET
; -- procedure+: string-split STRING CHARSET MAXSPLIT
;
; Returns a list of words delimited by the characters in CHARSET in
; STRING. CHARSET is a list of characters that are treated as delimiters.
; Leading or trailing delimeters are NOT trimmed. That is, the resulting
; list will have as many initial empty string elements as there are
; leading delimiters in STRING.
;
; If MAXSPLIT is specified and positive, the resulting list will
; contain at most MAXSPLIT elements, the last of which is the string
; remaining after (MAXSPLIT - 1) splits. If MAXSPLIT is specified and
; non-positive, the empty list is returned. "In time critical
; applications it behooves you not to split into more fields than you
; really need."
;
; This is based on the split function in Python/Perl
;
; (string-split " abc d e f  ") ==> ("abc" "d" "e" "f")
; (string-split " abc d e f  " '() 1) ==> ("abc d e f  ")
; (string-split " abc d e f  " '() 0) ==> ()
; (string-split ":abc:d:e::f:" '(#\:)) ==> ("" "abc" "d" "e" "" "f" "")
; (string-split ":" '(#\:)) ==> ("" "")
; (string-split "root:x:0:0:Lord" '(#\:) 2) ==> ("root" "x:0:0:Lord")
; (string-split "/usr/local/bin:/usr/bin:/usr/ucb/bin" '(#\:))
; ==> ("/usr/local/bin" "/usr/bin" "/usr/ucb/bin")
; (string-split "/usr/local/bin" '(#\/)) ==> ("" "usr" "local" "bin")

(define (string-split str . rest)
		; maxsplit is a positive number
  (define (split-by-whitespace str maxsplit)
    (define (skip-ws i yet-to-split-count)
      (cond
        ((>= i (string-length str)) '())
        ((char-whitespace? (string-ref str i))
          (skip-ws (++ i) yet-to-split-count))
        (else (scan-beg-word (++ i) i yet-to-split-count))))
    (define (scan-beg-word i from yet-to-split-count)
      (cond
        ((zero? yet-to-split-count)
          (cons (substring str from (string-length str)) '()))
        (else (scan-word i from yet-to-split-count))))
    (define (scan-word i from yet-to-split-count)
      (cond
        ((>= i (string-length str))
          (cons (substring str from i) '()))
        ((char-whitespace? (string-ref str i))
          (cons (substring str from i) 
            (skip-ws (++ i) (-- yet-to-split-count))))
        (else (scan-word (++ i) from yet-to-split-count))))
    (skip-ws 0 (-- maxsplit)))

		; maxsplit is a positive number
		; str is not empty
  (define (split-by-charset str delimeters maxsplit)
    (define (scan-beg-word from yet-to-split-count)
      (cond
        ((>= from (string-length str)) '(""))
        ((zero? yet-to-split-count)
          (cons (substring str from (string-length str)) '()))
        (else (scan-word from from yet-to-split-count))))
    (define (scan-word i from yet-to-split-count)
      (cond
        ((>= i (string-length str))
          (cons (substring str from i) '()))
        ((memq (string-ref str i) delimeters)
          (cons (substring str from i) 
            (scan-beg-word (++ i) (-- yet-to-split-count))))
        (else (scan-word (++ i) from yet-to-split-count))))
    (scan-beg-word 0 (-- maxsplit)))

			; resolver of overloading...
			; if omitted, maxsplit defaults to
			; (++ (string-length str))
  (if (string-null? str) '()
    (if (null? rest) 
      (split-by-whitespace str (++ (string-length str)))
      (let ((charset (car rest))
          (maxsplit
            (if (pair? (cdr rest)) (cadr rest) (++ (string-length str)))))
        (cond 
          ((not (positive? maxsplit)) '())
          ((null? charset) (split-by-whitespace str maxsplit))
          (else (split-by-charset str charset maxsplit))))))
)

