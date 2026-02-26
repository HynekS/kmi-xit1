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

// Reading/writing PPM images
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

int sign(int x) {
    return (x > 0) - (x < 0);
}

void convertToGrayscale(PPMImage *img) {
  if (img) {
    int i = 0;
    PPMPixel *data = img->data;

    while (i < img->x * img->y) {
      int colorsSum = data[i].red + data[i].green + data[i].blue;
      unsigned char colorsSumAverage = (unsigned char)roundf(colorsSum / 3.0);
      data[i].red = colorsSumAverage;
      data[i].green = colorsSumAverage;
      data[i].blue = colorsSumAverage;
      i++;
    }
  }
}

void desaturate(PPMImage *img, float magnitude) {
  if (img) {
    int i = 0;
    magnitude = magnitude < 0 ? 0 : magnitude > 1 ? 1 : magnitude;
    PPMPixel *data = img->data;

    while (i < img->x * img->y) {
      int colorsSum = data[i].red + data[i].green + data[i].blue;
      unsigned char colorsSumAverage = (unsigned char)roundf(colorsSum / 3.0f);

      unsigned char red = data[i].red;
      unsigned char green = data[i].green;
      unsigned char blue = data[i].blue;

      data[i].red = (unsigned char)roundf(((1 - magnitude) * colorsSumAverage) +
                                          (magnitude * red));
      data[i].green = (unsigned char)roundf(
          ((1 - magnitude) * colorsSumAverage) + (magnitude * green));

      data[i].blue = (unsigned char)roundf(
          ((1 - magnitude) * colorsSumAverage) + (magnitude * blue));
          
          i++;
    }
  }
}

void saturationBalance(PPMImage *img) {
  if (img) {
    int i = 0;
    PPMPixel *data = img->data;

    while (i < img->x * img->y) {
      int colorsSum = data[i].red + data[i].green + data[i].blue;
      unsigned char colorsSumAverage = (unsigned char)roundf(colorsSum / 3.0f);

      int distanceRed = colorsSumAverage - data[i].red;
      int distanceGreen = colorsSumAverage - data[i].green;
      int distanceBlue = colorsSumAverage - data[i].blue;

      int averageDistance = (unsigned char)roundf((abs(distanceRed) + abs(distanceGreen) + abs(distanceBlue)) / 3.0f);

      data[i].red = colorsSumAverage + (averageDistance * sign(distanceRed));
      data[i].green  = colorsSumAverage + (averageDistance * sign(distanceGreen));
      data[i].blue  = colorsSumAverage + (averageDistance * sign(distanceBlue));

      i++;
    }
  }
}

int main() {
  // NOTE reading the same image 3 times is obviously not very effective - the adjustment functions should probably accept/return new image struct instead

  // Grayscale
  printf("Performing grayscale conversion on file 'test_input.ppm'\n");
  PPMImage *image_a;
  image_a = readPPM("test_input.ppm");

  convertToGrayscale(image_a);
  writePPM("test_output_grayscale.ppm", image_a);

  free(image_a->data);
  free(image_a);

  printf("Grayscale conversion has been completed.\n\n");


  // Desaturate
  printf("Performing desaturation on file 'test_input.ppm'\n");
  PPMImage *image_b;
  image_b = readPPM("test_input.ppm");

  desaturate(image_b, 0.5);
  writePPM("test_output_desaturation.ppm", image_b);

  free(image_b->data);
  free(image_b);

  printf("Desaturation has been completed.\n\n");


  // Saturation balance
  printf("Performing saturation balance on file 'test_input.ppm'\n");
  PPMImage *image_c;
  image_c = readPPM("test_input.ppm");

  saturationBalance(image_c);
  writePPM("test_output_saturation_balance.ppm", image_c);

  free(image_c->data);
  free(image_c);

  printf("Saturation balance has been completed.\n\n");
}