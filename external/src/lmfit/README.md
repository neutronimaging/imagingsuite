# lmfit

This is the home page of **lmfit**,
a self-contained C library for Levenberg-Marquardt
least-squares minimization and curve fitting.


# Background information

## History

The core algorithm of lmfit has been invented by K Levenberg (1944) and D W Marquardt (1963).
The seminal FORTRAN implementation of MINPACK
is due to J M Moré, B S Garbow, and K E Hillstrom (1980).
An automatic translation into C was first published by S Moshier.
The present library lmfit started from a similar automatic translation.
With time, every single line was hand edited to convert the code into genuine, readable C.
The API was modified; examples, man pages, and build scripts were added.
Corrections and refinements were contributed by users (see CHANGELOG).

## Mathematics

For a gentle introduction to the Levenberg-Marquardt algorithm,
see e.g. K Madsen, H B Nielsen, O Tingleff (2004):
*Methods for non-linear least squares problems.*

## License

[FreeBSD License](http://opensource.org/licenses/BSD-2-Clause).

## Citation

When using lmfit as a tool in scientific work, no acknowledgement is needed:
If fit results are robust, it does not matter by which software they have been obtained.
If results are not robust, they should not be published anyway.
In methodological publications, e g when describing software that interacts with lmfit,
the preferred form of citation is:

Joachim Wuttke: lmfit – a C library for Levenberg-Marquardt least-squares minimization and curve fitting. Version […], retrieved on […] from https://jugit.fz-juelich.de/mlz/lmfit.

## Contact

For bug reports or suggestions, contact the maintainer: j.wuttke@fz-juelich.de.

# Installation

## From source

Get the latest source archive (tgz) from
[Repository > Tags](https://jugit.fz-juelich.de/mlz/lmfit/tags).

Build&install are based on CMake. Out-of-source build is enforced. After unpacking the source, go to the source directory and do:

```
mkdir build
cd build
cmake ..
ctest
make
make install
```

## RPM Binaries

* https://apps.fedoraproject.org/packages/lmfit maintained by Miro Hrončok
* http://fr2.rpmfind.net/linux/rpm2html/search.php?query=lmfit

## Language bindings:

* https://github.com/jvail/lmfit.js by Jan Vaillant


# Usage

## Manual pages:

* [lmfit(7)](http://apps.jcns.fz-juelich.de/man/lmfit.html): Summary.
* [lmmin2(3)](http://apps.jcns.fz-juelich.de/man/lmmin2.html):
      Generic routine for minimizing whatever sum of squares.
* [lmmin(3)](http://apps.jcns.fz-juelich.de/man/lmmin.html):
      Simpler legacy API without error estimates.
* [lmcurve(3)](http://apps.jcns.fz-juelich.de/man/lmcurve.html):
      Simpler interface for fitting a parametric function f(x;p) to a data set y(x).

## Sample code:
* [curve fitting with lmcurve()](http://apps.jcns.fz-juelich.de/doku/sc/lmfit:basic-curve-fitting-example)
* [surface fitting as example for minimization with lmmin()](http://apps.jcns.fz-juelich.de/doku/sc/lmfit:surface-fitting-example)
* [nonlinear equations solving with lmmin()](http://apps.jcns.fz-juelich.de/doku/sc/lmfit:nonlinear-equations-example)


# FAQ

## How to constrain parameters?

In many applications, it is desirable to impose constraints like p0>=0 or 10<p1<10.

In lmfit, there is no explicit for mechanism for this, and it is unlikely that I will ever add one: In my experience, it is sufficient to mimick constraints by variable transform or by adding a penalty to the sum of squares. Variable transform seems to be the better solution.

In the above examples: To enforce p0'>0, use e.g. p0'=exp(p0) or p0'=p0^2. To enforce -10<p1'<10, use p1'=10*tanh(p1).

## How to weigh data points?

In least-squares curve fitting, data points _y_(_x_) are approximated by function values _f_(_x_;**p**). Sometimes, one wants to assign data points a weight _w_(_x_). One then has to minimize the sum of squares of _w_*(_y_-_f_).

### How to implement

This can be implemented easily by copying _lmcurve.h_ and _lmcurve.c_, and modifying them in obvious ways, just introducing a new parameter _w_.

### How to choose weights

Assuming that data points _y_(_x_) follow Gaussian distributions with standard deviations _dy_(_x_), the appropriate weight is _w_(_x_) = 1 / _dy_(_x_).

### How to deal with Poisson-distributed data

Least-squares fitting is not justified for Poisson-distributed data -- except if the data are large enough (say, on average _y_ of the order of 10 or larger) so that the Poisson distribution is reasonably well approximated by a Gaussian with standard deviation _dy_=sqrt(_y_). When experimental data have on average too low values _y_ per channel, they should be binned into fewer channels with better statistics before any least-squares fitting.