#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <sys/time.h>
#include <time.h>
#define NUM_IT 1000000

double dwalltime();
int N; //tamaño de la matriz
float *M, *part;
float tercio=1.0/3.0;
int partes;


void funcionA(int id){

	M = (float *)malloc(N*N*sizeof(float));
	float* aux;
	float* auxM = (float *) malloc(partes*N*sizeof(float));
	part = (float *) malloc(partes*N*sizeof(float));
	float* filaSig = (float *) malloc(N*sizeof(float));
	int i,j,iteraciones=0, convergeLocal=1, convergeGlobal=0;
	srand(time(NULL));

    //Inicializa la matriz con numeros random
    for(i=0;i<N*N;i++){
       M[i]=(float)rand()/(float)(RAND_MAX/1);
       //printf("%f\n",M[i]);
    }

	MPI_Scatter(M, partes*N, MPI_FLOAT, part, partes*N, MPI_FLOAT, 0, MPI_COMM_WORLD);

	while(convergeGlobal == 0){
		convergeLocal=1;

		//mandar fila al siguiente thread
		MPI_Recv(&filaSig[0], N, MPI_FLOAT, id+1, 30, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		MPI_Send(&part[(partes-1)*N], N, MPI_FLOAT, id+1, 31, MPI_COMM_WORLD);
		//hacer calculos de root, estos calculos se basan en que partes sea mayor a 2 sino se complican las cosas

		//hacer esquina
		auxM[0]=(part[0]+part[1]+
						part[N]+part[N+1])/4;
		//hacer lo del medio
		for (i=1;i<N-1;i++){
				auxM[i] = (part[i-1] + part[i] + part[i+1] + part[i-1+N] + part[i+N] + part[i+1+N])/6; //chequear si converge
		}
		//hacer otra esquina
		auxM[N-1] = (part[N-1] + part[N-2] + part[(2*N)-2] + part[(2*N)-1])/4;
		//hacer mi parte pensado para no numeros chicos
		for (i=1;i<partes-1;i++){
				auxM[i*N] = (part[(i-1)*N] + part[(i-1)*N+1]
										+ part[i*N] + part[i*N+1] +
										part[(i+1)*N] + part[(i+1)*N+1])/6;
				for(j=1;j<N-1;j++){
					 auxM[i*N+j] = (part[(i-1)*N+(j-1)] + part[(i-1)*N+j] + part[(i-1)*N+j+1]
											+ part[(i)*N+(j-1)] + part[(i)*N+j] + part[(i)*N+j+1]
											+ part[(i+1)*N+(j-1)] + part[(i+1)*N+j] + part[(i+1)*N+j+1])/9;
				}
				auxM[(i+1)*N-1] = (part[(i-1)*N + (N-2)] + part[(i-1)*N + (N-1)]
											 + part[i*N + (N-2)] + part[i*N + (N-1)]
											 + part[(i+1)*N + (N-2)] + part[(i+1)*N + (N-1)])/6;
		}

		//necesito la fila siguiente
		i=partes-1;
		auxM[i*N] = (part[(i-1)*N] + part[(i-1)*N+1]
								+ part[i*N] + part[i*N+1] +
								filaSig[0] + filaSig[1])/6;
		for(j=1;j<N-1;j++){
		  auxM[i*N+j] = (part[(i-1)*N+(j-1)] + part[(i-1)*N+j] + part[(i-1)*N+j+1]
				  					+ part[(i)*N+(j-1)] + part[(i)*N+j] + part[(i)*N+j+1]
			  						+ filaSig[(j-1)] + filaSig[j] + filaSig[j+1])/9;
		}
		auxM[(i+1)*N-1] = (part[(i-1)*N + (N-2)] + part[(i-1)*N + (N-1)]
									 + part[i*N + (N-2)] + part[i*N + (N-1)]
									 + filaSig[N-2] + filaSig[N-1])/6;
    //printf("hilo %d numero %f \n",id,auxM[N]);
		//enviar primer valor a todos
	  MPI_Bcast(&auxM[0], 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
		//hacer las comparaciones
		for (i=0;i<(partes*N);i++){
			if (fabs(auxM[0]-auxM[i])>0.01){
				//printf("hilo %d numero %f pos %d \n",id,auxM[i],i);
				convergeLocal=0;
				break;
			}
		}
		aux = part;
		part = auxM;
		auxM = aux;
		iteraciones++;

		/*for(i=0;i<partes*N;i++){
			printf("hilo %d numero %f pos %d \n",id,part[i],i);

		}*/
		//reduccion
		MPI_Allreduce(&convergeLocal, &convergeGlobal, 1, MPI_INT, MPI_LAND, MPI_COMM_WORLD);
	}
	MPI_Gather(part, partes*N, MPI_FLOAT, M, partes*N, MPI_FLOAT, 0, MPI_COMM_WORLD);
  printf("iteracion: %d \n",iteraciones);
}

void funcionB(int id,int T){
	float* auxM = (float *)malloc(partes*N*sizeof(float));
	part = (float *)malloc(partes*N*sizeof(float));
	float* filaSig = (float *)malloc(N*sizeof(float));
	float* filaAnt = (float *)malloc(N*sizeof(float));
	int i,j ,convergeLocal=1, convergeGlobal=0;
	float v0, *aux;
	MPI_Scatter(M, partes*N, MPI_FLOAT, part, partes*N, MPI_FLOAT, 0, MPI_COMM_WORLD);
  while(convergeGlobal==0){
  	convergeLocal=1;

	if (id!=T-1){
		MPI_Recv(&filaSig[0], N, MPI_FLOAT, id+1, 30, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		MPI_Send(&part[(partes-1)*N], N, MPI_FLOAT, id+1, 31, MPI_COMM_WORLD);
		MPI_Send(&part[0], N, MPI_FLOAT, id-1, 30, MPI_COMM_WORLD);
		MPI_Recv(&filaAnt[0], N, MPI_FLOAT, id-1, 31, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	}
	else {
		MPI_Send(&part[0], N, MPI_FLOAT, id-1, 30, MPI_COMM_WORLD);
		MPI_Recv(&filaAnt[0], N, MPI_FLOAT, id-1, 31, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	}

	//hacer calculos primero los que son comunes al ultimo y a los otros threads
	//hago la primer fila que necesita el anterior

	auxM[0] = (filaAnt[0] + filaAnt[1]
							+ part[0] + part[1] +
							part[N] + part[N+1])/6;
	for(j=1;j<N-1;j++){
		 auxM[j] = (filaAnt[j-1] + filaAnt[j] + filaAnt[j+1]
	    							+ part[(j-1)] + part[j] + part[j+1]
		       					+ part[N+(j-1)] + part[N+j] + part[N+j+1])/9;
	}
	auxM[N-1] = (filaAnt[N-2] + filaAnt[N-1]
								 + part[N-2] + part[N-1]
								 + part[2*N -2] + part[2*N -1])/6;

	//hagos los bloques que me tocan del for
	for (i=1;i<partes-1;i++){
			auxM[i*N] = (part[(i-1)*N] + part[(i-1)*N+1]
									+ part[i*N] + part[i*N+1] +
									part[(i+1)*N] + part[(i+1)*N+1])/6;
			for(j=1;j<N-1;j++){
				 auxM[i*N+j] = (part[(i-1)*N+(j-1)] + part[(i-1)*N+j] + part[(i-1)*N+j+1]
										+ part[(i)*N+(j-1)] + part[(i)*N+j] + part[(i)*N+j+1]
										+ part[(i+1)*N+(j-1)] + part[(i+1)*N+j] + part[(i+1)*N+j+1])/9;
			}
			auxM[(i+1)*N-1] = (part[(i-1)*N + (N-2)] + part[(i-1)*N + (N-1)]
										 + part[i*N + (N-2)] + part[i*N + (N-1)]
										 + part[(i+1)*N + (N-2)] + part[(i+1)*N + (N-1)])/6;
	}

  if(id != T-1){ //si no soy el ultimo hilo
		//necesito la fila siguiente
		i=partes-1;
		auxM[i*N] = (part[(i-1)*N] + part[(i-1)*N+1]
								+ part[i*N] + part[i*N+1] +
								filaSig[0] + filaSig[1])/6;
		for(j=1;j<N-1;j++){
		  auxM[i*N+j] = (part[(i-1)*N+(j-1)] + part[(i-1)*N+j] + part[(i-1)*N+j+1]
				  					+ part[(i*N)+(j-1)] + part[(i*N)+j] + part[(i*N)+j+1]
			  						+ filaSig[(j-1)] + filaSig[j] + filaSig[j+1])/9;
		}
		auxM[(i+1)*N-1] = (part[(i-1)*N + (N-2)] + part[(i-1)*N + (N-1)]
									 + part[i*N + (N-2)] + part[i*N + (N-1)]
									 + filaSig[N-2] + filaSig[N-1])/6;

	}else{
		//si soy el ultimo hilo
		//hago la esquina izquierda
		auxM[(partes-1)*N] = (part[(partes-1)*N] + part[(partes-1)*N+1] + part[(partes-2)*N] + part[N*(partes-2)+1] )/4;
		//hago lo del medio
		for(i=(partes-1)*N+1;i<partes*N;i++){
			auxM[i] = (part[i-1] + part[i] + part[i+1] + part[i-1-N] + part[i-N] + part[i+1-N])/6; //chequear si converge
		}
		//hago la ultima esquina
		auxM[(partes*N)-1] = (part[(partes*N)-1] + part[(partes-1)*N-1] + part[(partes-1)*N-2]
							 + part[(partes*N)-2])/4;
	}

	//recibir primer valor
	MPI_Bcast(&v0, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
	//hacer las comparaciones
	for (i=0;i<partes*N;i++){
		if (fabs(v0-auxM[i])>0.01){
			//printf("hilo %d numero %f pos %d \n",id,auxM[i],i);
			convergeLocal=0;
			break;
		}
	}

	//swap
	aux = part;
	part = auxM;
	auxM = aux;

	/*for(i=0;i<partes*N;i++){
		printf("hilo %d numero %f pos %d \n",id,part[i],i);

	}*/

	//reduccion
	MPI_Allreduce(&convergeLocal, &convergeGlobal, 1, MPI_INT, MPI_LAND, MPI_COMM_WORLD);
  }
	MPI_Gather(part, partes*N, MPI_FLOAT, M, partes*N, MPI_FLOAT, 0, MPI_COMM_WORLD);
}


int main(int argc, char** argv){
	int miID;
	int T; //cantidad de procesos
  double timetick;
	timetick=dwalltime();
	MPI_Init(&argc, &argv); // Inicializa el ambiente. No debe haber sentencias antes
	MPI_Comm_rank(MPI_COMM_WORLD,&miID); // Obtiene el identificador de cada proceso (rank)
	MPI_Comm_size(MPI_COMM_WORLD,&T); // Obtiene el numero de procesos
	N= atoi(argv[1]);
	partes = N/T;
	if (miID == 0) funcionA(miID);
	else funcionB(miID,T);

	MPI_Finalize(); // Finaliza el ambiente MPI. No debe haber sentencias después
	printf("\nTiempo en segundos: %f\n", dwalltime()-timetick);
	return(0); // Luego del MPI_Finalize()
	}

	double dwalltime(){
	        double sec;
	        struct timeval tv;

	        gettimeofday(&tv,NULL);
	        sec = tv.tv_sec + tv.tv_usec/1000000.0;
	        return sec;
	}
