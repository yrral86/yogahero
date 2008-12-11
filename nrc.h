/**
   @file nrc.h
   @author Larry Reaves

   Powell's Method from Numerical Recipies in C, adapted for YogaHero

  */

#ifndef __NRC_H__
#define __NRC_H__

#include <cv.h>

/// Maximum number of iterations
#define ITMAX 200
#define TINY 1.0e-20
#define TOL 2.0e-4
#define GOLD 1.618034
#define GLIMIT 100.0
#define CGOLD 0.3819660
#define ZEPS 1.0e-10
#define SHFT(a,b,c,d) (a)=(b);(b)=(c);(c)=(d);
#define SIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))
static float maxarg1,maxarg2;
#define FMAX(a,b) (maxarg1=(a),maxarg2=(b),(maxarg1) > (maxarg2) ?\
        (maxarg1) : (maxarg2))


/**
   Powell's Direction Set Minimization Method

   @param p[] parameters
   @param xi basis vectors
   @param n number of parameters

   @param ftol fractional tolerance- stopping criteria
   @param iter number of iterations minimization takes
   @param fret value of error function
   @param func pointer to error function
   @param image image we are attempting to match (should be binary mask)
   @see linmin, error_function
  */
void powell(float p[], float **xi, int n, float ftol, int *iter, float *fret, float (*func)(float [], IplImage*), IplImage *image);

/**
   Linear minimization method.  Finds the minimum in a given direction
   in n dimensional space.

   @param p[] parameters
   @param xi basis vectors
   @param n number of parameters
   @param fret value of error function
   @param func pointer to error function
   @param image image we are attempting to match (should be binary mask)
   @see error_function, mnbrak, powell
  */
void linmin(float p[], float xi[], int n, float *fret, float (*func)(float [], IplImage*), IplImage *image);

/**
   Brackets our function

   @param p initial point
   @param d search direction
   @param n size of p/d
   @param ax initial lower bound, brackets minimum
   @param bx initial upper bound, brackets minimum
   @param cx brackets minimum
   @param fa function evaluated at ax
   @param fb function evaluated at bx
   @param fc function evaluated at cx
   @param func pointer to error function
   @param image image we are attempting to match
  */
void mnbrak(float p[], float d[], int n, float *ax, float *bx, float *cx, float *fa, float *fb, float *fc, float (*func)(float[], IplImage*), IplImage *image);

/**
   Minimizes our function

   @param pt initial point
   @param dir search direction
   @param n size of p/d
   @param ax brackets minimum
   @param bx brackets minimum
   @param cx brackets minimum
   @param func pointer to error function
   @param tol tolerance, stopping criteria
   @param xmin, minimum value
   @param image image we are attempting to match
  */
float brent (float pt[], float dir[], int n, float ax, float bx, float cx, float (*func)(float [], IplImage*), float tol, float *xmin, IplImage *image);

#endif
