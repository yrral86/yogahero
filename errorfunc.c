#include "errorfunc.h"
#include "model.h"

#include <GL/gl.h>

static float floorWeight = 0.0;
static float angleWeight = 0.0;
//static int count = 0;

float error_function(float p[], IplImage *image) {
  IplImage *buffer = cvCreateImage(cvGetSize(image), 8, 1);
  float sd, fe, ae;
  //  char fn[100];
  
  project(buffer, p);

  sd = symmetric_difference(image, buffer);

  if (floorWeight > 0)
    fe = model_get_floor_error();
  else
    fe = 0.0;

  if (angleWeight > 0)
    ae = angle_error(p);
  else
    ae = 0.0;

  /*
  sprintf(fn, "out/%d.model", count);
  model_to_file(fn);
  sprintf(fn, "out/%d.png", count++);
  cvSaveImage(fn, buffer);
  */

  cvReleaseImage(&buffer);

  return sd + floorWeight*fe + angleWeight*ae;
}

float angle_error(float *p) {
  int i;
  float error = 0.0;

  for (i = 0; i < MODEL_ANGLES; i++)
    if (p[i] < model_get_min(model_angle_to_constraint(i)))
      error += pow(model_get_min(model_angle_to_constraint(i)) - p[i], 8.0);
    else if (p[i] > model_get_max(model_angle_to_constraint(i)))
      error += pow(p[i] - model_get_max(model_angle_to_constraint(i)), 8.0);

  return error;
}

float symmetric_difference(IplImage *img1, IplImage *img2) {
  IplImage *error = cvCreateImage(cvGetSize(img1), 8, 1);
  CvMoments m;
  float mass;

  cvXor(img1, img2, error, NULL);
  cvMoments(error, &m, 1);
  mass = cvGetSpatialMoment(&m, 0, 0);

  cvReleaseImage(&error);

  return mass;
}

void project(IplImage *img, float *p) {
  model_set_vector(p + 1);
  
  model_draw_from_vector();
  
  glReadPixels(0, 0, img->width, img->height, GL_LUMINANCE,
	       GL_UNSIGNED_BYTE, img->imageData);
}

void error_func_set_floor_weight(float w) {
  floorWeight = w;
}

void error_func_set_angle_weight(float w) {
  angleWeight = w;
}
