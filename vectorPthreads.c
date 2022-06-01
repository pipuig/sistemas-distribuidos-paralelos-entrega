#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>
#define prec 0.01

float *A,*auxVec,*auxSwap;
int N,pos,T, convergeGlobal=0,*convergeVec, iteraciones=0;
const float tercio = 1.0/3.0;

pthread_barrier_t barrera;


void* funcion(void *arg){
  int tid=*(int*)arg;
  //printf("Hilo id:%d\n",tid);
  int i;

  int posInicio=pos*(tid);
  int posFinal=posInicio+pos;
  int convergeLocal=1;
  if (tid == 0) posInicio++;
  if (tid == T-1) posFinal--;

  while(convergeGlobal == 0){

  float primero=(A[0]+A[1])*0.5; //todos calculan el primero asi puede chequear convergencia en el camino

  convergeVec[tid]=1;
  //printf("primero: %f \n",primero);

  if(tid == 0){ //si soy el primer thread el primero no lo hago porque ya lo hice
    auxVec[0]=primero;
    //printf("auxvec: %f \n",auxVec[0]);
  }

  if(tid == T-1){
  //si soy el thread que hace el ultimo cacho, no quiero que el for lo haga porque tiene una formula aparte
    auxVec[N-1]=(A[N-2]+A[N-1])*0.5;

    //printf("A[%d] es %.7f A[%d] es %.7f \n",N-2, A[N-2], N-1, A[N-1]);

    if( (fabs(primero-auxVec[N-1]) > prec) ){
     convergeVec[tid]=0;
     //printf("Hilo %d, B[0]-B[%d] = %.7f - B[0]=%.7f y B[%d]=%.7f \n",tid,N-1,fabs(primero-auxVec[N-1]),auxVec[0],N-1,auxVec[N-1]);
    }

  }

  for(i=posInicio; i<posFinal;i++){

    auxVec[i]=(A[i-1]+A[i]+A[i+1])*tercio;

    if( fabs(primero-auxVec[i])>prec){

     //printf("Hilo %d, B[0]-B[%d] = %.7f - B[0]=%.7f y B[%d]=%.7f \n",tid,i,fabs(primero-auxVec[i]),auxVec[0],i,auxVec[i]);
     //printf("le erro por %f \n",primero-auxVec[i]);
     convergeVec[tid]=0; //vector de threads convergentes
     //convergeLocal=0; //para que no entre mas a este if si ya sabemos que no converge
     break;
    }

  }
  for(;i<posFinal;i++){ //termino de computar lo que rompi en el break de arriba
    auxVec[i]=(A[i-1]+A[i]+A[i+1])*tercio;
  }

  //barrera para esperar
  pthread_barrier_wait(&barrera);

  //recorre uno secuencial
  if(tid == 0){
    i=0;
    convergeGlobal=1;
    while(convergeGlobal && (i < T)){
      convergeGlobal=convergeGlobal && convergeVec[i];
      i++;
    }

    iteraciones++;
    //necesito swapear los vectores para la proxima iteracion por eso uso un tercer puntero para hacer esto sino se me perderia la referencia
    auxSwap=A;
    A=auxVec;
    auxVec=auxSwap;

    //printf("iteracion: %d y i es %d\n",iteraciones,i);

    /*for(i=0;i<N;i++){
      //A[i]=auxVec[i];
      printf("auxVec[%d]= %.7f \n",i,auxVec[i]);
    }*/

  }

  //barrera para esperar
  pthread_barrier_wait(&barrera);

  }


  pthread_exit(NULL);
}


int main(int argc, char* argv[]){

 T = atoi(argv[2]);
 N = atoi(argv[1]);

 pthread_t misThreads[T];
 int threads_ids[T];

 double timetick;
 int i,j;

double dwalltime(){
        double sec;
        struct timeval tv;

        gettimeofday(&tv,NULL);
        sec = tv.tv_sec + tv.tv_usec/1000000.0;
        return sec;
}

//Aloca memoria para las matrices
  A=(float*)malloc(sizeof(float)*N);
  auxVec=(float*)malloc(sizeof(float)*N);
  convergeVec=(int*)malloc(sizeof(int)*T);

  //srand(time(NULL));
    //Inicializa el vector con numeros random
    for(i=0;i<N;i++){
       A[i]=(float)rand()/(float)(RAND_MAX/1);
       //printf("%f\n",A[i]);

    }
    //printf("\n");

    //Inicializa el vector converge
    for(i=0;i<T;i++){
       convergeVec[i]=1;
    }

 pthread_barrier_init(&(barrera), NULL, T);


 pos=N/T;

 timetick = dwalltime();

 for(int id=0;id<T;id++){
   threads_ids[id]=id;
   pthread_create(&misThreads[id],NULL,&funcion,(void*)&threads_ids[id]);
 }
 for(int id=0;id<T;id++){
   pthread_join(misThreads[id],NULL);
 }

 printf("Tamaño del vector: %d\nTiempo en segundos en correr el algoritmo es %f con %d iteraciones\n",N, dwalltime() - timetick, iteraciones);


//Elimino las barreras

pthread_barrier_destroy(&(barrera));


 free(A);
 free(auxVec);

 return 0;
}
