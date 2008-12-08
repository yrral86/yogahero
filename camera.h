#ifndef __CAMERA_H__
#define __CAMERA_H__

#define WIDTH 640
#define HEIGHT 480

typedef enum {
  c_pos_x,
  c_pos_y,
  c_pos_z,
  c_look_x,
  //  c_look_y,
  c_look_z,
  //  c_up_x,
  //  c_up_y,
  //  c_up_z,
  c_fov,
  c_scale
} cameraParams;

void camera_set_from_vector(float*);

#endif
