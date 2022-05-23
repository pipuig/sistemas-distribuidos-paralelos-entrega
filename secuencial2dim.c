#include<stdio.h>
#include<stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <time.h>

double dwalltime();
int iteraciones =0;

/*if (fabs(mat[0] - mat[i*N+j])){
   converge = 0;
   break
}*/

void secuencialDosDim(float* M, int N){
    int i,j, converge = 0;
    float* mat = malloc(sizeof(float)*N*N);
    float* auxSwap;
    while (converge == 0){
        converge = 1;
        mat[0] = (M[0] + M[1] + M[N] + M[N+1])/4;
        for (i=0;i<N-1;i++){
            mat[i] = (M[i-1] + M[i] + M[i+1] + M[i-1+N] + M[i+N] + M[i+1+N])/6; //chequear si converge
        }
        mat[N-1] = (M[N-1] + M[N-2] + M[(2*N)-2] + M[(2*N)-1])/4;
        
        for (i=1;i<N-1;i++){
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

        mat[N*(N-1)] = (M[N*(N-1)] + M[N*(N-2)] + M[N*(N-2)+1] + M[N*(N-1)+1])/4;

        for (i=(N-1)*N +1 ; i<N*N -1 ;i++){
            mat[i] = (M[i-1] + M[i] + M[i+1] + M[i-1-N] + M[i-N] + M[i+1-N])/6; //chequear si converge
        }

        mat[N*N-1] = (M[N*N-1] + M[N*(N-1)-1] + M[N*(N-1)-2] + M[N*N-2])/4;

        for (i=0;i<N*N;i++){
            if (fabs(mat[0] - mat[i]) > 0.01){
               converge = 0;
               break;
               }
            }
         auxSwap=M;
         M=mat;
         mat=auxSwap;
         iteraciones++;
        }
        /*for (i=0;i<N;i++){
            for(j=0;j<N;j++){
               printf("%f ", mat[i*N+j]);
            } 
            printf("\n");
         }*/
    }




int main(int argc, char *argv[]){
   int N = atoi(argv[1]);
   srand(time(NULL));
   double timetick;
   float a;
   int i;
   float* matriz = malloc(N*N*sizeof(float));
   for(i = 0; i<N*N; i++){
       matriz[i] = (float)rand()/((float)RAND_MAX);
   }
    timetick = dwalltime();
    secuencialDosDim(matriz, N);
    printf("\ntiempo en segundos %f con %d iteraciones\n", dwalltime()-timetick, iteraciones);
    free(matriz);
    return 0;
}

double dwalltime(){
        double sec;
        struct timeval tv;
        gettimeofday(&tv,NULL);
        sec = tv.tv_sec + tv.tv_usec/1000000.0;
        return sec;
}