#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <sys/time.h>
#include <time.h>

int N; //tamaño de la matriz
float *M, *part;
float tercio=1.0/3.0;
int partes;


void funcionA(int id){
	M = malloc(N*N*sizeof(float));
	float* aux;
	float* auxM = malloc(partes*sizeof(float));
	float* filaSig = malloc(N*sizeof(float));
	int i, convergeLocal=1, convergeGlobal=0;
	srand(time(NULL));
    //Inicializa la matriz con numeros random
    for(i=0;i<N*N;i++){
       M[i]=(float)rand()/(float)(RAND_MAX/1);
       printf("%f\n",M[i]);
    }
	part = malloc(partes*sizeof(float));
	MPI_Scatter(M, partes, MPI_FLOAT, part, partes, MPI_FLOAT, 0, MPI_COMM_WORLD);
	while(convergeGlobal == 0){
		//mandar fila al siguiente thread
		MPI_Recv( &filaSig, N, MPI_FLOAT, id+1, 30, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		MPI_Send(&part[partes-1], N, MPI_FLOAT, id+1, 31, MPI_COMM_WORLD);
		//hacer calculos de root

		//hacer esquina
		//hacer lo del medio
		//hacer otra esquina
		//hacer mi parte

		//enviar primer valor a todos
		MPI_Bcast(&auxM[0], 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
		//hacer las comparaciones
		for (i=0;i<partes*N-1;i++){
			if (fabs(auxM[0]-auxM[i])>0.01){
				convergeLocal=0;
				break;
			}
		}
		aux = part;
		part = auxM;
		auxM = aux;
		//reduccion
		MPI_Allreduce(&convergeLocal, &convergeGlobal, 1, MPI_INT, MPI_LAND, MPI_COMM_WORLD);
	}
	MPI_Gather(part, partes, MPI_FLOAT, M, partes, MPI_FLOAT, 0, MPI_COMM_WORLD);
}

void funcionB(int id,int T){
	float* auxM = malloc(partes*sizeof(float));
	float* filaSig = malloc(N*sizeof(float));
	float* filaAnt = malloc(N*sizeof(float));
	part = malloc(partes*sizeof(float));
	int i, convergeLocal=1, convergeGlobal=0;
	float v0, *aux;
	MPI_Scatter(M, partes, MPI_FLOAT, part, partes, MPI_FLOAT, 0, MPI_COMM_WORLD);
  while(convergeGlobal==0){
	if (id!=T-1){
		MPI_Recv(&filaSig, N, MPI_FLOAT, id+1, 30, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		MPI_Send(&part[partes-1], N, MPI_FLOAT, id+1, 31, MPI_COMM_WORLD);
		MPI_Send(&part, N, MPI_FLOAT, id-1, 30, MPI_COMM_WORLD);
		MPI_Recv(&filaAnt, N, MPI_FLOAT, id-1, 31, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	}
	else {
		MPI_Send(&part, N, MPI_FLOAT, id-1, 30, MPI_COMM_WORLD);
		MPI_Recv(&filaAnt, N, MPI_FLOAT, id-1, 31, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	}
	//hacer calculos


	//recibir primer valor
	MPI_Bcast(&v0, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
	//hacer las comparaciones
	for (i=0;i<partes*N-1;i++){
		if (fabs(v0-auxM[i])>0.01){
			convergeLocal=0;
			break;
		}
	}

	//swap
	aux = part;
	part = auxM;
	auxM = aux;

	//reduccion
	MPI_Allreduce(&convergeLocal, &convergeGlobal, 1, MPI_INT, MPI_LAND, MPI_COMM_WORLD);
  }
	MPI_Gather(part, partes, MPI_FLOAT, M, partes, MPI_FLOAT, 0, MPI_COMM_WORLD);
}


int main(int argc, char** argv){
	int miID;
	int T; //cantidad de procesos

	MPI_Init(&argc, &argv); // Inicializa el ambiente. No debe haber sentencias antes
	MPI_Comm_rank(MPI_COMM_WORLD,&miID); // Obtiene el identificador de cada proceso (rank)
	MPI_Comm_size(MPI_COMM_WORLD,&T); // Obtiene el numero de procesos
	N= atoi(argv[1]);
	partes = N/T;
	if (miID == 0) funcionA(miID);
	else funcionB(miID,T);

	MPI_Finalize(); // Finaliza el ambiente MPI. No debe haber sentencias después
	return(0); // Luego del MPI_Finalize()
	}
