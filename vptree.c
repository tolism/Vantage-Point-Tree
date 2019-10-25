// LOIPON
// 1. EXO 4 KOMMATIA KODIKA ME PTHREADS SE SXOLIA (NA SVISTEI I distanceCalculation)
// 2. O YPOLOGISMOS TOU DISTANCE STO GRAMMIKO ITAN LATHOS GIATI KANAME MALLOC ANTI GIA CALLOC
// 3. MPOROUME NA PERASOUME POLLES METAVLITES STIN SYNARTISI TOU THREAD MESO STRUCT -> https://stackoverflow.com/questions/1352749/multiple-arguments-to-function-called-by-pthread-create
// 4. EXO SVISI TIN TEMPDISTANCE KAI TIN EVALA MESA STO ARXEIO MEDIAN
// 5. EKANA KANADIO AKOMA MALAKITSES GIA NA DIAFOROPOIISO TON KODIKA
// 6. DEN EXO KATALAVEI TO KOMMATI ME TO iterations
// 7. MIPOS PREPEI NA DILOSOUME TA PTHREADS MESA STIN distanceCalculation KAI APLA NA KANOUME MIA NEA SINARTISI POU THA KANEI APLA TON YPOLOGISMO KAI THA MPAINEI STIN CREATE?


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <pthread.h>
#include "quickselect.h"

#define N 234567
#define D 2

// PTHREADS
#define NOTHREADS 3


int threadCounter = -1;
//double *distance;
pthread_mutex_t mux;
pthread_attr_t attr;
clock_t t ;

typedef struct vpTree {
  double * data; //2d array containing all the points
  double * vp; //vantage
  double md; //median distance
  int idxVp; //the index of the vantage point in the original set
  struct vpTree * inner;
  struct vpTree * outer;
} vpTree;

typedef struct distances {
  int n ;
  int d ;
  double *points ;
  double *distance;

} distances;


double * generate_points(int n, int d) {

  srand(time(NULL));

  double * points = (double * ) malloc(n * d * sizeof(double));

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < d; j++) {
      *(points + i * d + j) = (double) rand() / RAND_MAX;
    }
  }
  //printf("Size of points is: %ld \n", sizeof(points));
  return points;
}

double * distanceCalculationSer(double * X, double * vPoint, int n, int d) {

  double * rDistance = (double * ) calloc(n - 1, sizeof(double));

  for (int i = 0; i < n - 1; i++) {
    for (int j = 0; j < d; j++) {
      //printf("Value of power : %lf ",pow((vPoint[j]-*(X + i*d + j)),2));
      rDistance[i] = rDistance[i] + (*(X + i * d + j) - vPoint[j]) * (*(X + i * d + j) - vPoint[j]);
    }
    rDistance[i] = sqrt(rDistance[i]);
  }
  return rDistance;
}

void treePrint(distances *dist ) {
  //PRINTING THE POINTS
    printf("NEW VPTREE NODE\n----------------\n\n");
    for (int i = 0; i < (dist->n+1); i++) {
      printf("POINT NO.%d: (", i + 1);
      for (int j = 0; j < dist->d; j++) {
        printf("%lf, ", *(dist->points + i * dist->d + j));
      }
      if (i < dist->n ) {
        printf("), Distance from Vantage Point: %lf \n", dist->distance[i]);
      } else {
        printf("), VANTAGE POINT\n");
      }
    }
}
void setTree(vpTree *tree , double *X , int n , int d ){

  tree->data = (double * ) malloc(n * d * sizeof(double));
  tree->vp = (double * ) malloc(d * sizeof(double));
  tree->data = X;
  for (int j = 0; j < d; j++) {
    tree->vp[j] = * (X + (n-1) * d + j);
  }
  tree->idxVp = n - 1;
}
void setDistanceStruct(distances *dist , double *X , int n , int d ){
  //dist->points=(double * ) malloc(n * d * sizeof(double));
  dist->distance =  (double * ) calloc(n - 1, sizeof(double));
  dist->points=X;
  dist->n = n-1;
  dist->d = d ;
}
void printSubTree(double *XSubTree ,int Counter , int d ) {
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
void createNewX(double *Xinner , double *Xouter , double *X , int n , int d , double *distance, double median){
  int inCounter = 0;
  int outCounter = 0;

  for (int i = 0; i < n - 1; i++) {
    if (distance[i] <= median) {
      for (int j = 0; j < d; j++) {
        *(Xinner + inCounter * d + j) = * (X + i * d + j);
      }
      inCounter++;
    }
    else {
      for (int j = 0; j < d; j++) {
        *(Xouter + outCounter * d + j) = * (X + i * d + j);
      }
      outCounter++;
    }
  }
}
// PTHREADS
void * distanceCalculationPar(void *data) {
  distances *localDist= (distances *) data;
  int i,j,start,end,iterations=0 , lastIt;

  double sumDist=0;

  // PX EIXA 50 SHMEIA
  // ENA TO VANTAGE POINT 49
  // ME AUTON TON TROPO KANEI 49/8 TO PAEI STO 7
  // OPOTE TO TELEUTAIO THREAD DEN THA EXEI TIPOTA NA KANEI

  pthread_mutex_lock(&mux);
  if (threadCounter==-1 || threadCounter==NOTHREADS-1)
    threadCounter=0;
  else if(threadCounter<NOTHREADS-1)
    threadCounter++;
  pthread_mutex_unlock(&mux);

  if(localDist->n%NOTHREADS ==0){
    iterations = ((localDist->n)/NOTHREADS);
    start = threadCounter *iterations;
    end = start + iterations;
    printf("Thread %d is doing iterations %d to %d \n",threadCounter,start,end-1);
  }
  else {
    iterations= round((localDist->n)/NOTHREADS);
    //printf("sskakk %d \n", iterations);
    lastIt = localDist->n - (NOTHREADS-1)*iterations ;

    if(threadCounter == NOTHREADS-1){
      start = (threadCounter)*iterations;
      end = start+lastIt;
      printf("Thread %d is doing iterations %d to %d \n",threadCounter,start,end-1);
    }
    else{
      start = (threadCounter)*iterations;
      end = start + iterations;
      printf("Thread %d is doing iterations %d to %d \n",threadCounter,start,end-1);
    }
  }
  for (i=start; i<end; i++){
    for(j=0; j<localDist->d; j++){
      sumDist= sumDist + (*(localDist->points + (localDist->n+1)*localDist->d + j) - *(localDist->points + i*localDist->d + j)) * (*(localDist->points + (localDist->n+1)*localDist->d + j) - *(localDist->points + i*localDist->d + j));
    }
    localDist->distance[i]=sqrt(sumDist);
    sumDist =0;
  //  printf("The i is : %d , distance is : %lf  \n" , i , distance[i]);
  }
}
struct vpTree * buildvp(double * X, int n, int d) {
//Leaf state :*
  if (n == 1)
    return NULL;

//Memory allocation
  distances *dist=(distances *)malloc(sizeof(distances));
  vpTree *p = (vpTree * ) malloc(sizeof(vpTree));

//Counters for Inner and Outer subtree
  int numberOfOuter = 0;
  int numberOfInner = 0;

//Variables
  double median;
  double * Xinner = NULL;
  double * Xouter = NULL;

  // PTHREADS
  pthread_t thr[NOTHREADS];

  setDistanceStruct(dist , X , n ,  d );
  setTree(p, X, n, d);

  //THRESHOLD EAN O ARITHMOS TWN SHMEIWN PROS UPOLOGISMO
  // EINAI MIKROTEROS APO TON ARITHMO TO THREADS POU XRHSIMOPOIW
  // PANE SEIRIAKA
  // H ALLIWS RUTHISMIH GIA ELATTWSH TWN ARITHO TWN THREADS

//  distance = distanceCalculationSer(X, p->vp, n, d);
  //t = clock();
  if(n-1<NOTHREADS){
    dist->distance = distanceCalculationSer(X, p->vp, n, d);
  }
  else{
    //PTHREADS
    for(int i=0; i<NOTHREADS; i++){
      pthread_create(&thr[i], &attr, distanceCalculationPar, (void *)dist);
    }

    for(int i=0; i<NOTHREADS; i++){
      pthread_join(thr[i],NULL);
    }
    //pthread_exit(NULL);
  }

//  t = clock() - t;
  // time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds
//  printf("TIME IN PARALLEL  : %lf \n" , time_taken);

  //threadCounter = -1;
  median = qselect(dist->distance, n - 1, (int)((n - 2) / 2));

  p->md = median;

  treePrint(dist);

//  printf("MEDIAN : %lf \n\n", median);

  numberOfOuter = (int)((n - 1) / 2);
  numberOfInner = n - 1 - numberOfOuter;

  if (numberOfInner != 0) {
    Xinner = (double * ) malloc(numberOfInner * d * sizeof(double));
  }
  if (numberOfOuter != 0) {
    Xouter = (double * ) malloc(numberOfOuter * d * sizeof(double));
  }

createNewX( Xinner , Xouter , X , n , d , dist->distance , p->md);

 printf("->XINNER  :\n");
 printSubTree(Xinner, numberOfInner , d);

 printf("->XOUTER  :\n");
printSubTree(Xouter , numberOfOuter , d);

  printf("\n\n");
//free(distance);
 //  printf("The distance  array size is  %d  \n" , (int)sizeof(distance) );
  free(dist);
  if (Xinner != NULL) {
    p->inner = buildvp(Xinner, numberOfInner, d);
  }
  if (Xouter != NULL) {
    p->outer = buildvp(Xouter, numberOfOuter, d);
  }

  return p;
}




vpTree * getInner(vpTree * T) {
  return T->inner;
}

vpTree * getOuter(vpTree * T) {
  return T->outer;
}

double * getVP(vpTree * T) {
  return T->vp;
}

double getMD(vpTree * T) {
  return T->md;
}

int getidxVp(vpTree * T) {
  return T->idxVp;
}

int main(int argc, char const * argv[]) {

  double * array = generate_points(N, D);
  vpTree * root = (vpTree * ) malloc(sizeof(vpTree));

  root = buildvp(array, N, D);

  free(array);
  free(root);

  return 0;
}
