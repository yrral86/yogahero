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
  ALL,
  ANGLE_5000,
  ANGLE_15000,
  ANGLE_20000,
  ANGLE_25000,
  ANGLE_50000,
  FLOOR_500,
  FLOOR_1500,
  FLOOR_2000,
  FLOOR_2500,
  FLOOR_3000,
  FLOOR_3500,
  FLOOR_4000,
  ALL_10_15,
  ALL_10_20,
  ALL_10_25,
  ALL_10_30,
  ALL_15_15,
  ALL_15_20,
  ALL_15_25,
  ALL_15_30,
  ALL_20_15,
  ALL_20_20,
  ALL_20_25,
  ALL_20_30,
  ALL_25_15,
  ALL_25_20,
  ALL_25_25,
  ALL_25_30,
  ALL_10_35,
  ALL_15_35,
  ALL_20_35,
  ALL_25_35,
  ALL_20_5,
  ALL_20_5_S,
  ALL_10_25_S
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

  /* // first round of tests
  for (i = 0; i <= ALL; i++)
    run_match(imagefn, posefn, i);
  */

  /*  // second round of tests
  for (i = ANGLE_5000; i <= ALL_25_30; i++)
    run_match(imagefn, posefn, i);
  */

  /*  // third round
  for (i = ALL_10_35; i <= ALL_25_35; i++)
    run_match(imagefn, posefn, i);
  */

  // fourth round
  for (i = ALL_20_5; i <= ALL_10_25_S; i++)
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
  case ANGLE_5000:
    g_sprintf(command, "./findmatch %s %s -a 5000", imagefn, posefn);
    g_print("angle 5000\n");
    break;
   case ANGLE_15000:
    g_sprintf(command, "./findmatch %s %s -a 15000", imagefn, posefn);
    g_print("angle 15000\n");
    break;
  case ANGLE_20000:
    g_sprintf(command, "./findmatch %s %s -a 20000", imagefn, posefn);
    g_print("angle 20000\n");
    break;
  case ANGLE_25000:
    g_sprintf(command, "./findmatch %s %s -a 25000", imagefn, posefn);
    g_print("angle 25000\n");
    break;
  case ANGLE_50000:
    g_sprintf(command, "./findmatch %s %s -a 50000", imagefn, posefn);
    g_print("angle 50000\n");
    break;
  case FLOOR_500:
    g_sprintf(command, "./findmatch %s %s -f 500", imagefn, posefn);
    g_print("floor 500\n");
    break;
   case FLOOR_1500:
    g_sprintf(command, "./findmatch %s %s -f 1500", imagefn, posefn);
    g_print("floor 1500\n");
    break;
  case FLOOR_2000:
    g_sprintf(command, "./findmatch %s %s -f 2000", imagefn, posefn);
    g_print("floor 2000\n");
    break;
  case FLOOR_2500:
    g_sprintf(command, "./findmatch %s %s -f 2500", imagefn, posefn);
    g_print("floor 2500\n");
    break;
  case FLOOR_3000:
    g_sprintf(command, "./findmatch %s %s -f 3000", imagefn, posefn);
    g_print("floor 3000\n");
    break;
  case FLOOR_3500:
    g_sprintf(command, "./findmatch %s %s -f 3500", imagefn, posefn);
    g_print("floor 3500\n");
    break;
  case FLOOR_4000:
    g_sprintf(command, "./findmatch %s %s -f 4000", imagefn, posefn);
    g_print("floor 4000\n");
    break;
  case ALL_10_15:
    g_sprintf(command, "./findmatch %s %s -a 10000 -f 1500", imagefn, posefn);
    g_print("all 10000x1500\n");
    break;
  case ALL_10_20:
    g_sprintf(command, "./findmatch %s %s -a 10000 -f 2000", imagefn, posefn);
    g_print("all 10000x2000\n");
    break;
  case ALL_10_25:
    g_sprintf(command, "./findmatch %s %s -a 10000 -f 2500", imagefn, posefn);
    g_print("all 10000x2500\n");
    break;
  case ALL_10_30:
    g_sprintf(command, "./findmatch %s %s -a 10000 -f 3000", imagefn, posefn);
    g_print("all 10000x3000\n");
    break;
  case ALL_15_15:
    g_sprintf(command, "./findmatch %s %s -a 15000 -f 1500", imagefn, posefn);
    g_print("all 15000x1500\n");
    break;
  case ALL_15_20:
    g_sprintf(command, "./findmatch %s %s -a 15000 -f 2000", imagefn, posefn);
    g_print("all 15000x2000\n");
    break;
  case ALL_15_25:
    g_sprintf(command, "./findmatch %s %s -a 15000 -f 2500", imagefn, posefn);
    g_print("all 15000x2500\n");
    break;
  case ALL_15_30:
    g_sprintf(command, "./findmatch %s %s -a 15000 -f 3000", imagefn, posefn);
    g_print("all 15000x3000\n");
    break;
  case ALL_20_15:
    g_sprintf(command, "./findmatch %s %s -a 20000 -f 1500", imagefn, posefn);
    g_print("all 20000x1500\n");
    break;
  case ALL_20_20:
    g_sprintf(command, "./findmatch %s %s -a 20000 -f 2000", imagefn, posefn);
    g_print("all 20000x2000\n");
    break;
  case ALL_20_25:
    g_sprintf(command, "./findmatch %s %s -a 20000 -f 2500", imagefn, posefn);
    g_print("all 20000x2500\n");
    break;
  case ALL_20_30:
    g_sprintf(command, "./findmatch %s %s -a 20000 -f 3000", imagefn, posefn);
    g_print("all 20000x3000\n");
    break;
  case ALL_25_15:
    g_sprintf(command, "./findmatch %s %s -a 25000 -f 1500", imagefn, posefn);
    g_print("all 25000x1500\n");
    break;
  case ALL_25_20:
    g_sprintf(command, "./findmatch %s %s -a 25000 -f 2000", imagefn, posefn);
    g_print("all 25000x2000\n");
    break;
  case ALL_25_25:
    g_sprintf(command, "./findmatch %s %s -a 25000 -f 2500", imagefn, posefn);
    g_print("all 25000x2500\n");
    break;
  case ALL_25_30:
    g_sprintf(command, "./findmatch %s %s -a 25000 -f 3000", imagefn, posefn);
    g_print("all 25000x3000\n");
    break;
  case ALL_10_35:
    g_sprintf(command, "./findmatch %s %s -a 10000 -f 3500", imagefn, posefn);
    g_print("all 10000x3500\n");
    break;
  case ALL_15_35:
    g_sprintf(command, "./findmatch %s %s -a 15000 -f 3500", imagefn, posefn);
    g_print("all 15000x3500\n");
    break;
  case ALL_20_35:
    g_sprintf(command, "./findmatch %s %s -a 20000 -f 3500", imagefn, posefn);
    g_print("all 20000x3500\n");
    break;
  case ALL_25_35:
    g_sprintf(command, "./findmatch %s %s -a 25000 -f 3500", imagefn, posefn);
    g_print("all 25000x3500\n");
    break;
  case ALL_20_5:
    g_sprintf(command, "./findmatch %s %s -a 20000 -f 500", imagefn, posefn);
    g_print("all 20000x500\n");
    break;
  case ALL_20_5_S:
    g_sprintf(command, "./findmatch %s %s -a 25000 -f 500 -s", imagefn, posefn);
    g_print("all 20000x500 w/scale\n");
    break;
  case ALL_10_25_S:
    g_sprintf(command, "./findmatch %s %s -a 10000 -f 2500", imagefn, posefn);
    g_print("all 10000x2500\n");
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
  case ANGLE_5000:
    g_string_append(newFn, "-ANGLE5000");
    break;
  case ANGLE_15000:
    g_string_append(newFn, "-ANGLE15000");
    break;
  case ANGLE_20000:
    g_string_append(newFn, "-ANGLE20000");
    break;
  case ANGLE_25000:
    g_string_append(newFn, "-ANGLE25000");
    break;
  case ANGLE_50000:
    g_string_append(newFn, "-ANGLE50000");
    break;
  case FLOOR_500:
    g_string_append(newFn, "-FLOOR500");
    break;
  case FLOOR_1500:
    g_string_append(newFn, "-FLOOR1500");
    break;
  case FLOOR_2000:
    g_string_append(newFn, "-FLOOR2000");
    break;
  case FLOOR_2500:
    g_string_append(newFn, "-FLOOR2500");
    break;
  case FLOOR_3000:
    g_string_append(newFn, "-FLOOR3000");
    break;
  case FLOOR_3500:
    g_string_append(newFn, "-FLOOR3500");
    break;
  case FLOOR_4000:
    g_string_append(newFn, "-FLOOR4000");
    break;
  case ALL_10_15:
    g_string_append(newFn, "-ALL1015");
    break;
  case ALL_10_20:
    g_string_append(newFn, "-ALL1020");
    break;
  case ALL_10_25:
    g_string_append(newFn, "-ALL1025");
    break;
  case ALL_10_30:
    g_string_append(newFn, "-ALL1030");
    break;
  case ALL_15_15:
    g_string_append(newFn, "-ALL1515");
    break;
  case ALL_15_20:
    g_string_append(newFn, "-ALL1520");
    break;
  case ALL_15_25:
    g_string_append(newFn, "-ALL1525");
    break;
  case ALL_15_30:
    g_string_append(newFn, "-ALL1530");
    break;
  case ALL_20_15:
    g_string_append(newFn, "-ALL2015");
    break;
  case ALL_20_20:
    g_string_append(newFn, "-ALL2020");
    break;
  case ALL_20_25:
    g_string_append(newFn, "-ALL2025");
    break;
  case ALL_20_30:
    g_string_append(newFn, "-ALL2030");
    break;
  case ALL_25_15:
    g_string_append(newFn, "-ALL2515");
    break;
  case ALL_25_20:
    g_string_append(newFn, "-ALL2520");
    break;
  case ALL_25_25:
    g_string_append(newFn, "-ALL2525");
    break;
  case ALL_25_30:
    g_string_append(newFn, "-ALL2530");
    break;
  case ALL_10_35:
    g_string_append(newFn, "-ALL1035");
    break;
  case ALL_15_35:
    g_string_append(newFn, "-ALL1535");
    break;
  case ALL_20_35:
    g_string_append(newFn, "-ALL2035");
    break;
  case ALL_25_35:
    g_string_append(newFn, "-ALL2535");
    break;
  case ALL_20_5:
    g_string_append(newFn, "-ALL205");
    break;
  case ALL_20_5_S:
    g_string_append(newFn, "-ALL205S");
    break;
  case ALL_10_25_S:
    g_string_append(newFn, "-ALL1025S");
    break;

  }

  g_string_append(newFn, suffix);

  g_string_append(command, "/");
  g_string_append(command, newFn->str);

  system(command->str);

  g_string_free(command, TRUE);
  g_string_free(newFn, TRUE);
}
