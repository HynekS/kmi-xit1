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
  float step_distance = (next - current) / steps;
  float midpoint = (current + (step_distance * step));
  return midpoint;
}

point *reduce_lines(point **points, int line_count, int steps) {
  if (line_count == 1)
    return points[0];

  point **midpoints = (point **)malloc((line_count - 1) * sizeof(point *));
  if (midpoints == NULL) {
    fprintf(stderr, "Error: Failed to allocate memory for midpoints.\n");
    exit(1);
  }

  for (int i = 0; i < line_count - 1; i++) {
    midpoints[i] = (point *)malloc(steps * sizeof(point));

    for (int step = 0; step < steps; step++) {
      float x_current = points[i][step].x;
      float y_current = points[i][step].y;

      float x_next = points[i + 1][step].x;
      float y_next = points[i + 1][step].y;

      midpoints[i][step].x =
          get_midpoint(x_current, x_next, steps, step);
      midpoints[i][step].y =
          get_midpoint(y_current, y_next, steps, step);
    }
  }

  // free points?

  return reduce_lines(midpoints, line_count - 1, steps);
}

point** create_steps(point *points, int points_count, int steps) {
  point **midpoints = (point **)malloc((points_count - 1) * sizeof(point *));
  if (midpoints == NULL) {
    fprintf(stderr, "Error: Failed to allocate memory for midpoints.\n");
    exit(1);
  }

  for (int i = 0; i < points_count - 1; i++) {
    midpoints[i] = malloc(steps * sizeof(point));

    int x_current = points[i].x;
    int y_current = points[i].y;

    int x_next = points[i + 1].x;
    int y_next = points[i + 1].y;

    for (int step = 0; step < steps; step++) {
      midpoints[i][step].x =
          get_midpoint(x_current, x_next, steps, step);
      midpoints[i][step].y =
          get_midpoint(y_current, y_next, steps, step);
    }
  }

  return midpoints;
}

int main() {
  PGMImage *blank_canvas = create_blank_image(256, 256);

  int const points_count = 5;
  int const steps = 64;
  point points[points_count] = {
      {0.0, 0.0}, {32.0, 120.0}, {128.0, 160.0}, {164.0, 16.0}, {256.0, 256.0}};

  point** first_iteration = create_steps(points, points_count, steps);

  point* points_to_plot = reduce_lines(first_iteration, points_count - 1, steps);

  for(int k = 0; k < steps; k++) {
    setPixel((int)round(points_to_plot[k].x), (int)round(points_to_plot[k].y), blank_canvas);
  }

  for (int i = 0; i < points_count - 1; i++) {
    for(int j = 0; j < steps; j++) {
      setPixel((int)round(first_iteration[i][j].x), (int)round(first_iteration[i][j].y), blank_canvas);
    }
    free(first_iteration[i]);
  }

  free(first_iteration);
  free(points_to_plot);

  pgma_write("bezier.pgm", "Output bezier curve drawing", blank_canvas->xsize,
             blank_canvas->ysize, blank_canvas->maxg, blank_canvas->g);

  free(blank_canvas->g);
  free(blank_canvas);
}