#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "quickselect.h"

#define N 20
#define D 2


typedef struct vptree {
  double * data; //2d array containing all the points
  double * vp; //vantage
  double md; //median distance
  int idxVp; //the index of the vantage point in the original set
  struct vptree * inner;
  struct vptree * outer;
} vptree;



double * generate_points(int n, int d) {

  srand(time(NULL));

  double * points = (double * ) malloc(n * d * sizeof(double));

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < d; j++) {
      *(points + i * d + j) = (double) rand() / RAND_MAX;
    }
  }
  return points;
}

double * distanceCalculation(double * X, double * vPoint, int n, int d) {

  double * rDistance = (double * ) calloc(n - 1, sizeof(double));
  double temp = 0;
  for (int i = 0; i < n - 1; i++) {
    for (int j = 0; j < d; j++) {
      //printf("Value of power : %lf ",pow((vPoint[j]-*(X + i*d + j)),2));
      rDistance[i] = rDistance[i] + (*(X + i * d + j) - vPoint[j]) * (*(X + i * d + j) - vPoint[j]);
    }
    rDistance[i] = sqrt(rDistance[i]);
  }
  return rDistance;
}


void printTree(){

}

void setTree(vptree *tree , double * X , int *idx ,  int n , int d ){

  tree->data = (double * ) malloc(n * d * sizeof(double));
  tree->vp = (double * ) malloc(d * sizeof(double));
  tree->data = X;
  for (int j = 0; j < d; j++) {
    tree->vp[j] = * (X + (n-1) * d + j);
  }
  tree->idxVp = idx[n-1];
}


void printSubTree(double *XSubTree ,int Counter , int d){
  if (Counter == 0) {
    printf("  NULL\n");
  }
  else {
    for (int i = 0; i < Counter; i++) {
      for (int j = 0; j < d; j++) {
        printf("  %8.6lf ", *(XSubTree + i * d + j));
      }
      printf("\n");
    }
  }
}

void createNewX(double * Xinner , double * Xouter , double * X , int *innerIdx , int *outerIdx , int n , int d , double * distance , double median){

  int inCounter = 0; //number of Inner points//
  int outCounter = 0; //number of Outer points//
  for (int i = 0; i < n - 1; i++) {
    if (distance[i] <= median) {
      for (int j = 0; j < d; j++) {
        *(Xinner + inCounter * d + j) = * (X + i * d + j);
      }
      innerIdx[inCounter]=i;
      inCounter++;
    } else {
      for (int j = 0; j < d; j++) {
        *(Xouter + outCounter * d + j) = * (X + i * d + j);
      }
      outerIdx[outCounter]=i;
      outCounter++;
    }
  }
}


vptree * buildvp(double * X, int * idx, int n, int d) {

  vptree *p = (vptree * ) malloc(sizeof(vptree));
  int numberOfOuter = 0;
  int numberOfInner = 0;
  double * distance;
  double median;
  double * Xinner = NULL;
  double * Xouter = NULL;
  int * innerIdx = NULL;
  int * outerIdx = NULL;

  if (n == 1)
    return NULL;
  else if(n == 0)
    return NULL;

  setTree(p,X,idx,n,d);

  distance = distanceCalculation(X, p->vp, n, d);
  median = qselect(distance, n - 1, (int)((n - 2) / 2));

  p->md = median;

  numberOfOuter = (int)((n - 1) / 2);
  numberOfInner = n - 1 - numberOfOuter;

  if (numberOfInner != 0) {
    Xinner = (double * ) malloc(numberOfInner * d * sizeof(double));
    innerIdx = (int *) malloc(numberOfInner * sizeof(int));
  }
  if (numberOfOuter != 0) {
    Xouter = (double * ) malloc(numberOfOuter * d * sizeof(double));
    outerIdx = (int *) malloc(numberOfOuter * sizeof(int));
  }

createNewX( Xinner , Xouter , X , innerIdx , outerIdx ,n , d , distance , median);


  printf("NEW vptree NODE\n----------------\n\n");
  for (int i = 0; i < n; i++) {
    printf("POINT NO.%d: (", idx[i]);
    for (int j = 0; j < d; j++) {
      printf("%lf, ", *(X + i * d + j));
    }
    if (i < n - 1) {
      printf("), Distance from Vantage Point: %lf \n", distance[i]);
    } else {
      printf("), VANTAGE POINT\n");
    }
  }
  printf("MEDIAN : %lf \n\n", median);
  printf("THE VANTAGE POINT INDEX IS: %d\n",p->idxVp);


  printSubTree(Xinner , numberOfInner, d);
  printSubTree(Xouter , numberOfOuter , d);

  printf("\n\n");

  if (Xinner != NULL) {
    p->inner = buildvp(Xinner, innerIdx, numberOfInner, d);
  }
  if (Xouter != NULL) {
    p->outer = buildvp(Xouter, outerIdx, numberOfOuter, d);
  }

  free(distance);
  free(Xinner);
  free(Xouter);
  free(innerIdx);
  free(outerIdx);

  return p;
}

vptree * getInner(vptree * T) {
  return T->inner;
}

vptree * getOuter(vptree * T) {
  return T->outer;
}

double * getVP(vptree * T) {
  return T->vp;
}

double getMD(vptree * T) {
  return T->md;
}

int getidxVp(vptree * T) {
  return T->idxVp;
}


int main(int argc, char const * argv[]) {

  double * array = generate_points(N, D);
  int * idx = (int *) malloc(N * sizeof(int));
  for(int i=0; i<N; i++){
    idx[i] = i;
  }
  vptree * root = (vptree * ) malloc(sizeof(vptree));

  root = buildvp(array, idx, N, D);

  free(root);
  free(array);

  return 0;
}
