This repository contains some programms. 
First programm is 'createMatrixFile.c'. That programm has save a lot of time generating my own matrix.txt file.
    Using: compile and run programm. It will create file 'data.txt' with matrix, wich size was taken from proggram.

Second programm is 'symmetry_check.c'. It using prhread library to make checking symmetry of two matrixes(common and transponned) multi-thread. The main goal of this programm is to prove that summing common and transpon matrixes will became a symmetry matrix.  
    Using: compile and run programm. it will ask you for matrix size(n) and name file with matirix to read .Important that this file with matrix has to be located in one directory with programm. After that, programm will ask you count of threads you want to create to check symmetry. 

Third programm is 'MPI_matrix_sum.c'. This programm using MPI library for creating parallel processes. The main goal of programm is to realize data exchanging with all processes and distribute elements equally with processes. 
In first process programm reads file, making transpon matrix and creating processes, then send them data, they has to recive and send back. 