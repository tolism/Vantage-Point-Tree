#ifndef LIBRARIES_H
#define LIBRARIES_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <pthread.h>
//subnodes
#define MAX_THREADS 5
//distance calculation
#define NOTHREADS 2


typedef struct vptree {
  double * data; //2d array containing all the points
  double * vp; //vantage
  double md; //median distance
  int idxVp; //the index of the vantage point in the original set
  struct vptree * inner;
  struct vptree * outer;
} vptree;

typedef struct param {
  double * data;
  int * idx;
  double *distances;
  int n;
  int d;
} param;

double * generate_points(int n, int d);
double * distanceCalculationSer(double * X, double * vPoint, int n, int d);
void * distanceCalculationPar(void *data);
double qselect(double *v, int len, int k);

#endif
