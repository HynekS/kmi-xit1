// gcc edit.c pgma_io.c -o edit
#include <math.h>
#include <stdlib.h>

#include "pgma_io.h"

typedef struct {
  int *g;
  int maxg;
  int xsize;
  int ysize;
} PGMImage;

PGMImage *pgma_read_to_memory(char *file_name) {
  int *g;
  int maxg;
  int xsize;
  int ysize;

  g = NULL;
  /*
    Read the data.
  */
  pgma_read(file_name, &xsize, &ysize, &maxg, &g);

  fprintf(stdout, "\n");
  fprintf(stdout, "TEST_PGMA_READ:\n");
  fprintf(stdout, "  PGMA_READ was able to read \"%s\".\n", file_name);
  /*
    Check the data.
  */
  pgma_check_data(xsize, ysize, maxg, g);

  fprintf(stdout, "\n");
  fprintf(stdout, "TEST_PGMA_READ:\n");
  fprintf(stdout, "  PGMA_CHECK_DATA has approved the data from the file.\n");

  PGMImage *image_data = (PGMImage *)malloc(sizeof(PGMImage));
  if (image_data == NULL) {
    fprintf(stderr, "Error: Failed to allocate memory for PGMImage struct.\n");
    exit(1);
  }
  image_data->xsize = xsize;
  image_data->ysize = ysize;
  image_data->maxg = maxg;
  image_data->g = g;

  return image_data;
}

PGMImage *histogram_balance(PGMImage *img) {
  int frequencies[img->maxg + 1];
  for (int i = 0; i <= img->maxg; i++) {
    frequencies[i] = 0;
  }

  int total_pixels = img->xsize * img->ysize;
  for (int i = 0; i < total_pixels; i++) {
    int val = img->g[i];
    frequencies[val]++;
  }

  int cumulative_frequencies[img->maxg + 1];
  cumulative_frequencies[0] = frequencies[0];
  for (int i = 1; i <= img->maxg; i++) {
    cumulative_frequencies[i] = cumulative_frequencies[i - 1] + frequencies[i];
  }

  int *g = (int *)malloc(total_pixels * sizeof(int));
  if (!g)
    return NULL;

  for (int i = 0; i < total_pixels; i++) {
    int orig_val = img->g[i];
    int min_val = cumulative_frequencies[0];
    int balanced_val =
        ((cumulative_frequencies[orig_val] - min_val) * img->maxg) /
        (total_pixels - min_val);
    g[i] = balanced_val;
  }

  PGMImage *image_data = (PGMImage *)malloc(sizeof(PGMImage));
  if (!image_data) {
    free(g);
    return NULL;
  }

  image_data->xsize = img->xsize;
  image_data->ysize = img->ysize;
  image_data->maxg = img->maxg;
  image_data->g = g;

  return image_data;
}

// amount should be 0 to 1
PGMImage *change_contrast(PGMImage *img, float amount) {
  float normalized_amount = amount < 0 ? 0 : amount > 1 ? 1 : amount;
  float mid_val = img->maxg / 2.0;

  int total_pixels = img->xsize * img->ysize;

  int *g = (int *)malloc(total_pixels * sizeof(int));
  if (!g)
    return NULL;

  for (int i = 0; i < total_pixels; i++) {
    int val = img->g[i];
    float delta = (val - mid_val) * normalized_amount;
    int new_value = (int)roundf(val + delta);
    int new_value_normalized = new_value < 0           ? 0
                               : new_value > img->maxg ? img->maxg
                                                       : new_value;
    g[i] = new_value_normalized;
  }

  PGMImage *image_data = (PGMImage *)malloc(sizeof(PGMImage));
  if (!image_data) {
    free(g);
    return NULL;
  }

  image_data->xsize = img->xsize;
  image_data->ysize = img->ysize;
  image_data->maxg = img->maxg;
  image_data->g = g;

  return image_data;
}

// amount should be -1 to 1
PGMImage *change_brightness(PGMImage *img, float amount) {
  float normalized_amount = amount < -1 ? -1 : amount > 1 ? 1 : amount;
  float max_amount = img->maxg;
  float resolved_amount = normalized_amount * max_amount;

  int total_pixels = img->xsize * img->ysize;

  int *g = (int *)malloc(total_pixels * sizeof(int));
  if (!g)
    return NULL;

  for (int i = 0; i < total_pixels; i++) {
    int val = img->g[i];
    int new_value = (int)roundf(val + resolved_amount);
    int new_value_normalized = new_value < 0           ? 0
                               : new_value > img->maxg ? img->maxg
                                                       : new_value;
    g[i] = new_value_normalized;
  }

  PGMImage *image_data = (PGMImage *)malloc(sizeof(PGMImage));
  if (!image_data) {
    free(g);
    return NULL;
  }

  image_data->xsize = img->xsize;
  image_data->ysize = img->ysize;
  image_data->maxg = img->maxg;
  image_data->g = g;

  return image_data;
}

PGMImage *change_gamma(PGMImage *img, float gamma, float c) {
  int total_pixels = img->xsize * img->ysize;

  int *g = (int *)malloc(total_pixels * sizeof(int));
  if (!g)
    return NULL;

  for (int i = 0; i < total_pixels; i++) {
    int val = img->g[i];
    int new_value = (int)roundf(pow(val, gamma) * c);
    int new_value_normalized = new_value < 0           ? 0
                               : new_value > img->maxg ? img->maxg
                                                       : new_value;
    g[i] = new_value_normalized;
  }

  PGMImage *image_data = (PGMImage *)malloc(sizeof(PGMImage));
  if (!image_data) {
    free(g);
    return NULL;
  }

  image_data->xsize = img->xsize;
  image_data->ysize = img->ysize;
  image_data->maxg = img->maxg;
  image_data->g = g;

  return image_data;
}

int main() {
  PGMImage *img = pgma_read_to_memory("screws.ascii.pgm");

  PGMImage *balanced_histogram_img = histogram_balance(img);

  pgma_write("screws.ascii.balanced.pgm", "balanced histogram",
             balanced_histogram_img->xsize, balanced_histogram_img->ysize,
             balanced_histogram_img->maxg, balanced_histogram_img->g);

  PGMImage *updated_contrast_img = change_contrast(img, 0.8);

  pgma_write("screws.ascii.contrast.pgm", "contrast 0.8",
             updated_contrast_img->xsize, updated_contrast_img->ysize,
             updated_contrast_img->maxg, updated_contrast_img->g);

  PGMImage *updated_brightness_img = change_brightness(img, -0.5);

  pgma_write("screws.ascii.brightness.pgm", "brightness -0.5",
             updated_brightness_img->xsize, updated_brightness_img->ysize,
             updated_brightness_img->maxg, updated_brightness_img->g);

  PGMImage *updated_gamma_img = change_gamma(img, 1.25, 1);

  pgma_write("screws.ascii.gamma.pgm", "gamma ^1.25", updated_gamma_img->xsize,
             updated_gamma_img->ysize, updated_gamma_img->maxg,
             updated_gamma_img->g);

  free(img->g);
  free(img);

  free(balanced_histogram_img->g);
  free(balanced_histogram_img);

  free(updated_contrast_img->g);
  free(updated_contrast_img);

  free(updated_brightness_img->g);
  free(updated_brightness_img);

  free(updated_gamma_img->g);
  free(updated_gamma_img);

  return 0;
}
