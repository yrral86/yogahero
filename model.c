#include "camera.h"
#include "model.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>
#include <stdio.h>

static float vector[MODEL_ANGLES + MODEL_SEGMENT_LENGTHS + MODEL_CAMERA];
static float min[MODEL_CONSTRAINTS];
static float max[MODEL_CONSTRAINTS];
static int visible[MODEL_SEGMENTS];

static model_type type;

static float correction_angle;
static float floor_error;


void model_draw_from_vector() {
  float scale;
  float sideangle;

  floor_error = 0.0;

  // calculate the angle between the side segment and vertical
  sideangle = asin((vector[shoulder_s_l] - 0.5*vector[pelvis_s_l])
		   / vector[side_s_l]);

  // convert sideangle to degrees
  sideangle *= 180/M_PI;

  // setup openGL projection matrix
  camera_set_from_vector(vector + MODEL_ANGLES + MODEL_SEGMENT_LENGTHS);
  
  // black background
  glClearColor(0.0, 0.0, 0.0, 0.0);
  // white foreground
  glColor3f(1.0, 1.0, 1.0);
  // clear buffer
  glClear(GL_COLOR_BUFFER_BIT);

  scale = (vector + MODEL_ANGLES + MODEL_SEGMENT_LENGTHS)[c_scale]/8.0;

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
  my_gluCylinder(0.25, l_side_s);

  // push left shoulder
  glPushMatrix();
  
  // draw left upper arm
  rotate_y(sideangle - 90 + vector[l_shoulder_j_0]);
  rotate_x(vector[l_shoulder_j_1]);
  my_gluCylinder(0.25, l_u_arm_s);

  // left shoulder internal rotation
  rotate_internal(-vector[l_shoulder_j_i]);

  // draw left lower arm
  rotate_x(-vector[l_elbow_j_0]);
  my_gluCylinder(0.20, l_l_arm_s);

  // left elbow internal rotation
  rotate_internal(vector[l_elbow_j_i]);

  // draw left hand
  rotate_x(vector[l_wrist_j_0]);
  rotate_y(vector[l_wrist_j_1]);
  my_gluCylinder(0.15, l_hand_s);

  // pop back to left shoulder
  glPopMatrix();

  // draw left shoulder
  rotate_y(sideangle + 90);
  my_gluCylinder(0.25, l_shoulder_s);

  // push base of neck
  glPushMatrix();

  // draw head
  rotate_y(vector[neck_j_1] - 90);
  rotate_x(vector[neck_j_0]);
  my_gluCylinder(0.5, head_s);

  // pop back to base of neck
  glPopMatrix();
  
  // draw right shoulder
  my_gluCylinder(0.25, r_shoulder_s);

  // push right shoulder
  glPushMatrix();

  // draw right upper arm
  rotate_y(-vector[r_shoulder_j_0]);
  rotate_x(vector[r_shoulder_j_1]);
  my_gluCylinder(0.25, r_u_arm_s);

  // right shoulder internal rotation
  rotate_internal(vector[r_shoulder_j_i]);

  // draw right lower arm
  rotate_x(-vector[r_elbow_j_0]);
  my_gluCylinder(0.20, r_l_arm_s);

  // right elbow internal rotation
  rotate_internal(-vector[r_elbow_j_i]);

  // draw right hand
  rotate_x(vector[r_wrist_j_0]);
  rotate_y(-vector[r_wrist_j_1]);
  my_gluCylinder(0.15, r_hand_s);

  // pop back to right shoulder
  glPopMatrix();

  // draw right side
  rotate_y(90 + sideangle);
  my_gluCylinder(0.25, r_side_s);

  glFinish();
}

void model_draw_legs() {
  float modelview[16];

  // draw right foot
  rotate_y(180.0);
  my_gluCylinder(0.25, r_foot_s);

  // draw right lower leg
  rotate_x(-90 + vector[r_ankle_j_0]);
  rotate_z(vector[r_ankle_j_1]);
  my_gluCylinder(0.3, r_l_leg_s);

  // right knee internal rotation
  rotate_internal(vector[r_knee_j_i]);

  // draw right upper leg
  rotate_x(vector[r_knee_j_0]);
  my_gluCylinder(0.4, r_u_leg_s);

  // right hip internal rotation
  rotate_internal(vector[r_hip_j_i]);

  // draw pelvis
  rotate_y(-90 + vector[r_hip_j_0]);
  rotate_z(vector[r_hip_j_1]);
  my_gluCylinder(0.25, pelvis_s);

  // push left hip
  glPushMatrix();

  // draw left upper leg
  rotate_y(-90 + vector[l_hip_j_0]);
  rotate_x(-vector[l_hip_j_1]);
  my_gluCylinder(0.4, l_u_leg_s);

  // left hip internal rotation
  rotate_internal(-vector[l_hip_j_i]);

  // draw left lower leg
  rotate_x(vector[l_knee_j_0]);
  my_gluCylinder(0.3, l_l_leg_s);

  // left knee internal rotation
  rotate_internal(-vector[l_knee_j_i]);

  // draw left foot
  rotate_x(-90 + vector[l_ankle_j_0]);
  rotate_y(vector[l_ankle_j_1]);
  my_gluCylinder(0.25, l_foot_s);

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
void my_gluCylinder(float radius, model_segment s) {
  float modelview[16];
  float yval;
  GLUquadricObj *quad;
  float length = vector[model_segment_to_length(s)];
  if (visible[s]) {
    if (type == stick) {
      glBegin(GL_LINES);
      glVertex3f(0,0,0);
      glVertex3f(0,0,length);
      glEnd();
    } else {
      quad = gluNewQuadric();
      if (type == ellipsoid) {
	// begin draw using ellipsoids
	glPushMatrix();
	glTranslatef(0.0, 0.0, length/2.0);
	glScalef(radius, radius, length/2.0);
	gluSphere(quad, 1.0, 32, 32);
	glPopMatrix();
      } else if (type == cylinder) {
	// begin drawing using cylinders
	gluDisk(quad, 0.0, radius, 32, 32);
	gluCylinder(quad, radius, radius, length, 32, 32);
      } else {
	printf("Invalid model type, please set before drawing\n");
      }
      gluDeleteQuadric(quad);
    }
  }
  
  glTranslatef(0.0, 0.0, length);

  // we are at a new point, so make sure it is above the floor
  glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
  // modelview 12-15 is homogenous coords (x,y,z,w)
  // for (0,0,0) translated to world coords, so y/w = yval
  yval = modelview[13]/modelview[15];

  if (yval < 0)
    floor_error += pow(yval, 8.0);
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

void model_init() {
  int i;

  // set angle constraints
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

  // set everything visible
  for (i = 0; i < MODEL_SEGMENTS; i++) {
    visible[i] = 1;
  }
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

void model_set_visible(model_segment s) {
  visible[s] = 1;
}

void model_set_invisible(model_segment s) {
  visible[s] = 0;
}

model_segment_length model_segment_to_length(model_segment s) {
  switch(s) {
  case head_s:
    return head_s_l;
  case r_shoulder_s:
  case l_shoulder_s:
    return shoulder_s_l;
  case r_u_arm_s:
  case l_u_arm_s:
    return u_arm_s_l;
  case r_l_arm_s:
  case l_l_arm_s:
    return l_arm_s_l;
  case r_hand_s:
  case l_hand_s:
    return hand_s_l;
  case r_side_s:
  case l_side_s:
    return side_s_l;
  case pelvis_s:
    return pelvis_s_l;
  case r_u_leg_s:
  case l_u_leg_s:
    return u_leg_s_l;
  case r_l_leg_s:
  case l_l_leg_s:
    return l_leg_s_l;
  case r_foot_s:
  case l_foot_s:
    return foot_s_l;
  }
}

void model_set_vector(float* v) {
  int i;
  int n = MODEL_ANGLES + MODEL_SEGMENT_LENGTHS + MODEL_CAMERA;

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

  // side (sqrt((shoulder - 1/2 pelvis)^2 + (height - head - legs)^2))
  // height = 4 cubits (8 units), head = 0.5 cubits (1 unit)
  // shoulder = 0.5 cubits (1 unit), pelvis = 0.5 cubits (1 unit)
  // upper leg = 1 cubit (2 units), lower leg = 1 cubit (2 units)
  // sqrt((1 - 0.5)^2 + (8 - 1 - 4)^2)
  vector[side_s_l] = sqrt(9.25);

  // pelvis
  vector[pelvis_s_l] = 0.5*CUBIT;

  // upper leg
  vector[u_leg_s_l] = CUBIT;

  // lower leg
  vector[l_leg_s_l] = CUBIT;

  // foot
  vector[foot_s_l] = 0.5*CUBIT;

  // camera parameters
  i = MODEL_ANGLES + MODEL_SEGMENT_LENGTHS;

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

  for (i = 0; i < MODEL_ANGLES + MODEL_SEGMENT_LENGTHS + MODEL_CAMERA; i++) {
    fprintf(out, "%g\n", vector[i]);
  }

  fclose(out);
}

void model_from_file(char *fn) {
  int i;
  FILE *in = fopen(fn, "r");

  for (i = 0; i < MODEL_ANGLES + MODEL_SEGMENT_LENGTHS + MODEL_CAMERA; i++) {
    fscanf(in, "%g\n", vector + i);
  }

  fclose(in);
}

float model_get_floor_error() {
  return floor_error;
}
