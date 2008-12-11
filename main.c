#include "camera.h"
#include "model.h"
#include "nrc.h"
#include "errorfunc.h"

#include <cv.h>
#include <highgui.h>
#include <gtk/gtk.h>
#include <gtk/gtkgl.h>

void match (IplImage *image) {
  int iterations, n, i, j;
  float returnval;
  float *p;
  float *mp;

  n = MODEL_ANGLES + MODEL_SEGMENT_LENGTHS + MODEL_CAMERA;
  mp = model_get_vector();
  p = malloc(n*sizeof(float));

  for (i = 0; i < n; i++) {
    p[i] = mp[i];
  }

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

  free(p + 1);
}

int main (int argc, char **argv) {
  IplImage *image;
  IplImage *buffer;
  char imagefn[100], posefn[100];

  GdkGLConfig *glconfig;
  GdkGLContext *glcontext;
  GdkPixmap *pixmap;
  GdkGLDrawable *gldrawable;
  
  if (argc < 3) {
    g_printf("Usage: ./match imagefile posefile [-c] [-f n] [-a n]\n");
    g_printf("-c: enable camera alignment\n");
    g_printf("-f n: enable floor constraints with a weight of n\n");
    g_printf("-a n: enable angle constraints with a weight of n\n");
    exit(0);
  }
  
  // copy off our arguments
  strcpy(imagefn, argv[1]);
  strcpy(posefn, argv[2]);

  g_printf("Matching image %s, to pose %s\n", imagefn, posefn);

  // load image
  image = cvLoadImage(imagefn, 0);
  cvFlip(image, NULL, 0);


  // begin off-screen rendering setup
  gtk_init(&argc, &argv);
  gtk_gl_init(&argc, &argv);
  glconfig = gdk_gl_config_new_by_mode(GDK_GL_MODE_RGB|
				       GDK_GL_MODE_DEPTH|
				       GDK_GL_MODE_SINGLE);
  pixmap = gdk_pixmap_new(NULL, 640, 480, 24);
  gldrawable = GDK_GL_DRAWABLE(gdk_pixmap_set_gl_capability(pixmap,
							    glconfig,
							    NULL));
  glcontext = gdk_gl_context_new(gldrawable,
				 NULL,
				 TRUE,
				 GDK_GL_RGBA_TYPE);
  gdk_gl_drawable_gl_begin(gldrawable, glcontext);
  // end off-screen setup

  if (!gdk_gl_context_is_direct(glcontext)) {
    printf("Warning: no direct rendering... falling back to software rendering.\n");
  }

  // prepare model
  model_init();
  model_from_file(posefn);

  // match image to pose
  printf("error before match: %g\n", error_function(model_get_vector() - 1, image));
  match(image);
  printf("error after match: %g\n", error_function(model_get_vector() - 1, image));

  // save model as image
  buffer = cvCreateImage(cvGetSize(image), 8, 1);
  project(buffer, model_get_vector() - 1);
  cvFlip(buffer, NULL, 0);
  cvSaveImage("pose.png", buffer);

  // end opengl
  gdk_gl_drawable_gl_end(gldrawable);

  // save model as file
  model_to_file("pose.out");

  // cleanup images
  cvReleaseImage(&image);
  cvReleaseImage(&buffer);

  // cleanup gl
  g_object_unref(glconfig);
  g_object_unref(glcontext);
 
  return 0;
}

