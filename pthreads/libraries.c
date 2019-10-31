#include "libraries.h"



volatile int threadCounter = -1;
pthread_mutex_t mux;
pthread_mutex_t mux1;
pthread_attr_t attr;

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

double * distanceCalculationSer(double * X, double * vPoint, int n, int d) {

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


void * distanceCalculationPar(void *data) {
  param *localDist= (param *) data;
  int i,j,start,end,iterations=0 , lastIt;

  double sumDist=0;
  int localThreadCounter = 0 ;

  pthread_mutex_lock(&mux);
  if (threadCounter==-1 || threadCounter==NOTHREADS-1){
    threadCounter=0;
    localThreadCounter = threadCounter;
  }
  else if(threadCounter<NOTHREADS-1){
    threadCounter++;
    localThreadCounter = threadCounter;
  }
  pthread_mutex_unlock(&mux);

  if((localDist->n-1)%NOTHREADS ==0){
    iterations = ((localDist->n)/NOTHREADS);
    start = localThreadCounter *iterations;
    end = start + iterations;
  //  printf("Thread %d is doing iterations %d to %d \n",localThreadCounter,start,end-1);
  }
  else {
    iterations= round((localDist->n-1)/NOTHREADS);
    lastIt = (localDist->n-1) - (NOTHREADS-1)*iterations ;

    if(localThreadCounter == NOTHREADS-1){
      start = (localThreadCounter)*iterations;
      end = start+lastIt;
    //  printf("Thread %d is doing iterations %d to %d \n",localThreadCounter,start,end-1);
    }
    else{
      start = (localThreadCounter)*iterations;
      end = start + iterations;
  //   printf("Thread %d is doing iterations %d to %d \n",localThreadCounter,start,end-1);
    }
  }

  for (i=start; i<end; i++){
    for(j=0; j<localDist->d; j++){
      sumDist= sumDist + (*(localDist->data + (localDist->n)*localDist->d + j) - *(localDist->data + i*localDist->d + j)) * (*(localDist->data + (localDist->n)*localDist->d + j) - *(localDist->data + i*localDist->d + j));
    }
    localDist->distances[i]=sqrt(sumDist);
  //  printf("THE DISTANCES IS :  %lf \n" , localDist->distances[i]);
    sumDist =0;
  }

}


double qselect(double *v, int len, int k)
{
	#	define SWAP(a, b) { tmp = tArray[a]; tArray[a] = tArray[b]; tArray[b] = tmp; }
	int i, st;
	double tmp;
	double * tArray = (double * ) malloc(len * sizeof(double));
	for(int i=0; i<len; i++){
		tArray[i] = v[i];
	}
	for (st = i = 0; i < len - 1; i++) {
		if (tArray[i] > tArray[len-1]) continue;
		SWAP(i, st);
		st++;
	}
	SWAP(len-1, st);
	return k == st	? tArray[st]
			:st > k	? qselect(tArray, st, k)
				: qselect(tArray + st, len - st, k - st);
}
