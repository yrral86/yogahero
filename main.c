#include <GL/glut.h>

#include "model.h"

#define WIDTH 640
#define HEIGHT 480

int main (int argc, char **argv) {

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
  glutInitWindowSize(WIDTH,HEIGHT);
  glutCreateWindow("yogahero");
  glutDisplayFunc(model_draw_from_vector);

  model_set_zero();

  glutMainLoop();
 
  return 0;
}
