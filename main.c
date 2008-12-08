#include <GL/glut.h>

#include "camera.h"
#include "model.h"

int main (int argc, char **argv) {

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
  glutInitWindowSize(WIDTH,HEIGHT);
  glutCreateWindow("yogahero");
  glutDisplayFunc(model_draw_from_vector);

  model_set_zero();
  model_set_type(cylinder);

  glutMainLoop();
 
  return 0;
}
