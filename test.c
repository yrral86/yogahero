#include <glib.h>
#include <glib/gprintf.h>
#include <gio/gio.h>

typedef enum {
  BASE,
  ANGLE,
  FLOOR,
  ALL
} TEST_TYPE;

void run_match(char*, char*, TEST_TYPE);
void move_image(char*, TEST_TYPE);
void move_data(char*, TEST_TYPE);
void move_pose(char*, TEST_TYPE);
/*
void system(char *command) {
  printf("%s\n", command);
}
*/
int main (int argc, char **argv) {
  GString *newFn;
  GString *command;
  char imagefn[100], posefn[100];
  if (argc != 3) {
    g_print("Usage: ./test filename posefile\n");
    exit(0);
  }
  
  strcpy(imagefn, argv[1]);
  strcpy(posefn, argv[2]);

  //  gtk_init(&argc, &argv);

  newFn = g_string_new(g_path_get_basename(imagefn));;
  command = g_string_new("mkdir -p images/output/");

  // shave off .png
  g_string_truncate(newFn, newFn->len - 4);
  g_string_append(command, newFn->str);

  // mkdir
  system(command->str);

  // test pure powell's
  run_match(imagefn, posefn, BASE);

  // test angle constraints + powell's
  run_match(imagefn, posefn, ANGLE);
  
  // test floor + powell's
  run_match(imagefn, posefn, FLOOR);

  // test all 3 + powell's
  run_match(imagefn, posefn, ALL);


  return 0;
}


void run_match(char *imagefn, char *posefn, TEST_TYPE type) {
  gchar command[100];

  switch (type) {
  case BASE:
    g_sprintf(command, "./findmatch %s %s", imagefn, posefn);
    g_print("base\n");
    break;
  case ANGLE:
    g_sprintf(command, "./findmatch %s %s -a 1000", imagefn, posefn);
    g_print("angle\n");
    break;
  case FLOOR:
    g_sprintf(command, "./findmatch %s %s -f 1000", imagefn, posefn);
    g_print("floor\n");
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
  GString *newFn;
  GString *command;

  newFn = g_string_new(g_path_get_basename(imagefn));;
  command = g_string_new("mv pose.png images/output/");

  // shave off .png
  g_string_truncate(newFn, newFn->len - 4);

  g_string_append(command, newFn->str);

  switch (type) {
  case BASE:
    g_string_append(newFn, "-BASE");
    break;
  case ANGLE:
    g_string_append(newFn, "-ANGLE");
    break;
  case FLOOR:
    g_string_append(newFn, "-FLOOR");
    break;
  case ALL:
    g_string_append(newFn, "-ALL");
    break;
  }

  g_string_append(newFn, ".png");

  g_string_append(command, "/");
  g_string_append(command, newFn->str);

  system(command->str);

}

void move_data(char *imagefn, TEST_TYPE type) {
  GString *newFn;
  GString *command;

  newFn = g_string_new(g_path_get_basename(imagefn));;
  command = g_string_new("mv match.data images/output/");

  // shave off .png
  g_string_truncate(newFn, newFn->len - 4);

  g_string_append(command, newFn->str);

  switch (type) {
  case BASE:
    g_string_append(newFn, "-BASE");
    break;
  case ANGLE:
    g_string_append(newFn, "-ANGLE");
    break;
  case FLOOR:
    g_string_append(newFn, "-FLOOR");
    break;
  case ALL:
    g_string_append(newFn, "-ALL");
    break;
  }

  g_string_append(newFn, ".data");

  g_string_append(command, "/");
  g_string_append(command, newFn->str);

  system(command->str);

}

void move_pose(char *imagefn, TEST_TYPE type) {
  GString *newFn;
  GString *command;

  newFn = g_string_new(g_path_get_basename(imagefn));;
  command = g_string_new("mv pose.out images/output/");

  // shave off .png
  g_string_truncate(newFn, newFn->len - 4);

  g_string_append(command, newFn->str);

  switch (type) {
  case BASE:
    g_string_append(newFn, "-BASE");
    break;
  case ANGLE:
    g_string_append(newFn, "-ANGLE");
    break;
  case FLOOR:
    g_string_append(newFn, "-FLOOR");
    break;
  case ALL:
    g_string_append(newFn, "-ALL");
    break;
  }

  g_string_append(newFn, ".pose");

  g_string_append(command, "/");
  g_string_append(command, newFn->str);

  system(command->str);

}
