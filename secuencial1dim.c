#include<stdio.h>
#include<stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <time.h>

double dwalltime();
int iteraciones =0;

float absoluto(float a){
    printf("%f ", a);
    if (a > 0) return a;
    else return a*(-1);
}

void secuencialUnaDim (float* V, int size){
    int i, contador = 0;
    float *swapAux, * arr = malloc(size*sizeof(float));
    int converge = 0;
    while (converge == 0){ //se repite el proceso hasta encontrar la convergencia
        converge = 1; //inicio suponiendo que el vector es convergente
        arr[0] = (V[0] + V[1])/2;
        arr[size-1] = (V[size-1] + V[size-2])/2;
        if (fabs((arr[0]-arr[size-1])) > 0.01) converge = 0;

        for (i=1; i < size-1; i++){ //calculo los promedios en un vector auxiliar (antes del for los casos particulares)
            arr[i] = (V[i-1] + V[i] + V[i+1])/3;
             if (converge != 0 && fabs((arr[0]-arr[i])) > 0.01){ 
                converge = 0; //si alguno se pasa del valor de precision vuelvo a realizar la reducción
                break;
                }
            }
        for (;i<size-1;i++){
            arr[i] = (V[i-1] + V[i] + V[i+1])/3;
            }
        
        /*for (i=0; i < size; i++){
            V[i] = arr[i];
            }*/
            swapAux=V;
            V=arr;
            arr=swapAux;
        /*for (i=0; i < size; i++){ //verifico convergencia del vector resultante
            if (fabs((V[0]-V[i])) > 0.01){ 
                converge = 0; //si alguno se pasa del valor de precision vuelvo a realizar la reducción
                break;
                }
            }*/
            iteraciones++;
        }
        /*printf("\n %d\n", contador);
        for (i=0; i < size; i++){
            printf ("%f ", V[i]);
            }
        printf("\n");*/
}

/*void secuencialDosDim(float* M, int N){
    int i,j, converge = 0;
    float mat = malloc(sizeof(float)*N*N);
    while (converge == 0){
        converge = 1;
        mat[0] = (M[0] + M[1] + M[N] + M[N+1])/4;
        mat[N-1] = (M[N-1] + M[N-2] + M[2*N-2] + M[2*N-1])/4;
        mat[N*(N-1)] = (M[N*(N-1)] + M[N*(N-2)] + M[N*(N-2)+1] + M[N*(N-1)+1])/4;
        mat[N*N-1] = (M[N*N-1] + M[N*(N-1)-1] + M[N*(N-1)+(N-2)] + M[N*(N-2)+(N-1)])/4;
        for (i=1;i<N-1;i++){
            for(j=0;j<N-1;j++){

            }
        }
        }
    }
*/
int main(int argc, char *argv[]){
   int N = atoi(argv[1]);
   srand(time(NULL));
   double timetick;
   float a;
   int i,j;
   float* arreglo = malloc(N*sizeof(float));
   //matriz = malloc(N*N*sizeof(float));
   for(i = 0; i<N; i++){
       arreglo[i] = (float)rand()/((float)RAND_MAX);
       printf("%f - ", arreglo[i]);
   }
    timetick = dwalltime();
    secuencialUnaDim(arreglo, N);
    free(arreglo);
    printf("\ntiempo en segundos %f con %d iteraciones\n", dwalltime()-timetick, iteraciones);
    return 0;
}


double dwalltime(){
        double sec;
        struct timeval tv;
        gettimeofday(&tv,NULL);
        sec = tv.tv_sec + tv.tv_usec/1000000.0;
        return sec;
}

/*void secuencialDosDim(float ** V, int size){
    int i,j;
    float** mat = malloc(sizeof(float)*size*size);
    int converge = 0;
    while (converge==0){
        converge =1;
        //comienzo calculando los casos de las esquinas
        mat[0][0]=(V[0][0] + V[0][1] + V[1][1] + V[1][0])/4; //sup izq
        mat[0][size-1]=(V[0][size-1] + V[0][size-2] + V[1][size-1] + V[1][size-2])/4; //sup der
        mat[size-1][size-1]=(V[size-1][size-1] + V[size-2][size-1] + V[size-2][size-2] + V[size-1][size-2])/4; //inf der
        mat[size-1][0]=(V[size-1][0] + V[size-1][1] + V[size-2][0] + V[size-2][1])/4; //inf izq

        //siguiente calculo los lados
        for (i=1;i<=size-2;i++){ //lados izq y der
            mat[i][0] = (V[i-1][0] + V[i][0] + V[i+1][0] + V[i-1][1] + V[i][1] + V[i+1][1])/6;
            mat[i][size-1] = (V[i-1][size-1] + V[i][size-1] + V[i+1][size-1] + V[i-1][size-2] + V[i][size-2] + V[i+1][size-2])/6;
        }
        for (i=1;i<=size-2;i++){ //lados sup e inf
            mat[0][i] = (V[0][i-1] + V[0][i] + V[0][i+1] + V[1][i-1] + V[1][i] + V[1][i+1])/6;
            mat[size-1][i] = (V[size-1][i-1] + V[size-1][i] + V[size-1][i+1] + V[size-2][i-1] + V[size-2][i] + V[size-2][i+1])/6;
        }
        for (i=1;i<=size-2;i++){ //calculo del resto de la matriz
            for (j=1;j<=size-2;j++){
                mat[i][j]= (V[i-1][j-1] + V[i-1][j] + V[i-1][j+1] + V[i][j-1] + V[i][j] + V[i][j+1] + V[i+1][j-1] + V[i+1][j] + V[i+1][j+1])/9;
            }
        }
        for (i=0;i<size;i++){ //reviso la convergencia
            for (j=0;j<size;j++){
                if (abs(mat[0][0] - mat[i][j]) > 0.01){
                    converge = 0;
                    break;
                }
                if (converge = 0) break;
            }
        }
        for (i=0;i<size;i++){ //una vez verificado que converge aplico los valores a la matriz original
            for (j=0;j<size;j++){
                V[i][j] = mat[i][j];
            }
        }
    }

}
*/
