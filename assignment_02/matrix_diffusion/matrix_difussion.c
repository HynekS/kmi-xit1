#include <stdio.h>
#include <stdlib.h>

char *create_difussion_matrix(char *matrix, char current_dimension,
                              char max_dimension) {
  if (current_dimension >= max_dimension)
    return matrix;

  char extended_matrix_item_count = (current_dimension * 4);

  char *extended_matrix = malloc(extended_matrix_item_count * sizeof(char));

  for (int i = 0; i < extended_matrix_item_count / 4; i++) {
    char current_matrix_item = matrix[i];
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

  free(matrix);

  return create_difussion_matrix(extended_matrix, extended_matrix_item_count,
                                 max_dimension);
}

int main() {
  char *base_matrix = malloc(sizeof(char));
  base_matrix[0] = 0;

  char *m = create_difussion_matrix(base_matrix, 1, 64);

  printf("[");
  for (int k = 0; k < 256; k++) {
    printf("%d, ", m[k]);
  }
  printf("]\n");

  free(m);
}