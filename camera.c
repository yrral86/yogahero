#include "camera.h"

#include <GL/glut.h>

void camera_set_from_vector(float *params) {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  gluPerspective(params[c_fov],
		 (float)WIDTH/(float)HEIGHT,
		 1*params[c_scale],
		 5*params[c_scale]);

  gluLookAt(params[c_pos_x], params[c_pos_y], params[c_pos_z],
	    params[c_look_x], params[c_pos_y], params[c_look_z],
	    0.0, 1.0, 0.0);

  glMatrixMode(GL_MODELVIEW);		 
}
