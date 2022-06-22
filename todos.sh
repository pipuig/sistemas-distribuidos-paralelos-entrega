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
echo""
echo "8 threads"
echo""
echo "una dimension 512 1024 y 2048"
./vectorPthreads 512 8
./vectorPthreads 1024 8
./vectorPthreads 2048 8
echo""
echo "dos dimensiones 512 1024 y 2048"
./matrizPthreads 512 8
./matrizPthreads 1024 8
./matrizPthreads 2048 8