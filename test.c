#include <glib.h>
#include <glib/gprintf.h>
#include <gio/gio.h>

typedef enum {
  ALIGN_ONLY,
  ALIGN_ONLY_SCALE,
  BASE,
  ANGLE_1,
  FLOOR_1,
  ANGLE_10,
  FLOOR_10,
  ANGLE_100,
  FLOOR_100,
  ANGLE_1000,
  FLOOR_1000,
  ANGLE_10000,
  FLOOR_10000,
  ALL
} TEST_TYPE;

void run_match(char*, char*, TEST_TYPE);
void move_image(char*, TEST_TYPE);
void move_data(char*, TEST_TYPE);
void move_pose(char*, TEST_TYPE);
void move_file(char*, char*, char*, TEST_TYPE);
/*
void system(char *command) {
  printf("%s\n", command);
}
*/
int main (int argc, char **argv) {
  GString *newFn;
  GString *command;
  char imagefn[100], posefn[100];
  gchar *basename;
  int i;

  if (argc != 3) {
    g_print("Usage: ./test filename posefile\n");
    exit(0);
  }
  
  strcpy(imagefn, argv[1]);
  strcpy(posefn, argv[2]);

  // create output directory
  basename = g_path_get_basename(imagefn);
  newFn = g_string_new(basename);
  g_free(basename);
  command = g_string_new("mkdir -p images/output/");

  // shave off .png
  g_string_truncate(newFn, newFn->len - 4);
  g_string_append(command, newFn->str);

  // mkdir
  system(command->str);

  g_string_free(command, TRUE);

  // copy original file to output dir for comparison
  command = g_string_new("cp ");
  g_string_append(command, imagefn);
  g_string_append(command, " images/output/");
  g_string_append(command, newFn->str);

  // copy
  system(command->str);

  g_string_free(command, TRUE);
  g_string_free(newFn, TRUE);

  for (i = 0; i <= ALL; i++)
    run_match(imagefn, posefn, i);

  return 0;
}


void run_match(char *imagefn, char *posefn, TEST_TYPE type) {
  gchar command[100];

  switch (type) {
  case ALIGN_ONLY:
    g_sprintf(command, "./findmatch %s %s -n", imagefn, posefn);
    g_print("align only\n");
    break;
  case ALIGN_ONLY_SCALE:
    g_sprintf(command, "./findmatch %s %s -n -s", imagefn, posefn);
    g_print("align only w/scale\n");
    break;
  case BASE:
    g_sprintf(command, "./findmatch %s %s", imagefn, posefn);
    g_print("base\n");
    break;
  case ANGLE_1:
    g_sprintf(command, "./findmatch %s %s -a 1", imagefn, posefn);
    g_print("angle 1\n");
    break;
  case FLOOR_1:
    g_sprintf(command, "./findmatch %s %s -f 1", imagefn, posefn);
    g_print("floor 1\n");
    break;
  case ANGLE_10:
    g_sprintf(command, "./findmatch %s %s -a 10", imagefn, posefn);
    g_print("angle 10\n");
    break;
  case FLOOR_10:
    g_sprintf(command, "./findmatch %s %s -f 10", imagefn, posefn);
    g_print("floor 10\n");
    break;
  case ANGLE_100:
    g_sprintf(command, "./findmatch %s %s -a 100", imagefn, posefn);
    g_print("angle 100\n");
    break;
  case FLOOR_100:
    g_sprintf(command, "./findmatch %s %s -f 100", imagefn, posefn);
    g_print("floor 100\n");
    break;
  case ANGLE_1000:
    g_sprintf(command, "./findmatch %s %s -a 1000", imagefn, posefn);
    g_print("angle 1000\n");
    break;
  case FLOOR_1000:
    g_sprintf(command, "./findmatch %s %s -f 1000", imagefn, posefn);
    g_print("floor 1000\n");
    break;
  case ANGLE_10000:
    g_sprintf(command, "./findmatch %s %s -a 10000", imagefn, posefn);
    g_print("angle 10000\n");
    break;
  case FLOOR_10000:
    g_sprintf(command, "./findmatch %s %s -f 10000", imagefn, posefn);
    g_print("floor 10000\n");
    break;
  case ALL:
    g_sprintf(command, "./findmatch %s %s -a 1000 -f 1000", imagefn, posefn);
    g_print("all\n");
    break;
  }

  system(command);

  move_image(imagefn, type);
  move_data(imagefn, type);
  move_pose(imagefn, type);
}

void move_image(char *imagefn, TEST_TYPE type) {
  move_file(imagefn, "match.png", ".png", type);
}

void move_data(char *imagefn, TEST_TYPE type) {
  move_file(imagefn, "match.data", ".data", type);
}

void move_pose(char *imagefn, TEST_TYPE type) {
  move_file(imagefn, "match.pose", ".pose", type);
}

void move_file(char *imagefn, char *origin, char *suffix, TEST_TYPE type) {
  GString *newFn;
  GString *command;
  gchar *basename;

  basename = g_path_get_basename(imagefn);
  newFn = g_string_new(basename);
  g_free(basename);
  command = g_string_new("mv ");
  g_string_append(command, origin);
  g_string_append(command, " images/output/");

  // shave off .png
  g_string_truncate(newFn, newFn->len - 4);

  g_string_append(command, newFn->str);

  switch (type) {
  case ALIGN_ONLY:
    g_string_append(newFn, "-ALIGN");
    break;
  case ALIGN_ONLY_SCALE:
    g_string_append(newFn, "-ALIGNSCALE");
    break;
  case BASE:
    g_string_append(newFn, "-BASE");
    break;
  case ANGLE_1:
    g_string_append(newFn, "-ANGLE1");
    break;
  case FLOOR_1:
    g_string_append(newFn, "-FLOOR1");
    break;
  case ANGLE_10:
    g_string_append(newFn, "-ANGLE10");
    break;
  case FLOOR_10:
    g_string_append(newFn, "-FLOOR10");
    break;
  case ANGLE_100:
    g_string_append(newFn, "-ANGLE100");
    break;
  case FLOOR_100:
    g_string_append(newFn, "-FLOOR100");
    break;
  case ANGLE_1000:
    g_string_append(newFn, "-ANGLE1000");
    break;
  case FLOOR_1000:
    g_string_append(newFn, "-FLOOR1000");
    break;
  case ANGLE_10000:
    g_string_append(newFn, "-ANGLE10000");
    break;
  case FLOOR_10000:
    g_string_append(newFn, "-FLOOR10000");
    break;
  case ALL:
    g_string_append(newFn, "-ALL");
    break;
  }

  g_string_append(newFn, suffix);

  g_string_append(command, "/");
  g_string_append(command, newFn->str);

  system(command->str);

  g_string_free(command, TRUE);
  g_string_free(newFn, TRUE);
}
