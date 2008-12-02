#include "model.h"

#include <GL/glut.h>
#include <math.h>

static float vector[MODEL_ANGLES + MODEL_SEGMENTS + MODEL_CAMERA];

void model_draw_from_vector() {
  camera_set_from_vector(vector + MODEL_ANGLES + MODEL_SEGMENTS);

  
}

void model_set_vector(float* v) {
  int i;
  int n = MODEL_ANGLES + MODEL_SEGMENTS + MODEL_CAMERA;

  for (i = 0; i < n; i++)
    vector[i] = v[i];
}

void model_set_zero() {
  int i;
  
  // angle parameters
  for (i = 0; i < MODEL_ANGLES; i++) {
    vector[i] = 0;
  }
  
#define CUBIT 2.0

  // FIXME: legs aren't right, so sides aren't right
  // (vitruvian man doesn't mention legs)
  // segment parameters (vitruvian man)
  // head
  vector[i++] = 0.5*CUBIT;

  // shoulder
  vector[i++] = 0.5*CUBIT;

  // upper arm
  vector[i++] = 0.5*CUBIT;

  // lower arm
  vector[i++] = 0.5*CUBIT;

  // hand
  vector[i++] = 1.0/3.0*CUBIT;

  // side (sqrt(0.25^2 + 1.5^2) cubits)
  vector[i++] = sqrt(2.3125);

  // pelvis
  vector[i++] = 0.5*CUBIT;

  // upper leg
  vector[i++] = CUBIT;

  // lower leg
  vector[i++] = CUBIT;

  // foot
  vector[i++] = 0.5*CUBIT;

  // camera parameters
  // camera position x,y,z (origin is toe end of right foot)
  vector[i++] = 0.0;
  vector[i++] = 2.0;
  vector[i++] = -2.0;

  // where we are looking x,y,z
  vector[i++] = 0.0;
  vector[i++] = 2.0;
  vector[i++] = 0.0;

  // where is up x,y,z (this is constant assuming level camera)
  vector[i++] = 0.0;
  vector[i++] = 1.0;
  vector[i++] = 0.0;

  // fov
  vector[i++] = 30.0;

  // scale
  vector[i] = 1.0;
}
