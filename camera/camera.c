#include "camera.h"

#include <GL/glut.h>

void camera_set_from_vector(float *params) {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  printf("fov = %g, scale = %g\n", params[c_fov], params[c_scale]);
  gluPerspective(params[c_fov],
		 (float)WIDTH/(float)HEIGHT,
		 1*params[c_scale],
		 5*params[c_scale]);

  printf("pos: x = %g, y = %g, z = %g\n", params[c_pos_x],
	 params[c_pos_y], params[c_pos_z]);
  printf("look: x = %g, y = %g, z = %g\n", params[c_look_x],
	 params[c_pos_y], params[c_look_z]);
  printf("up: x = %g, y = %g, z = %g\n", 0.0, 1.0, 0.0);
  gluLookAt(params[c_pos_x], params[c_pos_y], params[c_pos_z],
	    params[c_look_x], params[c_pos_y], params[c_look_z],
	    0.0, 1.0, 0.0);

  glMatrixMode(GL_MODELVIEW);		 
}
