#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "gmath.h"
#include "matrix.h"
#include "ml6.h"


//vector functions
//normalize vetor, should modify the parameter
void normalize( double *vector ) {
}

//Return the dot porduct of a . b
double dot_product( double *a, double *b ) {
  return (a[0] * b[0]) + (a[1] * b[1]) + (a[2] * b[2]);
}


//Calculate the surface normal for the triangle whose first
//point is located at index i in polygons
double *calculate_normal(struct matrix *polygons, int i) {
  double ** matrix = polygons -> m;
  double *norm = malloc(3 * sizeof(double));
  double a[3];
  double b[3];

  a[0] = matrix[0][i + 1] - matrix[0][i];
  a[1] = matrix[1][i + 1] - matrix[1][i];
  a[2] = matrix[2][i + 1] - matrix[2][i];

  b[0] = matrix[0][i + 2] - matrix[0][i];
  b[1] = matrix[1][i + 2] - matrix[1][i];
  b[2] = matrix[2][i + 2] - matrix[2][i];

  norm[0] = (a[1] * b[2]) - (a[2] * b[1]);
  norm[1] = (a[2] * b[0]) - (a[0] * b[2]);
  norm[2] = (a[0] * b[1]) - (a[1] * b[0]);

  return norm;
}
