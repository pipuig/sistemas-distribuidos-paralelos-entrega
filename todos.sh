#!/bin/bash
echo "SECUENCIALES"
echo "una dimension 512 1024 y 2048"
./secuencial1dim 512
./secuencial1dim 1024
./secuencial1dim 2048
echo""
echo "dos dimensiones 512 1024 y 2048"
./secuencial2dim 512
./secuencial2dim 1024
./secuencial2dim 2048
echo""
echo "PTHREADS"
echo "2 threads"
echo""
echo "una dimension 512 1024 y 2048"
./vectorPthreads 512 2
./vectorPthreads 1024 2
./vectorPthreads 2048 2
echo""
echo "dos dimensiones 512 1024 y 2048"
./matrizPthreads 512 2
./matrizPthreads 1024 2
./matrizPthreads 2048 2
echo""
echo "4 threads"
echo""
echo "una dimension 512 1024 y 2048"
./vectorPthreads 512 4
./vectorPthreads 1024 4
./vectorPthreads 2048 4
echo""
echo "dos dimensiones 512 1024 y 2048"
./matrizPthreads 512 4
./matrizPthreads 1024 4
./matrizPthreads 2048 4