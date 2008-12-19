#include "camera.h"
#include "model.h"
#include "nrc.h"
#include "errorfunc.h"

#include <cv.h>
#include <highgui.h>
#include <gtk/gtk.h>
#include <gtk/gtkgl.h>
#include <glib/gprintf.h>

const int n = MODEL_ANGLES + MODEL_SEGMENT_LENGTHS + MODEL_CAMERA;
void match (IplImage*, int*);

void alignTorso(IplImage *image) {
  int i, cam = MODEL_ANGLES + MODEL_SEGMENT_LENGTHS;
  int enabled[n];
  IplImage *buffer = cvCreateImage(cvGetSize(image), 8, 1);
  IplImage *and = cvCreateImage(cvGetSize(image), 8, 1);
  CvMoments *m = malloc(sizeof(CvMoments));
  CvPoint2D32f mCM, aCM;
  float dx, dy;
  float mag;
  float mdx, mdz;
  float *p;
  float error, olderror;
  float oldp[n];

  // hide everything except torso segments
  for (i = 0; i < MODEL_SEGMENTS; i++)
    if (i != pelvis_s &&
	i != r_side_s &&
	i != l_side_s &&
	i != r_shoulder_s &&
	i != l_shoulder_s)
      model_set_invisible(i);

  // disable everything except side and pelvis lengths for optimization
  for (i = 0; i < n; i++)
    if (i != side_s_l &&
	i != pelvis_s_l)
      enabled[i] = 0;
    else
      enabled[i] = 1;

  p = model_get_vector();

  project(buffer, p - 1);

  error = symmetric_difference(image, buffer);
  olderror = 2*error;

  while (olderror - error > 0) {
    cvAnd(buffer, image, and, NULL);

    cvMoments(buffer, m, 1);
    mCM.x = cvGetSpatialMoment(m, 1, 0)/cvGetSpatialMoment(m, 0, 0);
    mCM.y = cvGetSpatialMoment(m, 0, 1)/cvGetSpatialMoment(m, 0, 0);

    cvMoments(and, m, 1);
    aCM.x = cvGetSpatialMoment(m, 1, 0)/cvGetSpatialMoment(m, 0, 0);
    aCM.y = cvGetSpatialMoment(m, 0, 1)/cvGetSpatialMoment(m, 0, 0);

    // find pixel difference
    dy = mCM.y - aCM.y;
    dx = mCM.x - aCM.x;

    // save old p
    for (i = 0; i < n; i++)
      oldp[i] = p[i];

    // adjust y
    p[cam + c_pos_y] += 0.01*dy/p[cam + c_scale];

    // calculate "x" movement
    // find line to move along (negative inverse of dz/dx)
    mdx = p[cam+c_look_z]-p[cam+c_pos_z];
    mdz = p[cam+c_pos_x]-p[cam+c_look_x];

    // normalize so we move 1 unit
    mag = sqrt(mdx*mdx + mdz*mdz);
    mdx /= mag;
    mdz /= mag;

    // move "x" by adjusting x and z
    p[cam+c_pos_x] += 0.01*p[cam+c_scale]*mdx;
    p[cam+c_look_x] += 0.01*p[cam+c_scale]*mdx;
    p[cam+c_pos_z] += 0.01*p[cam+c_scale]*mdz;
    p[cam+c_look_z] += 0.01*p[cam+c_scale]*mdz;

    // optimize side and pelvis lengths
    // p will be ok since it is a pointer to the static variable
    match(image, enabled);

    project(buffer, p - 1);

    olderror = error;
    error = symmetric_difference(image, buffer);
  }

  // undo last set of changes, as it increased our error
  for (i = 0; i < n; i++)
    p[i] = oldp[i];

  cvReleaseImage(&buffer);
  cvReleaseImage(&and);
  free(m);

  // unhide everything
  for (i = 0; i < MODEL_SEGMENTS; i++)
    model_set_visible(i);
}

void match (IplImage *image, int *enabled) {
  int iterations, i, j;
  float returnval;
  float *p;
  float *mp;

  mp = model_get_vector();
  p = malloc(n*sizeof(float));

  for (i = 0; i < n; i++) {
    p[i] = mp[i];
  }

  // adjust pointer to be p[1]..p[n]
  p--;
  enabled--;

  float **xi;

  xi = malloc(n*sizeof(float));
  xi--;

  for (i = 1; i <= n; i++) {
    xi[i] = malloc(n*sizeof(float));
    xi[i]--;
    for (j = 1; j <= n; j++)
      if (i == j)
	if (enabled[i])
	  xi[i][j] = 1;
	else
	  xi[i][j] = 0;
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
  int i;
  int enabled[n];

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
  model_set_type(ellipsoid);
  model_from_file(posefn);


  printf("error before alignTorso: %g\n", error_function(model_get_vector() - 1, image));
  alignTorso(image);
  printf("error after alignTorso: %g\n", error_function(model_get_vector() - 1, image));


  for (i = 0; i < n; i++)
    enabled[i] = 1;

  enabled[shoulder_s_l] = 0;

  // match image to pose
  printf("error before match: %g\n", error_function(model_get_vector() - 1, image));
  match(image, enabled);
  printf("error after match: %g\n", error_function(model_get_vector() - 1, image));

  // save model as image
  buffer = cvCreateImage(cvGetSize(image), 8, 1);
  project(buffer, model_get_vector() - 1);
  cvFlip(buffer, NULL, 0);
  cvSaveImage("pose.png", buffer);

  // save model as file
  model_to_file("pose.out");

  // end opengl
  gdk_gl_drawable_gl_end(gldrawable);

  // cleanup images
  cvReleaseImage(&image);
  cvReleaseImage(&buffer);

  // cleanup gl
  g_object_unref(glconfig);
  g_object_unref(glcontext);
 
  return 0;
}

