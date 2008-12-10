#ifndef __MODEL_H__
#define __MODEL_H__

#define MODEL_ANGLES 30
#define MODEL_SEGMENT_LENGTHS 10
#define MODEL_CAMERA 7
#define MODEL_CONSTRAINTS 16
#define MODEL_SEGMENTS 18

typedef enum {
  cylinder,
  stick
} model_type;

typedef enum {
  neck_j_0,
  neck_j_1,
  r_shoulder_j_0,
  r_shoulder_j_1,
  l_shoulder_j_0,
  l_shoulder_j_1,
  r_elbow_j_0,
  l_elbow_j_0,
  r_wrist_j_0,
  r_wrist_j_1,
  l_wrist_j_0,
  l_wrist_j_1,
  r_hip_j_0,
  r_hip_j_1,
  l_hip_j_0,
  l_hip_j_1,
  r_knee_j_0,
  l_knee_j_0,
  r_ankle_j_0,
  r_ankle_j_1,
  l_ankle_j_0,
  l_ankle_j_1,
  r_shoulder_j_i,
  l_shoulder_j_i,
  r_elbow_j_i,
  l_elbow_j_i,
  r_hip_j_i,
  l_hip_j_i,
  r_knee_j_i,
  l_knee_j_i
} model_angle;

typedef enum {
  neck_c_0,
  neck_c_1,
  shoulder_c_0,
  shoulder_c_1,
  elbow_c_0,
  wrist_c_0,
  wrist_c_1,
  hip_c_0,
  hip_c_1,
  knee_c_0,
  ankle_c_0,
  ankle_c_1,
  shoulder_c_i,
  elbow_c_i,
  hip_c_i,
  knee_c_i
} model_angle_constraint;

typedef enum {
  head_s_l = MODEL_ANGLES,
  shoulder_s_l,
  u_arm_s_l,
  l_arm_s_l,
  hand_s_l,
  side_s_l,
  pelvis_s_l,
  u_leg_s_l,
  l_leg_s_l,
  foot_s_l
} model_segment_length;

typedef enum {
  head_s,
  r_shoulder_s,
  l_shoulder_s,
  r_u_arm_s,
  l_u_arm_s,
  r_l_arm_s,
  l_l_arm_s,
  r_hand_s,
  l_hand_s,
  r_side_s,
  l_side_s,
  pelvis_s,
  r_u_leg_s,
  l_u_leg_s,
  r_l_leg_s,
  l_l_leg_s,
  r_foot_s,
  l_foot_s
} model_segment;

// drawing functions
void model_draw_from_vector();
void model_draw_legs();
void my_gluCylinder(float, model_segment);
void rotate_internal(float);
void rotate_x(float);
void rotate_y(float);
void rotate_z(float);

// sets up constraints and visibility
void model_init();

// constraint functions
float model_get_min(model_angle_constraint);
float model_get_max(model_angle_constraint);
model_angle_constraint model_angle_to_constraint(model_angle);

// visibility functions
void model_set_visible(model_segment);
void model_set_invisible(model_segment);
model_segment_length model_segment_to_length(model_segment);

// vector manipulation functions
void model_set_vector();
float* model_get_vector();
void model_set_zero();

// type functions (cylinder vs. stick model)
void model_set_type(model_type);

// file functions
void model_to_file(char*);
void model_from_file(char*);

#endif
