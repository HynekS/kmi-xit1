#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct pixel {
  unsigned int r, g, b;
} pixel;

typedef struct imagem {
  int cols, rows;
  char mnumber[3];
  int maxcolor;
  pixel *matrix;
} imagem;

static imagem read(const char *filename) {
  imagem img;
  FILE *f;
  int i;
  char buf[BUFSIZ];

  f = fopen(filename, "r");
  if (f == NULL) {
    fprintf(stderr, "Can't open %s\n", filename);
    exit(1);
  }

  if (fgets(buf, sizeof buf, f) == NULL) {
    fprintf(stderr, "Can't read data\n");
    exit(1);
  }

  for (i = 0; i < 2; i++) {
    img.mnumber[i] = buf[i];
  }
  img.mnumber[2] = '\0';

  if (fgets(buf, sizeof buf, f) == NULL) {
    fprintf(stderr, "Can't read data\n");
    exit(1);
  }
  sscanf(buf, "%d %d", &img.cols, &img.rows);
  if (fgets(buf, sizeof buf, f) == NULL) {
    fprintf(stderr, "Can't read data\n");
    exit(1);
  }
  sscanf(buf, "%d", &img.maxcolor);
  size_t num_pixels = (size_t)img.cols * (size_t)img.rows;
  if (img.cols <= 0 || img.rows <= 0 || num_pixels > SIZE_MAX / sizeof(pixel)) {
    fprintf(stderr, "Invalid image size or integer overflow detected\n");
    exit(1);
  }
  img.matrix = malloc(num_pixels * sizeof(pixel));
  if (img.matrix == NULL) {
    fprintf(stderr, "malloc failed\n");
    exit(1);
  }

  i = 0;
  while (i < num_pixels) {
    unsigned int r, g, b;
    int res = fscanf(f, "%u %u %u", &r, &g, &b);
    if (res != 3) {
      break;
    }
    img.matrix[i].r = r;
    img.matrix[i].g = g;
    img.matrix[i].b = b;
    i++;
  }
  fclose(f);
  return img;
}

int main() {
  imagem img = read("p3.ppm");
  int totalPixels = img.cols * img.rows;
  for(int i = 0; i < totalPixels; i++) {
    printf("%i %i %i\n", img.matrix[i].r, img.matrix[i].g, img.matrix[i].b);
  }
  free(img.matrix);
  img.matrix = NULL;
}
