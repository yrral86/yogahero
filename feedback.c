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
#include <GL/gl.h>

const int n = MODEL_ANGLES + MODEL_SEGMENT_LENGTHS + MODEL_CAMERA;

void project_color(IplImage *img, float *p) {
  model_set_vector(p+1);
  model_draw_from_vector();
  glReadPixels(0,0,img->width,img->height,GL_BGR,
	       GL_UNSIGNED_BYTE,img->imageData);
}

int main (int argc, char **argv) {
  IplImage *image;
  IplImage *buffer;
  char imagefn[100], posefn[100], matchfn[100];

  GdkGLConfig *glconfig;
  GdkGLContext *glcontext;
  GdkPixmap *pixmap;
  GdkGLDrawable *gldrawable;

  if (argc != 4) {
    g_printf("Usage: ./feedback imagefile posefile matchfile\n");
    g_printf("imagefile: image to draw on\n");
    g_printf("posefile: pose image is attempting\n");
    g_printf("matchfile: pose matched to image\n");
    exit(0);
  }

  // copy off our arguments
  strcpy(imagefn, argv[1]);
  strcpy(posefn, argv[2]);
  strcpy(matchfn, argv[3]);

  g_printf("Drawing model %s in pose %s on image %s\n",
	   matchfn, posefn, imagefn);

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
  model_set_type(stick);
  model_from_file(matchfn);
  pose_from_file(posefn);

  buffer = cvCreateImage(cvGetSize(image), 8, 3);

  model_set_image(image);

  project_color(buffer, model_get_vector() - 1);


  // save model as image
  cvFlip(buffer, NULL, 0);
  cvSaveImage("drawn.png", buffer);


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
