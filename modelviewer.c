#include "camera.h"
#include "model.h"

#include <glib.h>
#include <gtk/gtk.h>
#include <gtk/gtkgl.h>
#include <glade/glade.h>

#include <GL/glut.h>

GladeXML *xml;
GtkWidget *area;

static float vector[MODEL_ANGLES + MODEL_SEGMENTS + MODEL_CAMERA];

void viewer_set_vector() {
  int i;
  GString *name = g_string_new("");

  for (i = 0; i < MODEL_ANGLES + MODEL_SEGMENTS + MODEL_CAMERA; i++) {
    g_string_printf(name, "scale%d", i);
    vector[i] = gtk_range_get_value(GTK_RANGE(glade_xml_get_widget(xml, name->str)));
  }
}

void viewer_set_from_vector() {
  int i;
  GString *name = g_string_new("");

  for (i = 0; i < MODEL_ANGLES + MODEL_SEGMENTS + MODEL_CAMERA; i++) {
    g_string_printf(name, "scale%d", i);
    gtk_range_set_value(GTK_RANGE(glade_xml_get_widget(xml, name->str)), vector[i]);
  }
}

void on_drawButton_clicked(GtkWidget *widget, GdkEventButton *event) {
  GdkGLContext *glcontext;
  GdkGLDrawable *gldrawable;

  viewer_set_vector();
  model_set_vector(vector);
  model_set_type(cylinder);

  glcontext = gtk_widget_get_gl_context (area);
  gldrawable = gtk_widget_get_gl_drawable (area);
  gdk_gl_drawable_gl_begin (gldrawable, glcontext);

  model_draw_from_vector();

  gdk_gl_drawable_gl_end (gldrawable);
}

void on_loadButton_clicked(GtkWidget *widget, GdkEventButton *event) {
  GtkWidget *filechooser;
  gchar *filename;
  int i;
  float *mvector;

  filechooser = gtk_file_chooser_dialog_new("Load Pose",
					    NULL,
					    GTK_FILE_CHOOSER_ACTION_OPEN,
					    GTK_STOCK_CANCEL,
					    GTK_RESPONSE_CANCEL,
					    GTK_STOCK_OPEN,
					    GTK_RESPONSE_ACCEPT,
					    NULL);

  if (gtk_dialog_run(GTK_DIALOG(filechooser)) == GTK_RESPONSE_ACCEPT) {
    filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(filechooser));
    model_from_file((char*)filename);
    g_free(filename);
  }

  gtk_widget_destroy(filechooser);

  mvector = model_get_vector();

  for (i = 0; i < MODEL_ANGLES + MODEL_SEGMENTS + MODEL_CAMERA; i++) {
    vector[i] = mvector[i];
  }
  
  viewer_set_from_vector();
  
  on_drawButton_clicked(widget, event);
}

void on_saveButton_clicked(GtkWidget *widget, GdkEventButton *event) {
  GtkWidget *filechooser;
  gchar *filename;

  filechooser = gtk_file_chooser_dialog_new("Save Pose",
					    NULL,
					    GTK_FILE_CHOOSER_ACTION_SAVE,
					    GTK_STOCK_CANCEL,
					    GTK_RESPONSE_CANCEL,
					    GTK_STOCK_SAVE,
					    GTK_RESPONSE_ACCEPT,
					    NULL);

  if (gtk_dialog_run(GTK_DIALOG(filechooser)) == GTK_RESPONSE_ACCEPT) {
    filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(filechooser));
    model_to_file((char*)filename);
    g_free(filename);
  }

  gtk_widget_destroy(filechooser);
}

void on_resetButton_clicked(GtkWidget *widget, GdkEventButton *event) {
  int i;
  float *mvector;

  model_set_zero();

  mvector = model_get_vector();

  for (i = 0; i < MODEL_ANGLES + MODEL_SEGMENTS + MODEL_CAMERA; i++) {
    vector[i] = mvector[i];
  }

  viewer_set_from_vector();

  on_drawButton_clicked(widget, event);
}

int main (int argc, char **argv) {
  GtkWidget *win;
  GtkWidget *glwin;
  GThread *thread;
  GdkGLConfig *config;

  gtk_init(&argc, &argv);
  gtk_gl_init(&argc, &argv);

  config = gdk_gl_config_new_by_mode(GDK_GL_MODE_SINGLE | GDK_GL_MODE_RGB);
  
  xml = glade_xml_new("modelviewer.ui", NULL, NULL);

  glade_xml_signal_autoconnect(xml);

  win = glade_xml_get_widget(xml, "mainWindow");

  gtk_widget_show_all(GTK_WIDGET(win));

  glwin = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  gtk_window_set_title(GTK_WINDOW(glwin), "Model");

  area = gtk_drawing_area_new();

  gtk_widget_set_size_request(area, WIDTH, HEIGHT);

  gtk_widget_set_gl_capability(area, config, NULL, TRUE, GDK_GL_RGBA_TYPE);

  gtk_container_add(GTK_CONTAINER(glwin), area);

  gtk_widget_show_all(glwin);

  gtk_main();
}
