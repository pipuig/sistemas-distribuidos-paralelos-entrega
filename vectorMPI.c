#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <sys/time.h>
#include <time.h>

double dwalltime();
int N; //tamaño del vector
float *vec, *part;
float tercio=1.0/3.0;


void funcionA(int id, int partes){
	vec = malloc(N*sizeof(float));
	float* aux;
	double timetick;
	float* auxVec = malloc(partes*sizeof(float));
	float* valor = malloc(sizeof(float));
	int i, convergeLocal=1, convergeGlobal=0;
	//srand(time(NULL));
    //Inicializa el vector con numeros random
    for(i=0;i<N;i++){
       vec[i]=(float)rand()/(float)(RAND_MAX/1);
       //printf("%f\n",vec[i]);
    }
	part = malloc(partes*sizeof(float));
	timetick=dwalltime();
	MPI_Scatter(vec, partes, MPI_FLOAT, part, partes, MPI_FLOAT, 0, MPI_COMM_WORLD);
	while(convergeGlobal == 0){
		convergeLocal=1;
		//enviar primer valor a todos
		MPI_Bcast(&auxVec[0], 1, MPI_FLOAT, 0, MPI_COMM_WORLD);

		MPI_Send(&part[partes-1], 1, MPI_FLOAT, id+1, 31, MPI_COMM_WORLD);
		MPI_Recv(&valor[0], 1, MPI_FLOAT, id+1, 30, MPI_COMM_WORLD,MPI_STATUS_IGNORE);

		//MPI_Recv( &valor[0], 1, MPI_FLOAT, id+1, 30, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		//MPI_Send(&part[partes-1], 1, MPI_FLOAT, id+1, 31, MPI_COMM_WORLD);
		//hacer calculos
		auxVec[0]=(part[0]+part[1])*0.5;
		for (i=1;i<partes-1;i++){
			auxVec[i]=(part[i-1]+part[i]+part[i+1])*tercio;
			if (fabs(auxVec[0]-auxVec[i])>0.01){
				convergeLocal=0;
				break;
				}
		}

		for (;i<partes-1,i++;){
			auxVec[i]=(part[i-1]+part[i]+part[i+1])*tercio;
		}

		auxVec[partes-1] = (part[partes-2]+part[partes-1]+valor[0])*tercio;
		if (convergeLocal && (fabs(auxVec[0]-auxVec[partes-1])>0.01)){
			convergeLocal=0;
		}
		//hacer las comparaciones
		/*for (i=0;i<partes-1;i++){
			if (fabs(auxVec[0]-auxVec[i])>0.01){
				convergeLocal=0;
				break;
			}
		}*/
		aux = part;
		part = auxVec;
		auxVec = aux;
		//reduccion
		MPI_Allreduce(&convergeLocal, &convergeGlobal, 1, MPI_INT, MPI_LAND, MPI_COMM_WORLD);
	}
	MPI_Gather(part, partes, MPI_FLOAT, vec, partes, MPI_FLOAT, 0, MPI_COMM_WORLD);
	printf("Tiempo: %lf\n", dwalltime()-timetick);
}

void funcionB(int id,int T, int partes){
	float* auxVec = malloc(partes*sizeof(float));
	float* valores = malloc(2*sizeof(float)); //contiene primer y ultimo valor (externos, de otros procesos)
	part = malloc(partes*sizeof(float));
	int i, convergeLocal=1, convergeGlobal=0;
	float v0, *aux;
	int iteraciones=0;
	MPI_Scatter(vec, partes, MPI_FLOAT, part, partes, MPI_FLOAT, 0, MPI_COMM_WORLD);
  while(convergeGlobal==0){
  	convergeLocal=1; //asumo que converge
	//recibir primer valor
	MPI_Bcast(&v0, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
	if (id % 2 == 0 && id !=T-1){
		MPI_Send(&part[0], 1, MPI_FLOAT, id-1, 30, MPI_COMM_WORLD);
		MPI_Recv(&valores[0], 1, MPI_FLOAT, id-1, 31, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		MPI_Send(&part[partes-1], 1, MPI_FLOAT, id+1, 31, MPI_COMM_WORLD);
		MPI_Recv(&valores[1], 1, MPI_FLOAT, id+1, 30, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	}
	else if (id % 2 == 1 && id != T-1) {
		MPI_Recv(&valores[1], 1, MPI_FLOAT, id+1, 30, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		MPI_Send(&part[partes-1], 1, MPI_FLOAT, id+1, 31, MPI_COMM_WORLD);
		MPI_Recv(&valores[0], 1, MPI_FLOAT, id-1, 31, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		MPI_Send(&part[0], 1, MPI_FLOAT, id-1, 30, MPI_COMM_WORLD);
	}
	else if (id == (T-1)){
		MPI_Send(&part[0], 1, MPI_FLOAT, id-1, 30, MPI_COMM_WORLD);
		MPI_Recv(&valores[0], 1, MPI_FLOAT, id-1, 31, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	}
	/*if (id!=T-1){
		MPI_Recv(&valores[1], 1, MPI_FLOAT, id+1, 30, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		MPI_Send(&part[partes-1], 1, MPI_FLOAT, id+1, 31, MPI_COMM_WORLD);
		MPI_Send(&part[0], 1, MPI_FLOAT, id-1, 30, MPI_COMM_WORLD);
		MPI_Recv(&valores[0], 1, MPI_FLOAT, id-1, 31, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	}
	else {
		MPI_Send(&part[0], 1, MPI_FLOAT, id-1, 30, MPI_COMM_WORLD);
		MPI_Recv(&valores[0], 1, MPI_FLOAT, id-1, 31, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	}*/
	//hacer calculos
	auxVec[0]=(valores[0]+part[0]+part[1])*tercio;
	for (i=1;i<partes-1;i++){
		auxVec[i]=(part[i-1]+part[i]+part[i+1])*tercio;
		if (fabs(v0-auxVec[i])>0.01){
			convergeLocal=0;
			break;
			}
		}
	for (;i<partes-1;i++){
		auxVec[i]=(part[i-1]+part[i]+part[i+1])*tercio;
		}

	if(id != T-1){ //si no es es el ultimo thread hace el promedio de 3 posiciones
		auxVec[partes-1] = (part[partes-2]+part[partes-1]+valores[1])*tercio;
	}else{
		auxVec[partes-1] = (part[partes-2]+part[partes-1])*0.5;
	}

	if (convergeLocal && (fabs(v0-auxVec[partes-1])>0.01)){ //calculo convergencia al final
		convergeLocal=0;
		}

	//hacer las comparaciones
	/*for (i=0;i<partes-1;i++){
		if (fabs(v0-auxVec[i])>0.01){
			convergeLocal=0;
			break;
		}
	}*/

	//swap
	aux = part;
	part = auxVec;
	auxVec = aux;
	//reduccion
	MPI_Allreduce(&convergeLocal, &convergeGlobal, 1, MPI_INT, MPI_LAND, MPI_COMM_WORLD);
  }
	MPI_Gather(part, partes, MPI_FLOAT, vec, partes, MPI_FLOAT, 0, MPI_COMM_WORLD);
}


int main(int argc, char** argv){
	int miID;
	int T; //cantidad de procesos
	MPI_Init(&argc, &argv); // Inicializa el ambiente. No debe haber sentencias antes
	MPI_Comm_rank(MPI_COMM_WORLD,&miID); // Obtiene el identificador de cada proceso (rank)
	MPI_Comm_size(MPI_COMM_WORLD,&T); // Obtiene el numero de procesos
	N= atoi(argv[1]);
	printf("Threads: %d - N: %d\n", T,N);
	if (miID == 0) funcionA(miID, N/T);
	else funcionB(miID,T, N/T);
	MPI_Finalize(); // Finaliza el ambiente MPI. No debe haber sentencias después
	return(0); // Luego del MPI_Finalize()
	}

double dwalltime(){
        double sec;
        struct timeval tv;

        gettimeofday(&tv,NULL);
        sec = tv.tv_sec + tv.tv_usec/1000000.0;
        return sec;
}
