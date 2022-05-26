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

  while(convergeGlobal == 0){

      primero=(M[0]+M[1]+
              M[N]+M[N+1])/4;
      convergeVec[tid]=1;


      if (tid==0){ //primer thread hace primera fila siempre
        mat[0] = primero;
        //hago lo del medio
        for (i=1;i<N-1;i++){
            mat[i] = (M[i-1] + M[i] + M[i+1] + M[i-1+N] + M[i+N] + M[i+1+N])/6; //chequear si converge
        }
        //hago la esquina derecha superior
        mat[N-1] = (M[N-1] + M[N-2] + M[(2*N)-2] + M[(2*N)-1])/4;


        //hago los bloques que me tocan en for
        for (i=posInicio;i<posFinal;i++){
            mat[i*N] = (M[(i-1)*N] + M[(i-1)*N+1]
                        + M[i*N] + M[i*N+1] +
                        M[(i+1)*N] + M[(i+1)*N+1])/6;
            for(j=1;j<N-1;j++){
               mat[i*N+j] = (M[(i-1)*N+(j-1)] + M[(i-1)*N+j] + M[(i-1)*N+j+1]
                          + M[(i)*N+(j-1)] + M[(i)*N+j] + M[(i)*N+j+1]
                          + M[(i+1)*N+(j-1)] + M[(i+1)*N+j] + M[(i+1)*N+j+1])/9;
            }
            mat[(i+1)*N-1] = (M[(i-1)*N + (N-2)] + M[(i-1)*N + (N-1)]
                           + M[i*N + (N-2)] + M[i*N + (N-1)]
                           + M[(i+1)*N + (N-2)] + M[(i+1)*N + (N-1)])/6;
        }
      }

      if (tid==T-1){ //ultimo thread hace ultima fila siempre
        //hagos los bloques que me tocan del for
        for (i=posInicio;i<posFinal;i++){
            mat[i*N] = (M[(i-1)*N] + M[(i-1)*N+1]
                        + M[i*N] + M[i*N+1] +
                        M[(i+1)*N] + M[(i+1)*N+1])/6;
            for(j=1;j<N-1;j++){
               mat[i*N+j] = (M[(i-1)*N+(j-1)] + M[(i-1)*N+j] + M[(i-1)*N+j+1]
                          + M[(i)*N+(j-1)] + M[(i)*N+j] + M[(i)*N+j+1]
                          + M[(i+1)*N+(j-1)] + M[(i+1)*N+j] + M[(i+1)*N+j+1])/9;
            }
            mat[(i+1)*N-1] = (M[(i-1)*N + (N-2)] + M[(i-1)*N + (N-1)]
                           + M[i*N + (N-2)] + M[i*N + (N-1)]
                           + M[(i+1)*N + (N-2)] + M[(i+1)*N + (N-1)])/6;
        }
        //hago la esquina izquierda
        mat[N*(N-1)] = (M[N*(N-1)] + M[N*(N-2)] + M[N*(N-2)+1] + M[N*(N-1)+1])/4;
        //hago lo del medio
        for (i=(N-1)*N +1 ; i<N*N -1 ;i++){
            mat[i] = (M[i-1] + M[i] + M[i+1] + M[i-1-N] + M[i-N] + M[i+1-N])/6; //chequear si converge
        }
        //hago la ultima esquina
        mat[N*N-1] = (M[N*N-1] + M[N*(N-1)-1]
                   + M[N*(N-1)-2] + M[N*N-2])/4;
      }

      if((tid != 0) && (tid != T-1)){
        //si soy cualquier otro thread
        for (i=posInicio;i<posFinal;i++){
            mat[i*N] = (M[(i-1)*N] + M[(i-1)*N+1]
                        + M[i*N] + M[i*N+1] +
                        M[(i+1)*N] + M[(i+1)*N+1])/6;
            for(j=1;j<N-1;j++){
               mat[i*N+j] = (M[(i-1)*N+(j-1)] + M[(i-1)*N+j] + M[(i-1)*N+j+1]
                          + M[(i)*N+(j-1)] + M[(i)*N+j] + M[(i)*N+j+1]
                          + M[(i+1)*N+(j-1)] + M[(i+1)*N+j] + M[(i+1)*N+j+1])/9;
            }
            mat[(i+1)*N-1] = (M[(i-1)*N + (N-2)] + M[(i-1)*N + (N-1)]
                           + M[i*N + (N-2)] + M[i*N + (N-1)]
                           + M[(i+1)*N + (N-2)] + M[(i+1)*N + (N-1)])/6;
        }
      }


      int inicioChequeo=posInicio*N; //porque posInicio no se deberia tocar
      int finalChequeo=posFinal*N;

      if(tid==0){
        inicioChequeo=1;
      }
      if(tid==T-1){
        finalChequeo=N*N;
      }

      for (i=inicioChequeo;i<finalChequeo;i++){
          if (fabs(mat[0] - mat[i]) > 0.01){
             convergeVec[tid] = 0;
             break;
             }
          }

      pthread_barrier_wait(&barrera);

      if(tid == 0){
        i=0;
        convergeGlobal=1;
        while(convergeGlobal && (i < T)){
          convergeGlobal=convergeGlobal && convergeVec[i];
          i++;
        }

        auxSwap=M;
        M=mat;
        mat=auxSwap;

        iteraciones++;

        //printf("iteracion: %d \n",iteraciones);
      }

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
  M=(float*)malloc(sizeof(float)*N*N);
  mat=(float*)malloc(sizeof(float)*N*N);
  convergeVec=(int*)malloc(sizeof(int)*T);

  srand(time(NULL));
    //Inicializa el vector con numeros random
    for(i=0;i<N*N;i++){
       M[i]=(float)rand()/(float)(RAND_MAX/1);
       //printf("%f\n",M[i]);

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

 free(M);
 free(mat);

 return 0;
}
