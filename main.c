#include "camera.h"
#include "model.h"
#include "nrc.h"
#include "errorfunc.h"

#include <GL/glut.h>
#include <cv.h>
#include <highgui.h>

void match (IplImage *image) {
  int iterations, n, i, j;
  float returnval;

  n = MODEL_ANGLES + MODEL_SEGMENT_LENGTHS + MODEL_CAMERA;
  float *p = model_get_vector();
  // adjust pointer to be p[1]..p[n]
  p--;

  float **xi;

  xi = malloc(n*sizeof(float));
  xi--;

  for (i = 1; i <= n; i++) {
    xi[i] = malloc(n*sizeof(float));
    xi[i]--;
    for (j = 1; j <= n; j++)
      if (i == j)
	xi[i][j] = 1;
      else
	xi[i][j] = 0;
  }

  powell(p, xi, n, 0.01, &iterations, &returnval, error_function, image);

  for (i = 1; i <= n; i++)
    free(xi[i] + 1);

  free(xi + 1);
}

int main (int argc, char **argv) {
  IplImage *image;
  char imagefn[100], posefn[100];

  if (argc < 3) {
    g_printf("Usage: ./match imagefile posefile [-c] [-f n] [-a n]\n");
    g_printf("-c: enable camera alignment\n");
    g_printf("-f n: enable floor constraints with a weight of n\n");
    g_printf("-a n: enable angle constraints with a weight of n\n");
    exit(0);
  }
  
  strcpy(imagefn, argv[1]);
  strcpy(posefn, argv[2]);

  g_printf("Matching image %s, to pose %s\n", imagefn, posefn);

  image = cvLoadImage(imagefn, 0);

  cvFlip(image, NULL, 0);

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
  glutInitWindowSize(640,480);

  glutCreateWindow("match");

  model_init();

  model_from_file(posefn);

  match(image);

  model_to_file("pose.out");

  cvReleaseImage(&image);
 
  return 0;
}

