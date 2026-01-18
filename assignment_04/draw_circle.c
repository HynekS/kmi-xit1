#include "pgma_io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  int *g;
  int maxg;
  int xsize;
  int ysize;
} PGMImage;

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
  if (x < 0 || x >= canvas->xsize || y < 0 || y >= canvas->ysize) return;
  canvas->g[canvas->xsize * y + x] = 0;
}

void draw_circle(int x0, int y0, int radius, PGMImage *canvas) {
  int f = 1 - radius;
  int ddF_x = 1;
  int ddF_y = -2 * radius;
  int x = 0;
  int y = radius;

  setPixel(x0, y0 + radius, canvas);
  setPixel(x0, y0 - radius, canvas);
  setPixel(x0 + radius, y0, canvas);
  setPixel(x0 - radius, y0, canvas);
  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
    setPixel(x0 + x, y0 + y, canvas);
    setPixel(x0 - x, y0 + y, canvas);
    setPixel(x0 + x, y0 - y, canvas);
    setPixel(x0 - x, y0 - y, canvas);
    setPixel(x0 + y, y0 + x, canvas);
    setPixel(x0 - y, y0 + x, canvas);
    setPixel(x0 + y, y0 - x, canvas);
    setPixel(x0 - y, y0 - x, canvas);
  }
}

int main() {
  PGMImage *blank_canvas = create_blank_image(256, 256);

  draw_circle(64, 64, 172, blank_canvas);

  pgma_write("circle.pgm", "Output circle drawing", blank_canvas->xsize,
             blank_canvas->ysize, blank_canvas->maxg, blank_canvas->g);

  free(blank_canvas->g);
  free(blank_canvas);
}