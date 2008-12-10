#include "camera.h"
#include "model.h"

#include <GL/glut.h>
#include <math.h>
#include <stdio.h>

static float vector[MODEL_ANGLES + MODEL_SEGMENTS + MODEL_CAMERA];
static float min[MODEL_CONSTRAINTS];
static float max[MODEL_CONSTRAINTS];

static model_type type;

static float correction_angle;

void model_draw_from_vector() {
  float scale;
  float sideangle;

  // calculate the angle between the side segment and vertical
  sideangle = asin((vector[shoulder_s_l] - 0.5*vector[pelvis_s_l])
		   / vector[side_s_l]);

  // convert sideangle to degrees
  sideangle *= 180/M_PI;

  // setup openGL projection matrix
  camera_set_from_vector(vector + MODEL_ANGLES + MODEL_SEGMENTS);
  
  // black background
  glClearColor(0.0, 0.0, 0.0, 0.0);
  // white foreground
  glColor3f(1.0, 1.0, 1.0);
  // clear buffer
  glClear(GL_COLOR_BUFFER_BIT);

  scale = (vector + MODEL_ANGLES + MODEL_SEGMENTS)[c_scale]/8.0;

  glLoadIdentity();

  glScalef(scale, scale, scale);

  model_draw_legs();
  
  // clear buffer
  glClear(GL_COLOR_BUFFER_BIT);

  glLoadIdentity();

  glScalef(scale, scale, scale);

  rotate_z(correction_angle);
  
  model_draw_legs();

  // draw left side
  rotate_y(90 - sideangle);
  my_gluCylinder(0.25, vector[side_s_l]);

  // push left shoulder
  glPushMatrix();
  
  // draw left  upper arm
  rotate_y(sideangle - 90 + vector[l_shoulder_j_0]);
  rotate_x(vector[l_shoulder_j_1]);
  my_gluCylinder(0.25, vector[u_arm_s_l]);

  // left shoulder internal rotation
  rotate_internal(-vector[l_shoulder_j_i]);

  // draw left lower arm
  rotate_x(-vector[l_elbow_j_0]);
  my_gluCylinder(0.25, vector[l_arm_s_l]);

  // left elbow internal rotation
  rotate_internal(vector[l_elbow_j_i]);

  // draw left hand
  rotate_x(vector[l_wrist_j_0]);
  rotate_y(vector[l_wrist_j_1]);
  my_gluCylinder(0.25, vector[hand_s_l]);

  // pop back to left shoulder
  glPopMatrix();

  // draw left shoulder
  rotate_y(sideangle + 90);
  my_gluCylinder(0.25, vector[shoulder_s_l]);

  // push base of neck
  glPushMatrix();

  // draw head
  rotate_y(vector[neck_j_1] - 90);
  rotate_x(vector[neck_j_0]);
  my_gluCylinder(0.25, vector[head_s_l]);

  // pop back to base of neck
  glPopMatrix();
  
  // draw right shoulder
  my_gluCylinder(0.25, vector[shoulder_s_l]);

  // push right shoulder
  glPushMatrix();

  // draw right upper arm
  rotate_y(-vector[r_shoulder_j_0]);
  rotate_x(vector[r_shoulder_j_1]);
  my_gluCylinder(0.25, vector[u_arm_s_l]);

  // right shoulder internal rotation
  rotate_internal(vector[r_shoulder_j_i]);

  // draw right lower arm
  rotate_x(-vector[r_elbow_j_0]);
  my_gluCylinder(0.25, vector[l_arm_s_l]);

  // right elbow internal rotation
  rotate_internal(-vector[r_elbow_j_i]);

  // draw right hand
  rotate_x(vector[r_wrist_j_0]);
  rotate_y(-vector[r_wrist_j_1]);
  my_gluCylinder(0.25, vector[hand_s_l]);

  // pop back to right shoulder
  glPopMatrix();

  // draw right side
  rotate_y(90 + sideangle);
  my_gluCylinder(0.25, vector[side_s_l]);

  glFlush();
}

void model_draw_legs() {
  int i;
  float modelview[16];

  // draw right foot
  rotate_y(180.0);
  my_gluCylinder(0.25, vector[foot_s_l]);

  // draw right lower leg
  rotate_x(-90 + vector[r_ankle_j_0]);
  rotate_z(vector[r_ankle_j_1]);
  my_gluCylinder(0.25, vector[l_leg_s_l]);

  // right knee internal rotation
  rotate_internal(vector[r_knee_j_i]);

  // draw right upper leg
  rotate_x(vector[r_knee_j_0]);
  my_gluCylinder(0.25, vector[u_leg_s_l]);

  // right hip internal rotation
  rotate_internal(vector[r_hip_j_i]);

  // draw pelvis
  rotate_y(-90 + vector[r_hip_j_0]);
  rotate_z(vector[r_hip_j_1]);
  my_gluCylinder(0.25, vector[pelvis_s_l]);

  // push left hip
  glPushMatrix();

  // draw left upper leg
  rotate_y(-90 + vector[l_hip_j_0]);
  rotate_x(-vector[l_hip_j_1]);
  my_gluCylinder(0.25, vector[u_leg_s_l]);

  // left hip internal rotation
  rotate_internal(-vector[l_hip_j_i]);

  // draw left lower leg
  rotate_x(vector[l_knee_j_0]);
  my_gluCylinder(0.25, vector[l_leg_s_l]);

  // left knee internal rotation
  rotate_internal(-vector[l_knee_j_i]);

  // draw left foot
  rotate_x(-90 + vector[l_ankle_j_0]);
  rotate_y(vector[l_ankle_j_1]);
  my_gluCylinder(0.25, vector[foot_s_l]);

  glGetFloatv(GL_MODELVIEW_MATRIX, modelview);

  correction_angle = -atan2(modelview[13], modelview[12]);

  correction_angle *= 180.0/M_PI;

  // pop back to left hip
  glPopMatrix();
}

// draws a gluCylinder, with endcap
// from origin to (0, 0, length)
// we only need one endcap as the color
// is uniform, so we only need to ensure
// we can't look down the pipe and see black
void my_gluCylinder(float radius, float length) {
  GLUquadricObj *quad;
  quad = gluNewQuadric();
  gluDisk(quad, 0.0, radius, 32, 32);
  gluCylinder(quad, radius, radius, length, 32, 32);
  gluDeleteQuadric(quad);
  glTranslatef(0.0, 0.0, length);
}

void rotate_internal(float angle) {
  rotate_z(angle);
}

void rotate_x(float angle) {
  glRotatef(angle, 1.0, 0.0, 0.0);
}

void rotate_y(float angle) {
  glRotatef(angle, 0.0, 1.0, 0.0);
}

void rotate_z(float angle) {
  glRotatef(angle, 0.0, 0.0, 1.0);
}

void model_set_constraints() {
  min[neck_c_0] = -100.0;
  max[neck_c_0] = 100.0;
  min[neck_c_1] = -100.0;
  max[neck_c_1] = 100.0;
  min[shoulder_c_0] = -100.0;
  max[shoulder_c_0] = 100.0;
  min[shoulder_c_1] = -150.0;
  max[shoulder_c_1] = 60.0;
  min[elbow_c_0] = -10.0;
  max[elbow_c_0] = 150.0;
  min[wrist_c_0] = -30.0;
  max[wrist_c_0] = 50.0;
  min[wrist_c_1] = -100.0;
  max[wrist_c_1] = 100.0;
  min[hip_c_0] = -15.0;
  max[hip_c_0] = 90.0;
  min[hip_c_1] = -50.0;
  max[hip_c_1] = 100.0;
  min[knee_c_0] = -10.0;
  max[knee_c_0] = 170.0;
  min[ankle_c_0] = -60.0;
  max[ankle_c_0] = 90.0;
  min[ankle_c_1] = -30.0;
  max[ankle_c_1] = 30.0;
  min[shoulder_c_i] = -100.0;
  max[shoulder_c_i] = 100.0;
  min[elbow_c_i] = -190.0;
  max[elbow_c_i] = 100.0;
  min[hip_c_i] = -100.0;
  max[hip_c_i] = 100.0;
  min[knee_c_i] = -30.0;
  max[knee_c_i] = 30.0;
}

float model_get_min(model_angle_constraint c) {
  return min[c];
}

float model_get_max(model_angle_constraint c) {
  return max[c];
}

model_angle_constraint model_angle_to_constraint(model_angle a) {
 switch (a) {
 case neck_j_0:
   return neck_c_0;
 case neck_j_1:
   return neck_c_1;
 case r_shoulder_j_0:
 case l_shoulder_j_0:
   return shoulder_c_0;
 case r_shoulder_j_1:
 case l_shoulder_j_1:
   return shoulder_c_1;
 case r_elbow_j_0:
 case l_elbow_j_0:
   return elbow_c_0;
 case r_wrist_j_0:
 case l_wrist_j_0:
   return wrist_c_0;
 case r_wrist_j_1:
 case l_wrist_j_1:
   return wrist_c_1;
 case r_hip_j_0:
 case l_hip_j_0:
   return hip_c_0;
 case r_hip_j_1:
 case l_hip_j_1:
   return hip_c_1;
 case r_knee_j_0:
 case l_knee_j_0:
   return knee_c_0;
 case r_ankle_j_0:
 case l_ankle_j_0:
   return ankle_c_0;
 case r_ankle_j_1:
 case l_ankle_j_1:
   return ankle_c_1;
 case r_shoulder_j_i:
 case l_shoulder_j_i:
   return shoulder_c_i;
 case r_elbow_j_i:
 case l_elbow_j_i:
   return elbow_c_i;
 case r_hip_j_i:
 case l_hip_j_i:
   return hip_c_i;
 case r_knee_j_i:
 case l_knee_j_i:
   return knee_c_i;
 }
}

void model_set_vector(float* v) {
  int i;
  int n = MODEL_ANGLES + MODEL_SEGMENTS + MODEL_CAMERA;

  for (i = 0; i < n; i++)
    vector[i] = v[i];
}

float* model_get_vector() {
  return vector;
}

void model_set_zero() {
  int i;
  
  // angle parameters
  for (i = 0; i < MODEL_ANGLES; i++) {
    vector[i] = 0;
  }

  //vector[l_wrist_j_0] = 90;

#define CUBIT 2.0

  // FIXME: legs aren't right, so sides aren't right
  // (vitruvian man doesn't mention legs)
  // segment parameters (vitruvian man)
  // head
  vector[head_s_l] = 0.5*CUBIT;

  // shoulder
  vector[shoulder_s_l] = 0.5*CUBIT;

  // upper arm
  vector[u_arm_s_l] = 0.5*CUBIT;

  // lower arm
  vector[l_arm_s_l] = 0.5*CUBIT;

  // hand
  vector[hand_s_l] = 1.0/3.0*CUBIT;

  // side (sqrt((shoulder - 1/2 pelvis)^2 + (height - head - legs)^2) cubits)
  // height = 4 cubits, head = 0.5 cubits
  vector[side_s_l] = sqrt(2.3125);

  // pelvis
  vector[pelvis_s_l] = 0.5*CUBIT;

  // upper leg
  vector[u_leg_s_l] = CUBIT;

  // lower leg
  vector[l_leg_s_l] = CUBIT;

  // foot
  vector[foot_s_l] = 0.5*CUBIT;

  // camera parameters
  i = MODEL_ANGLES + MODEL_SEGMENTS;

  // camera position x,y,z (origin is toe end of right foot)
  vector[i + c_pos_x] = 0.0;
  vector[i + c_pos_y] = 0.5;
  vector[i + c_pos_z] = 4.0;

  // where we are looking x,y,z
  vector[i + c_look_x] = 0.0;
  vector[i + c_look_z] = 0.0;

  // fov
  vector[i + c_fov] = 30.0;

  // scale
  vector[i + c_scale] = 1.0;
}

void model_set_type(model_type t) {
  type = t;
}

void model_to_file(char *fn) {
  int i;
  FILE *out = fopen(fn, "w");

  for (i = 0; i < MODEL_ANGLES + MODEL_SEGMENTS + MODEL_CAMERA; i++) {
    fprintf(out, "%g\n", vector[i]);
  }

  fclose(out);
}

void model_from_file(char *fn) {
  int i;
  FILE *in = fopen(fn, "r");

  for (i = 0; i < MODEL_ANGLES + MODEL_SEGMENTS + MODEL_CAMERA; i++) {
    fscanf(in, "%g\n", vector + i);
  }

  fclose(in);
}
