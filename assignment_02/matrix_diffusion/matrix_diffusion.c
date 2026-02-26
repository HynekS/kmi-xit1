#include "pgma_io.h"
#include <stdio.h>
#include <stdlib.h>

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

typedef struct {
  int **matrix;
  int dimension;
} DiffusionMatrix;

int get_square_root_of_2_exponent(int n) {
  int position = 0;
  int temp = n;

  while (temp >>= 1) {
    position++;
  }

  return 1 << (position / 2);
}

// TOTO Pixels are not 'correctly' placed, needs improvement
DiffusionMatrix *transform(int *old_matrix, int dimension) {

  int **new_matrix = malloc(dimension * sizeof(int));

  for (int i = 0; i < dimension; i++) {
    new_matrix[i] = malloc(dimension * sizeof(int));
  }

  for (int row_index = 0; row_index < dimension; row_index++) {
    for (int column_index = 0; column_index < dimension; column_index++) {
      new_matrix[row_index][column_index] =
          old_matrix[(row_index * dimension) + column_index];
    }
  }

  DiffusionMatrix *dm = malloc(sizeof(DiffusionMatrix));

  dm->matrix = new_matrix;
  dm->dimension = dimension;

  return dm;
}

int *create_difussion_matrix(int *matrix, int current_item_count,
                             int max_dimension) {
  if (current_item_count / 4 >= max_dimension)
    return matrix;

  int extended_matrix_item_count = (current_item_count * 4);

  int *extended_matrix = malloc(extended_matrix_item_count * sizeof(int));

  for (int i = 0; i < extended_matrix_item_count / 4; i++) {
    int current_matrix_item = matrix[i];
    for (int current_op = 0; current_op < 4; current_op++) {
      int extended_matrix_index = (i * 4) + current_op;

      if (current_op == 0) {
        extended_matrix[extended_matrix_index] = (current_matrix_item * 4);
      }

      if (current_op == 1) {
        extended_matrix[extended_matrix_index] = (current_matrix_item * 4) + 3;
      }

      if (current_op == 2) {
        extended_matrix[extended_matrix_index] = (current_matrix_item * 4) + 2;
      }

      if (current_op == 3) {
        extended_matrix[extended_matrix_index] = (current_matrix_item * 4) + 1;
      }
    }
  }

  //int root = get_square_root_of_2_exponent(extended_matrix_item_count);

  //for (int k = 0; k < extended_matrix_item_count; k++) {
  //  if (k % root == 0) {
  //    printf("\n");
  //  }
  //  printf("%3d ", extended_matrix[k]);
  //}
  //printf("\n\n");
  //
  free(matrix);

  return create_difussion_matrix(extended_matrix, extended_matrix_item_count,
                                 max_dimension);
}

PGMImage *matrix_diffusion(PGMImage *source,
                           DiffusionMatrix *diffusion_matrix) {
  int dimension = diffusion_matrix->dimension;
  int destination_xsize = source->xsize * dimension;
  int destination_ysize = source->ysize * dimension;

  PGMImage *destination =
      create_blank_image(destination_xsize, destination_ysize);

  for (int source_pixel_index = 0;
       source_pixel_index < source->xsize * source->ysize;
       source_pixel_index++) {
    int pixel_value = source->g[source_pixel_index];

    int src_row = source_pixel_index / source->xsize;
    int src_col = source_pixel_index % source->xsize;

    int dest_base_row = src_row * dimension;
    int dest_base_col = src_col * dimension;

    for (int row = 0; row < dimension; row++) {
      for (int column = 0; column < dimension; column++) {
        int diffusion_matrix_value = diffusion_matrix->matrix[row][column];
        int new_value = pixel_value >= diffusion_matrix_value ? 255 : 0;

        int dest_row = dest_base_row + row;
        int dest_col = dest_base_col + column;
        int dest_index = dest_row * destination_xsize + dest_col;

        destination->g[dest_index] = new_value;
      }
    }
  }

  return destination;
}

int main() {
  const int DIMENSION = 64;
  // TO CONSIDER: Do not free this right in the recursive function, main should
  // be responsible
  int *base_matrix = malloc(sizeof(int));
  base_matrix[0] = 0;

  // TODO return struct with the array and the resulting dimension
  int *array_of_recursive_function_results =
      create_difussion_matrix(base_matrix, 1, DIMENSION);

  // TODO use dimension instead of the input length (see above)
  DiffusionMatrix *diffusion_matrix =
      transform(array_of_recursive_function_results, DIMENSION);

  printf("Started processing image 'barbara.ascii.pgm. Please wait...'\n\n");

  PGMImage *source_1 = pgma_read_to_memory("barbara.ascii.pgm");
  PGMImage *destination_1 = matrix_diffusion(source_1, diffusion_matrix);

  pgma_write("result_barbara.ascii.pgm", "Diffusion result",
             destination_1->xsize, destination_1->ysize, destination_1->maxg,
             destination_1->g);

  free(source_1->g);
  free(source_1);

  free(destination_1->g);
  free(destination_1);

  printf("Completed processing image 'barbara.ascii.pgm'\n\n");

  PGMImage *source_2 = pgma_read_to_memory("coins.ascii.pgm");
  PGMImage *destination_2 = matrix_diffusion(source_2, diffusion_matrix);

  printf("Started processing image 'coins.ascii.pgm. Please wait...'\n\n");

  pgma_write("result_coins.ascii.pgm", "Diffusion result", destination_2->xsize,
             destination_2->ysize, destination_2->maxg, destination_2->g);

  free(source_2->g);
  free(source_2);

  free(destination_2->g);
  free(destination_2);

  printf("Completed processing image 'coins.ascii.pgm'\n\n");



  free(array_of_recursive_function_results);

  for (int i = 0; i < diffusion_matrix->dimension; i++) {
    free(diffusion_matrix->matrix[i]);
  }
  free(diffusion_matrix->matrix);
  free(diffusion_matrix);
}