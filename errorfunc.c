#include "errorfunc.h"
#include "model.h"

#include <GL/glut.h>

static float floorWeight = 0.0;
static float angleWeight = 0.0;
static count = 0;

float error_function(float p[], IplImage *image) {
  IplImage *buffer = cvCreateImage(cvGetSize(image), 8, 1);
  float sd, fe, ae;
  char fn[100];
  
  project(buffer, p);

  sd = symmetric_difference(image, buffer);

  /*
  sprintf(fn, "out/%d.model", count);
  model_to_file(fn);
  sprintf(fn, "out/%d.png", count++);
  cvSaveImage(fn, buffer);
  */

  cvReleaseImage(&buffer);

  return sd;// + floorWeight*fe + angleWeight*ae;
}

float floor_error();
float angle_error(float *p);

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
