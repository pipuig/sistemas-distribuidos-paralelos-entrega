#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>
#define prec 0.01

float *M,*mat,*auxSwap;
int N,pos,T, convergeGlobal=0,*convergeVec, iteraciones=0;
const float tercio = 1.0/3.0;

pthread_barrier_t barrera; 


void* funcion(void *arg){
  int tid=*(int*)arg;
  printf("Hilo id:%d\n",tid);
  int i,j;
  float primero;
  int posInicio=pos*(tid);
  int posFinal=posInicio+pos;
  if (tid==0) posInicio++;
  if (tid==(T-1)) posFinal--;
  int convergeLocal=1;
  
  while(convergeGlobal == 0){
      primero=(M[0]+M[1]+
              M[N]+M[N+1])/4;
      if (tid==0) mat[0] = primero;
      if (tid==T-1){
        mat[N*N-1] = (M[N*N-1] + M[N*(N-1)-1] 
                   + M[N*(N-1)-2] + M[N*N-2])/4;
      }
      for (i=posInicio;i<posFinal;i++){
        for (j=1;j<N-1;j++){ //calculo los del medio
          switch (i){
              default: { //si no estoy en un lateral superior o inferior
              mat[i*N+j] = (M[(i-1)*N+(j-1)] + M[(i-1)*N+j] + M[(i-1)*N+j+1]
                            + M[(i)*N+(j-1)] + M[(i)*N+j] + M[(i)*N+j+1]
                          + M[(i+1)*N+(j-1)] + M[(i+1)*N+j] + M[(i+1)*N+j+1])/9; 
              }
              case 0: { //si estoy en el lateral superior
                  mat[j] = (M[j-1] + M[j] + M[j+1]
                       + M[N+(j-1)] + M[N+j] + M[N+(j+1)])/6;
              }
              case (N-1): { //si estoy en el inferior
                  mat[(N-1)*N+j] = (M[(N-1)*N+(j-1)] + M[(N-1)*N+j] + M[(N-1)*N+(j+1)]
                                +  M[((N-1)*N-1)+(j-1)] + M[((N-1)*N-1)+j] + M[((N-1)*N-1)+(j+1)])/6; 
              }
        }
      }
      }
      //faltarian todos los chequeos de convergencia y el cálculo de los laterales izq y der
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
  A=(float*)malloc(sizeof(float)*N*N);
  auxVec=(float*)malloc(sizeof(float)*N*N);
  convergeVec=(int*)malloc(sizeof(int)*T);
  
  srand(time(NULL));
    //Inicializa el vector con numeros random
    for(i=0;i<N*N;i++){
       A[i]=(float)rand()/(float)(RAND_MAX/1);
       printf("%f\n",A[i]);
       
    }
    printf("\n");
    
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
 
 printf("Tiempo en segundos en correr el algoritmo es %f con %d iteraciones\n", dwalltime() - timetick, iteraciones);


//Elimino las barreras

pthread_barrier_destroy(&(barrera));  


 free(A);
 free(auxVec);

 return 0;
}