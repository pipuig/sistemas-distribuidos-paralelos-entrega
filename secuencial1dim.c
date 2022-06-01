#include<stdio.h>
#include<stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <time.h>

double dwalltime();
int iteraciones =0;
float tercio=1.0/3.0;
float* arr;
void secuencialUnaDim (float* V, int size){
    int i, contador = 0;
    float *swapAux;
    int converge = 0;
    while (converge == 0){ //se repite el proceso hasta encontrar la convergencia
        converge = 1; //inicio suponiendo que el vector es convergente
        arr[0] = (V[0] + V[1])/2;
        arr[size-1] = (V[size-1] + V[size-2])/2;
        if (fabs((arr[0]-arr[size-1])) > 0.01) converge = 0;

        for (i=1; i < size-1; i++){ //calculo los promedios en un vector auxiliar (antes del for los casos particulares)
            arr[i] = (V[i-1] + V[i] + V[i+1])*tercio;
            //printf("%f\n", arr[i]);
             if (fabs((arr[0]-arr[i])) > 0.01){ 
                //printf("%f\n", arr[0]-arr[i]);
                converge = 0; //si alguno se pasa del valor de precision vuelvo a realizar la reducción
                break;
                }
            }
        for (;i<size-1;i++){
            arr[i] = (V[i-1] + V[i] + V[i+1])*tercio;
            }
            //printf("%f\n", arr[0]);
            swapAux=V;
            V=arr;
            arr=swapAux;
            iteraciones++;
            //printf("iteracion %d\n", iteraciones++);
        }
}

int main(int argc, char *argv[]){
   int N = atoi(argv[1]);
   //srand(time(NULL));
   double timetick;
   float a;
   int i,j;
   float* arreglo = malloc(N*sizeof(float));
   arr = malloc(N*sizeof(float));
   //matriz = malloc(N*N*sizeof(float));
   for(i = 0; i<N; i++){
       arreglo[i] = (float)rand()/((float)RAND_MAX);
       //printf("%f - ", arreglo[i]);
   }
    timetick = dwalltime();
    secuencialUnaDim(arreglo, N);
    free(arreglo);
    printf("\nTamaño del vector: %d\ntiempo en segundos %f con %d iteraciones\n",N, dwalltime()-timetick, iteraciones);
    return 0;
}


double dwalltime(){
        double sec;
        struct timeval tv;
        gettimeofday(&tv,NULL);
        sec = tv.tv_sec + tv.tv_usec/1000000.0;
        return sec;
}