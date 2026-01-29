#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  unsigned char red, green, blue;
} PPMPixel;

typedef struct {
  int x, y;
  PPMPixel *data;
} PPMImage;

#define RGB_COMPONENT_COLOR 255

// https://stackoverflow.com/a/2699908/8252267
static PPMImage *readPPM(const char *filename) {
  char buff[16];
  PPMImage *img;
  FILE *fp;
  int c, rgb_comp_color;

  // open PPM file for reading
  fp = fopen(filename, "rb");
  if (!fp) {
    fprintf(stderr, "Unable to open file '%s'\n", filename);
    exit(1);
  }

  // read image format
  if (!fgets(buff, sizeof(buff), fp)) {
    perror(filename);
    exit(1);
  }

  // check the image format
  if (buff[0] != 'P' || buff[1] != '6') {
    fprintf(stderr, "Invalid image format (must be 'P6')\n");
    exit(1);
  }

  // alloc memory form image
  img = (PPMImage *)malloc(sizeof(PPMImage));
  if (!img) {
    fprintf(stderr, "Unable to allocate memory\n");
    exit(1);
  }

  // check for comments
  c = getc(fp);
  while (c == '#') {
    while (getc(fp) != '\n')
      ;
    c = getc(fp);
  }

  ungetc(c, fp);
  // read image size information
  if (fscanf(fp, "%d %d", &img->x, &img->y) != 2) {
    fprintf(stderr, "Invalid image size (error loading '%s')\n", filename);
    exit(1);
  }

  // read rgb component
  if (fscanf(fp, "%d", &rgb_comp_color) != 1) {
    fprintf(stderr, "Invalid rgb component (error loading '%s')\n", filename);
    exit(1);
  }

  // check rgb component depth
  if (rgb_comp_color != RGB_COMPONENT_COLOR) {
    fprintf(stderr, "'%s' does not have 8-bits components\n", filename);
    exit(1);
  }

  while (fgetc(fp) != '\n')
    ;
  // memory allocation for pixel data
  img->data = (PPMPixel *)malloc(img->x * img->y * sizeof(PPMPixel));

  if (!img->data) {
    fprintf(stderr, "Unable to allocate memory\n");
    exit(1);
  }

  // read pixel data from file
  if (fread(img->data, sizeof(PPMPixel), img->x * img->y, fp) != img->x * img->y) {
    fprintf(stderr, "Error loading image '%s'\n", filename);
    exit(1);
  }

  fclose(fp);
  return img;
}

// https://stackoverflow.com/a/2699908/8252267
void writePPM(const char *filename, PPMImage *img) {
  FILE *fp;
  // open file for output
  fp = fopen(filename, "wb");
  if (!fp) {
    fprintf(stderr, "Unable to open file '%s'\n", filename);
    exit(1);
  }

  // write the header file
  // image format
  fprintf(fp, "P6\n");

  // image size
  fprintf(fp, "%d %d\n", img->x, img->y);

  // rgb component depth
  fprintf(fp, "%d\n", RGB_COMPONENT_COLOR);

  // pixel data
  fwrite(img->data, sizeof(unsigned char), 3 * img->x * img->y, fp);
  fclose(fp);
}

typedef struct {
  unsigned char red;
  unsigned char green;
  unsigned char blue;
} color_depths;

char reduce_depth(int source_depth, char target_depth) {
  char delta = 8 - target_depth;
  char trimmed = source_depth >> delta;
  return trimmed << delta;
}

void modify_color_depth(PPMImage *img, color_depths* color_depths) {
  if (img) {
    int i = 0;
    PPMPixel *data = img->data;

    while (i < img->x * img->y) {
      data[i].red = reduce_depth(data[i].red, 3);
      data[i].green  = reduce_depth(data[i].green, 3);;
      data[i].blue  = reduce_depth(data[i].blue, 2);

      i++;
    }
  }
}


int main() {
  color_depths depths_params = {
    5,
    2,
    1,
  };

  PPMImage *image;
  image = readPPM("test_input_1.ppm");

  modify_color_depth(image, &depths_params);

  writePPM("test_output_1.ppm", image);
  free(image->data);
  free(image);
  image = NULL;

  printf("Press Enter to continue...");
  getchar();
}