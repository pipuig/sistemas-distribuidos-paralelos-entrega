#include<mpi.h>
#include<math.h>
#include<time.h>

float *vec, *part;
float tercio=1.0/3.0;
int partes;


void funcionA(){
	vec = malloc(N*sizeof(float));
	float* aux;
	float* auxVec = malloc(partes*sizeof(float));
	float* valor = malloc(sizeof(float));
	int i, convergeLocal=1, convergeGlobal=0;
	srand(time(NULL));
    //Inicializa el vector con numeros random
    for(i=0;i<N;i++){
       vec[i]=(float)rand()/(float)(RAND_MAX/1);
       printf("%f\n",vec[i]);
    }
	part = malloc(partes*sizeof(float));
	MPI_Scatter(vec, partes, MPI_FLOAT, part, partes, MPI_FLOAT, 0, MPI_COMM_WORLD);
	while(convergeGlobal == 0){
		MPI_Recv(valor[0], sizeof(float), MPI_FLOAT, id+1, 30, MPI_COMM_WORLD); 
		MPI_Send(part[partes-1], sizeof(float), MPI_FLOAT, id+1, 31, MPI_COMM_WORLD);
		//hacer calculos
		auxVec[0]=(part[0]+part[1])*0.5;
		for (i=1;i<partes-1;i++){
			auxVec[i]=(part[i-1]+part[i]+part[i+1])*tercio;
		}
		auxVec[partes-1] = (part[partes-2]+part[partes-1]+valor[0])*tercio;
		//enviar primer valor a todos
		MPI_BCast(auxVec[0], 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
		//hacer las comparaciones
		for (i=0;i<partes-1;i++){
			if (fabs(auxVec[0]-auxVec[i])>0.01){
				convergeLocal=0;
				break;
			}
		}
		aux = part;
		part = auxVec;
		auxVec = aux;
		//reduccion
		MPI_Allreduce(&convergeLocal, &convergeGlobal, 1, MPI_INT, MPI_LAND, MPI_COMM_WORLD);
	}
	MPI_Gather(part, partes, MPI_FLOAT, vec, partes, MPI_FLOAT, 0, MPI_COMM_WORLD);
}

void funcionB(int id){
	float* auxVec = malloc(partes*sizeof(float));
	float* valores = malloc(2*sizeof(float));
	part = malloc(partes*sizeof(float));
	MPI_Scatter(vec, partes, MPI_FLOAT, part, partes, MPI_FLOAT, 0, MPI_COMM_WORLD);
	if (id!=T-1){
		MPI_Recv(valores[1], sizeof(float), MPI_FLOAT, id+1, 30, MPI_COMM_WORLD); 
		MPI_Send(part[partes-1], sizeof(float), MPI_FLOAT, id+1, 31, MPI_COMM_WORLD);
		MPI_Send(part[0], sizeof(float), MPI_FLOAT, id-1, 30, MPI_COMM_WORLD); 
		MPI_Recv(valores[0], sizeof(float), MPI_FLOAT, id-1, 31, MPI_COMM_WORLD);
	}
	else {
		MPI_Send(part[0], sizeof(float), MPI_FLOAT, id-1, 30, MPI_COMM_WORLD); 
		MPI_Recv(valores[0], sizeof(float), MPI_FLOAT, id-1, 31, MPI_COMM_WORLD);
	}
	//hacer calculos
	//recibir primer valor
	//hacer las comparaciones
	//reduccion
}








int main(int argc, char** argv){
	int miID;
	int T; //cantidad de procesos
	int N; //tamaño del vector

	MPI_Init(&argc, &argv); // Inicializa el ambiente. No debe haber sentencias antes
	MPI_Comm_rank(MPI_COMM_WORLD,&miID); // Obtiene el identificador de cada proceso (rank)
	MPI_Comm_size(MPI_COMM_WORLD,&T); // Obtiene el numero de procesos
	N= atoi(argv[1]);
	partes = N/T;
	if (miID == 0) funcionA(miID);
	else funcionB(miID);


	/*if(miID == 0) funcionA(); // Función que implementa el root
	else if (miID >= 1 && miID <= K) funcionB(); // Función que implementa los procesos de tipo B*/
	MPI_Finalize(); // Finaliza el ambiente MPI. No debe haber sentencias después
	return(0); // Luego del MPI_Finalize()
	}