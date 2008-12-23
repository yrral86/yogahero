#ifndef __ERRORFUNC_H__
#define __ERRORFUNC_H__

#include <cv.h>

/**
   Calculates the error between parameters p and the image.

   @param p[] the model and camera parameters
   @param image image to match
   @see symmetric_difference
   @return error
  */
float error_function(float p[], IplImage *image);

/**
   Calculates the error from joint angle constraints

   @param p the model to calculate the error for
   @return error
  */
float angle_error(float *p);

/**
   Calculates the area of the symmetric differenct between two
   binary images

   @param img1 the first image
   @param img2 the second image
   @return img1 | img2 - img1 & img2
  */
float symmetric_difference(IplImage *img1, IplImage *img2);


void project (IplImage*, float*);

void error_func_set_floor_weight(float);
void error_func_set_angle_weight(float);

#endif
