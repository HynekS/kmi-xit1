#include "pgma_io.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  int *g;
  int maxg;
  int xsize;
  int ysize;
} PGMImage;

typedef struct {
  float x;
  float y;
} point;

PGMImage *create_blank_image(int xsize, int ysize) {
  PGMImage *image_data = (PGMImage *)malloc(sizeof(PGMImage));
  if (image_data == NULL) {
    fprintf(stderr, "Error: Failed to allocate memory for PGMImage struct.\n");
    exit(1);
  }

  image_data->xsize = xsize;
  image_data->ysize = ysize;
  image_data->maxg = 255;

  image_data->g = (int *)malloc(xsize * ysize * sizeof(int));
  if (image_data->g == NULL) {
    fprintf(stderr, "Error: Failed to allocate memory for pixel data.\n");
    free(image_data);
    exit(1);
  }
  for (int i = 0; i < xsize * ysize; i++) {
    image_data->g[i] = 255;
  }
  return image_data;
}

void setPixel(int x, int y, PGMImage *canvas) {
  if (x < 0 || x >= canvas->xsize || y < 0 || y >= canvas->ysize)
    return;
  canvas->g[canvas->xsize * y + x] = 0;
}

float get_midpoint(float current, float next, int steps, int step) {
  float step_distance = (next - current) / (float)steps;
  float midpoint = (current + (step_distance * step));
  return midpoint;
}

point *reduce_lines(point **points, int line_count, int steps) {
  if (line_count == 1)
    return points[0];

  point **midpoints = (point **)malloc((line_count - 1) * steps * sizeof(point *));
  if (midpoints == NULL) {
    fprintf(stderr, "Error: Failed to allocate memory for midpoints.\n");
    exit(1);
  }

  for (int i = 0; i < line_count - 1; i++) {
    // midpoints[i] = (point *)malloc(steps * sizeof(point));

    for (int step = 0; step < steps; step++) {
      int x_current = points[i][step].x;
      int y_current = points[i][step].y;

      int x_next = points[i + 1][step].x;
      int y_next = points[i + 1][step].y;

      midpoints[i][step].x =
          get_midpoint((float)x_current, (float)x_next, steps, step);
      midpoints[i][step].y =
          get_midpoint((float)y_current, (float)y_next, steps, step);
    }
  }

  return reduce_lines(midpoints, line_count - 1, steps);
}

point** create_steps(point *points, int points_count, int steps) {
  point **midpoints = (point **)malloc((points_count - 1) * steps * sizeof(point *));
  if (midpoints == NULL) {
    fprintf(stderr, "Error: Failed to allocate memory for midpoints.\n");
    exit(1);
  }

  for (int i = 0; i < points_count - 1; i++) {
    int x_current = points[i].x;
    int y_current = points[i].y;

    int x_next = points[i + 1].x;
    int y_next = points[i + 1].y;

    for (int step = 0; step < steps; step++) {
      midpoints[i][step].x =
          get_midpoint((float)x_current, (float)x_next, steps, step);
      midpoints[i][step].y =
          get_midpoint((float)y_current, (float)y_next, steps, step);
    }
  }

  return midpoints;
}

// void draw_bezier(int (*points)[2], int points_count, int steps, PGMImage
// *canvas) {
void draw_bezier(point *points, int points_count, int steps, PGMImage *canvas) {
  // float midpoints[points_count - 1][steps][2];
  point midpoints[points_count - 1][steps];

  for (int i = 0; i < points_count - 1; i++) {
    // int x_current = points[i][0];
    // int y_current = points[i][1];
    //
    // int x_next = points[i + 1][0];
    // int y_next = points[i + 1][1];

    int x_current = points[i].x;
    int y_current = points[i].y;

    int x_next = points[i + 1].x;
    int y_next = points[i + 1].y;

    for (int step = 0; step < steps; step++) {
      midpoints[i][step].x =
          get_midpoint((float)x_current, (float)x_next, steps, step);
      midpoints[i][step].y =
          get_midpoint((float)y_current, (float)y_next, steps, step);
    }
  }

  for (int line_count = 0; line_count < points_count - 2; line_count++) {
    printf("%i:\n", line_count);

    for (int step_increment = 0; step_increment < steps; step_increment++) {
      setPixel(get_midpoint(midpoints[line_count][step_increment].x,
                            midpoints[line_count + 1][step_increment].x, steps,
                            step_increment),
               get_midpoint(midpoints[line_count][step_increment].y,
                            midpoints[line_count + 1][step_increment].y, steps,
                            step_increment),
               canvas);
    }
  }
}

int main() {
  PGMImage *blank_canvas = create_blank_image(256, 256);

  // int points[5][2] = {{1, 1}, {32, 120}, {128, 160}, {164, 16}, {256, 0}};
  int const points_count = 5;
  int const steps = 32;
  point points[points_count] = {
      {1.0, 1.0}, {32.0, 120.0}, {128.0, 160.0}, {164.0, 16.0}, {256.0, 0.0}};

  // draw_bezier(points, sizeof(points) / sizeof(points[0]), 48, blank_canvas);
  // draw_bezier(points, sizeof(points) / sizeof(points[0]), 48, blank_canvas);
  point** foo = create_steps(points, points_count, steps);

  point* bar = reduce_lines(foo, points_count - 1, steps);


  pgma_write("bezier.pgm", "Output bezier curve drawing", blank_canvas->xsize,
             blank_canvas->ysize, blank_canvas->maxg, blank_canvas->g);

  free(blank_canvas->g);
  free(blank_canvas);
}