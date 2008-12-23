#include "nrc.h"

#include <stdio.h>

void powell(float p[], float **xi, int n, float ftol, int *iter, float *fret, float (*func)(float [], IplImage*), IplImage *image) {
  int i, ibig, j;
  float del, fp, fptt, t, *pt, *ptt, *xit;

  pt = malloc(sizeof(float)*n); // minus 1 so index is 1..n instead of 0.. n-1
  pt--;
  ptt = malloc(sizeof(float)*n);
  ptt--;
  xit = malloc(sizeof(float)*n);
  xit--;

  /// evaluate the parameters
  *fret=(*func)(p, image);
  /// save the initial point
  for (j = 1; j <= n; j++) pt[j]= p[j];
  /// iteration:
  for (*iter = 1;; ++(*iter)) {
    fp = (*fret);
    ibig = 0;
    del = 0.0;
    /// for each direction
    for (i = 1; i <= n; i++) {
      /// copy the direction
      for (j = 1; j <= n; j++) xit[j] = xi[j][i];
      fptt = (*fret);
      /// minimize along the direction
      linmin(p, xit, n, fret, func, image);

      // added so we can give up gracefully
      if (*fret == -1)
	return;

      /// record largest decrease so far
      if (fabs(fptt - (*fret)) > del) {
        del = fabs(fptt - (*fret));
        ibig = i;
      }
    }
    /// if we haven't decreased much, we are done, stop
    if (2.0*fabs(fp - (*fret)) <= ftol*(fabs(fp) + fabs(*fret))) {
      free(xit + 1);
      free(ptt + 1);
      free(pt + 1);
      return;
    }
    /// check if we have hit the maximum number of iterations
    if (*iter == ITMAX) {
      printf("powell exceeded maximum iterations\n");
      return;
    }
    for (j = 1; j <= n; j++) {
      /// calculate the extrapolated point
      ptt[j] = 2.0*p[j] - pt[j];
      /// calculate the new direction
      xit[j] = p[j] - pt[j];
      pt[j] = p[j];
    }
    /// evaluate the image at the extrapolated point
    fptt = (*func)(ptt, image);

    /// if the value at the extrapolated point is less than the original value
    if (fptt < fp) {
      t = 2.0*(fp - 2.0*(*fret) + fptt)*pow(fp - (*fret) - del, 2.0) - del*pow(fp-fptt, 2.0);
      /// if we should add the new direction
      if (t < 0.0) {
        /// minimize in this direction
        linmin(p, xit, n, fret, func, image);

	// added so we can give up gracefully
	if (*fret == -1)
	  return;

        /// save this direction
        for (j = 1; j <= n; j++) {
          xi[j][ibig] = xi[j][n];
          xi[j][n] = xit[j];
        }
      }
    }
  }
}

void linmin(float p[], float xi[], int n, float *fret, float (*func)(float [], IplImage*), IplImage *image) {
  int i, j;
  float xx, xmin, fx, fb, fa, bx, ax;

  float dot = 0.0;
  for (i = 1; i <=n; i++) dot += p[i]*xi[i];

  ax = -0.1*dot;
  xx = 0.1*dot;
  mnbrak(p, xi, n, &ax, &xx, &bx, &fa, &fx, &fb, func, image);
  *fret = brent(p, xi, n, ax, xx, bx, func, TOL, &xmin, image);

  // added so we can give up gracefully
  if (*fret == -1)
    return;

  for (j = 1; j <= n; j++) {
    xi[j] *= xmin;
    p[j] += xi[j];
  }
}

void mnbrak(float p[], float d[], int n, float *ax, float *bx, float *cx, float *fa, float *fb, float *fc, float (*func)(float[], IplImage*), IplImage *image) {
  float ulim, u, r, q, fu, dum;
  float *p2 = malloc(n*sizeof(float));
  p2--;
  int i;

  for (i = 1; i <= n; i++) p2[i] = p[i] + *ax*d[i];
  *fa = (*func)(p2, image);

  for (i = 1; i <= n; i++) p2[i] = p[i] + *bx*d[i];
  *fb = (*func)(p2, image);

  if (*fb > *fa) {
    SHFT(dum, *ax, *bx, dum);
    SHFT(dum, *fb, *fa, dum);
  }

  *cx = (*bx)+GOLD*(*bx - *ax);

  for (i = 1; i <= n; i++) p2[i] = p[i] + *cx*d[i];
  *fc = (*func)(p2, image);

  while (*fb > *fc) {
    r = (*bx - *ax)*(*fb - *fc);
    q = (*bx - *cx)*(*fb - *fa);
    u = (*bx) - ((*bx - *cx)*q - (*bx - *ax)*r)/
      (2.0*SIGN(FMAX(fabs(q-r), TINY), q-r));
    ulim = (*bx) + GLIMIT*(*cx - *bx);

    if ((*bx - u)*(u - *cx) > 0.0) {
      for (i = 1; i <= n; i++) p2[i] = p[i] + u*d[i];
      fu = (*func)(p2, image);
      if (fu < *fc) {
        *ax = (*bx);
        *bx = u;
        *fa = (*fb);
        *fb = fu;
        free(p2 + 1);
        return;
      } else if (fu > *fb) {
        *cx = u;
        *fc = fu;
        free(p2 + 1);
        return;
      }
      u = (*cx) + GOLD*(*cx - *bx);
      for (i = 1; i <= n; i++) p2[i] = p[i] + u*d[i];
      fu = (*func)(p2, image);
    } else if ((*cx - u)*(u - ulim) > 0.0) {
      for (i = 1; i <= n; i++) p2[i] = p[i] + u*d[i];
      fu = (*func)(p2, image);
      if (fu < *fc) {
        SHFT(*bx, *cx, u, *cx + GOLD*(*cx - *bx));
        for (i = 1; i <= n; i++) p2[i] = p[i] + u*d[i];
        SHFT(*fb, *fc, fu, (*func)(p2, image));
      }
    } else if ((u - ulim)*(ulim - *cx) >= 0.0){
      u = ulim;
      for (i = 1; i <= n; i++) p2[i] = p[i] + u*d[i];
      fu = (*func)(p2, image);
    } else {
      u = (*cx) + GOLD*(*cx - *bx);
      for (i = 1; i <= n; i++) p2[i] = p[i] + u*d[i];
      fu = (*func)(p2, image);
    }
    SHFT(*ax, *bx, *cx, u);
    SHFT(*fa, *fb, *fc, fu);
  }
}


float brent (float pt[], float dir[], int n, float ax, float bx, float cx, float (*func)(float [], IplImage*), float tol, float *xmin, IplImage *image) {
  int i, iter;
  float a,b,d,etemp,fu,fv,fw,fx,p,q,r,tol1,tol2,u,v,w,x,xm,*p2;
  p2 = malloc(n*sizeof(float));
  p2--;
  float e = 0.0;

  a = (ax < cx ? ax : cx);
  b = (ax > cx ? ax : cx);
  x=w=v=bx;
  for (i = 1; i <= n; i++) p2[i] = pt[i] + x*dir[i];
  fw=fv=fx=(*func)(p2, image);
  for (iter = 1; iter <= ITMAX; iter++) {
    xm = 0.5*(a+b);
    tol2 = 2.0*(tol1=tol*fabs(x)+ZEPS);
    if (fabs(x-xm) <= (tol2 - 0.5*(b-a))) {
      *xmin = x;
      free(p2 + 1);
      return fx;
    }
    if (fabs(e) > tol1) {
      r = (x-w)*(fx-fv);
      q = (x-v)*(fx-fw);
      p = (x-v)*q-(x-w)*r;
      q = 2.0*(q-r);
      if (q > 0.0) p = -p;
      q = fabs(q);
      etemp = e;
      e = d;
      if (fabs(p) >= fabs(0.5*q*etemp) || p <= q*(a-x) || p >= q*(b-x))
        d = CGOLD*(e=(x >= xm ? a-x : b-x));
      else {
        d = p/q;
        u = x + d;
        if (u - a < tol2 || b - u < tol2)
          d = SIGN(tol1, xm-x);
      }
    } else
      d = CGOLD*(e=(x >= xm ? a-x : b-x));
    u = (fabs(d) >= tol1 ? x+d : x + SIGN(tol1, d));
    for (i = 1; i <= n; i++) p2[i] = pt[i] + u*dir[i];
    fu = (*func)(p2, image);
    if (fu <= fx) {
      if (u >= x) a=x; else b = x;
      SHFT(v,w,x,u);
      SHFT(fv,fw,fx,fu);
    } else {
      if (u < x) a = u; else b = u;
      if (fu <= fw || w == x) {
        v = w;
        w = u;
        fv = fw;
        fw = fu;
      } else if (fu <= fv || v == x || v == w) {
        v = u;
        fv = fu;
      }
    }
  }
  printf("too many iterations in brent\n");
  return -1;
}
