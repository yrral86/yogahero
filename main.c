#include "camera.h"
#include "model.h"
#include "nrc.h"
#include "errorfunc.h"

#include <cv.h>
#include <highgui.h>
#include <gtk/gtk.h>
#include <gtk/gtkgl.h>
#include <glib.h>
#include <glib/gprintf.h>

const int n = MODEL_ANGLES + MODEL_SEGMENT_LENGTHS + MODEL_CAMERA;
void match (IplImage*, int*);
static int torsoscale = 0;
static int alignonly = 0;

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

  // disable everything except side+pelvis lengths for optimization
  for (i = 0; i < n; i++)
    if (i != side_s_l &&
	i != pelvis_s_l &&
	i != head_s_l)
      enabled[i] = 0;
    else
      enabled[i] = 1;

  // if requested, also optimize scale
  if (torsoscale)
    enabled[cam + c_scale] = 1;

  p = model_get_vector();

  project(buffer, p - 1);

  error = symmetric_difference(image, buffer);
  olderror = 2*error;

  // begin initial align based on com of model and image
  while (olderror - error > 0) {
    cvMoments(buffer, m, 1);
    mCM.x = cvGetSpatialMoment(m, 1, 0)/cvGetSpatialMoment(m, 0, 0);
    mCM.y = cvGetSpatialMoment(m, 0, 1)/cvGetSpatialMoment(m, 0, 0);

    cvMoments(image, m, 1);
    aCM.x = cvGetSpatialMoment(m, 1, 0)/cvGetSpatialMoment(m, 0, 0);
    aCM.y = cvGetSpatialMoment(m, 0, 1)/cvGetSpatialMoment(m, 0, 0);

    // find pixel difference
    dy = mCM.y - aCM.y;
    dx = mCM.x - aCM.x;

    // save old p
    for (i = 0; i < n; i++)
      oldp[i] = p[i];

    // adjust y
    p[cam + c_pos_y] += 0.001*dy/p[cam + c_scale];

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

    project(buffer, p - 1);

    olderror = error;
    error = symmetric_difference(image, buffer);
  }

  // undo last set of changes, as it increased our error
  for (i = 0; i < n; i++)
    p[i] = oldp[i];

  // hide everything except torso segments
  for (i = 0; i < MODEL_SEGMENTS; i++)
    if (i != pelvis_s &&
	i != r_side_s &&
	i != l_side_s &&
	i != r_shoulder_s &&
	i != l_shoulder_s &&
	i != head_s)
      model_set_invisible(i);

  project(buffer, p - 1);

  error = symmetric_difference(image, buffer);
  olderror = 2*error;

  // begin finer alignment
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
    p[cam + c_pos_y] += 0.001*dy/p[cam + c_scale];

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

    // optimize side + pelvis lengths (and possibly scale)
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
  float initialsd, alignsd, matchsd, aligntime, matchtime;
  GTimer *timer = g_timer_new();

  GdkGLConfig *glconfig;
  GdkGLContext *glcontext;
  GdkPixmap *pixmap;
  GdkGLDrawable *gldrawable;

  if (argc < 3) {
    g_printf("Usage: ./match imagefile posefile [-c] [-f n] [-a n]\n");
    g_printf("-n: no match- just run alignTorso and stop\n");
    g_printf("-s: enable scale in alignTorso\n");
    g_printf("-f n: enable floor constraints with a weight of n\n");
    g_printf("-a n: enable angle constraints with a weight of n\n");
    exit(0);
  }

  for (i = 3; i < argc; i++) {
    if (strcmp(argv[i], "-s") == 0)
      torsoscale = 1;
    if (strcmp(argv[i], "-n") == 0)
      alignonly = 1;
    if (strcmp(argv[i], "-f") == 0)
      error_func_set_floor_weight(atof(argv[++i]));
    if (strcmp(argv[i], "-a") == 0)
      error_func_set_angle_weight(atof(argv[++i]));
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

  buffer = cvCreateImage(cvGetSize(image), 8, 1);

  project(buffer, model_get_vector() - 1);
  initialsd = symmetric_difference(buffer, image);

  printf("error before alignTorso: %g\n", initialsd);

  g_timer_start(timer);
  alignTorso(image);
  g_timer_stop(timer);

  aligntime = g_timer_elapsed(timer, NULL);

  project(buffer, model_get_vector() - 1);
  alignsd = symmetric_difference(buffer, image);

  printf("error after alignTorso: %g\n", alignsd);
  printf("time for alignTorso: %gs\n", aligntime);

  if (!alignonly) {
    for (i = 0; i < n; i++)
      enabled[i] = 1;

    enabled[shoulder_s_l] = 0;

    // match image to pose
    g_timer_start(timer);
    match(image, enabled);
    g_timer_stop(timer);

    matchtime = g_timer_elapsed(timer, NULL);

  } else
    matchtime = 0.0;

    project(buffer, model_get_vector() - 1);
    matchsd = symmetric_difference(buffer, image);

    printf("error after match: %g\n", matchsd);
    printf("time for match: %gs\n", matchtime);
    printf("total time: %gs\n", matchtime + aligntime);


  // save model as image
  cvFlip(buffer, NULL, 0);
  cvSaveImage("match.png", buffer);

  // save model as file
  model_to_file("match.pose");

  // save timer/error data
  FILE *out = fopen("match.data", "w");
  fprintf(out, "%g\t%g\t%g\t%g\t%g\t%g", initialsd, alignsd, matchsd,
	  aligntime, matchtime, aligntime+matchtime);
  fclose(out);

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
