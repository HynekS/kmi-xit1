#include <stdio.h>
#include <stdlib.h>


char* four_from_one(char element) {
 
}


char *create_difussion_matrix(char *matrix, char current_dimension,
                              char max_dimension) {

  printf("current_dimension: %d\n", current_dimension);
  if (current_dimension >= max_dimension)
    return matrix;

  char extended_matrix_item_count = (current_dimension << 2);
  printf("extended_matrix_item_count: %d\n", extended_matrix_item_count);

  char *extended_matrix = malloc(extended_matrix_item_count * sizeof(char));

  int current_op = 0;

  for (int i = 0; i < extended_matrix_item_count; i++) {
    char current_matrix_item = matrix[i];
    // TO FIX
    char current_op = extended_matrix_item_count / 4;

    printf("current_op: %d\n", current_op);
    if (current_op == 0) {
      extended_matrix[i] = current_matrix_item * 4;
    }

    if (current_op == 1) {
      extended_matrix[i] = (current_matrix_item * 4) + 3;
    }

    if (current_op == 2) {
      extended_matrix[i] = (current_matrix_item * 4) + 2;
    }

    if (current_op == 3) {
      extended_matrix[i] = (current_matrix_item * 4) + 1;
    }
  }

  printf("[");
  for (int k = 0; k < extended_matrix_item_count; k++) {
    printf("%d, ", extended_matrix[k]);
  }
  printf("]\n");

  free(matrix);

  return create_difussion_matrix(extended_matrix, extended_matrix_item_count,
                                 max_dimension);
}

int main() {
  char *base_matrix = malloc(sizeof(char));
  base_matrix[0] = 0;

  char *m = create_difussion_matrix(base_matrix, 1, 16);
  free(m);
}