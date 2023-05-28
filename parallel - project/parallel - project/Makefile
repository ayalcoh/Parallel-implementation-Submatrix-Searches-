build:
	mpicc  -fopenmp -c files_handlers.c -o files_handlers.o
	mpicc -c openMPObjectcsSearch.c -o openMPObjectcsSearch.o
	mpicc -c mpi_SendRecv.c -o mpi_SendRecv.o
	mpicc  -fopenmp -c main.c -o main.o
	mpicc  -fopenmp main.o files_handlers.o mpi_SendRecv.o openMPObjectcsSearch.o -L/usr/local/cuda-11.7/targets/x86_64-linux/lib -lcudart -o main_prog

clean:
	rm -f *.o ./main_prog
run:
	mpirun -np 2 ./main_prog
