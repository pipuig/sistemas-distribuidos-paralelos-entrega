# TP Sistemas Convergencia

A hacer:

- [ ] Entregaron archivos de más, solo debían entregar los 7 archivos, los 3 codigos mas el informe.

- [X] El informe esta incompleto:

       - Para pthreads no indican como los hilos se reparten la matriz, solo indican que cada uno calcula una porcion.
       - Para mpi, indica que el proceso root reparte en partes la matriz o el vector con un scatter y tambien que cada hilo manda sus valores de frontera, pero no indica como es cada parte y la explicación es bastante breve

       - Detectan el problema de frontera pero para pthreads no explica como fue resuelto. Para mpi en cambio si lo explica de manera breve.

- [X] La estrategia paralela no es la misma que la secuencial, esto es muy imporante. En mpi de 1 dimension calculan convergencia al final cuando en el secuencial calculan mientras van procesando.  

- [X] En pthreads solo el root deberia hacer swap de las matrices, no todos.

- [X] En pthreads tienen tres if que podrian utilzar con else para que no haga todos los chequeos en todas las iteraciones

- [X] En MPI hacen los receives antes que los sends excepto en uno de los procesos “para que no haya deadlock”, creo que los sends no son sincronicos por lo que eso no seria necesario, pero si asi fueran podrian ahorrarse ese overhead si utilizaran ISend con un wait. ** preguntar si asi esta bien o hay que hacerlo si o si con isend **


- [ ] En MPI uilizan variables globales, esto esta mal, deberian pasar como parametro las variables que necesiten las funciones. ** hay que pasar la variable partes N/T como parametro **

- [ ] Podrían incluir una validacion en el informe indicando por ejemplo la cantidad de iteraciones de cada algoritmo

Algo raro que noté es que los tiempos son mas bajos de lo que deberían, ejecuté su algoritmo en mi máquina y me dieron mucho mas bajos de lo que ustedes tomaron, quizás les faltó poner bien algún parámetro en la ejecución en el cluster.
