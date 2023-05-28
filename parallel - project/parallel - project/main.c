#include "header.h"
#include "mpi_datatypes.h"




int main(int argc, char *argv[]) 
{

	//get time measure
	clock_t start, end;
	double time;
		 
	int rank, size, tag = 0;
	MPI_Status status;

	  // Initialize MPI
	MPI_Init(&argc, &argv);

	  
	  // Get the rank of the current process and the total number of processes
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	
	if (rank == 0) 
	{
		// Start measuring time
	    clock_t start = clock();
		// Read input data from file
		struct FileData* fileReadData = (struct FileData*)malloc(sizeof(struct FileData));
		MALLOC_SUCCSSED(fileReadData);
		readInputFile("input.txt", fileReadData);

		

		// Send number of objects and pictures to all processes
		int num_objects = fileReadData->num_objects;
		int num_pictures = fileReadData->num_pictures;
		transferSendInput(fileReadData->matching_value, num_objects, num_pictures, fileReadData->objects,1);


		

		int perProc = num_pictures % 2 == 0 ?  num_pictures/2 : num_pictures/2 + 1; // - for even/odd num_pictures
		MPI_Send(&perProc ,1 , MPI_INT, 1,0 ,MPI_COMM_WORLD);  // send per processes the number of work to do
		sendPicToSlave(fileReadData->pictures, perProc); // send in a func to the slaves the actull data perProc

		// Allocate memory for output
		int** resultToFile = (int**)malloc(num_pictures*sizeof(int*));
		MALLOC_SUCCSSED(resultToFile);
		for (int i = 0; i < num_pictures; i++) 
		{
			resultToFile[i] = (int *) malloc(RESULT_COLUMN * sizeof(int));
			MALLOC_SUCCSSED(resultToFile[i]);
		}

		for (int i = perProc ; i < num_pictures; i++ ) //searching from perProc index to the end and worker from index = 0 to perProc
		{
			mainSearchFunc( resultToFile[i],fileReadData->pictures[i],fileReadData->objects,num_objects, fileReadData->matching_value, rank); //  search all objects inside the image
		}
		
		
		for(int i = 0; i < num_pictures; i++)
		{
			MPI_Recv(resultToFile[i],RESULT_COLUMN,MPI_INT, 1, MPI_ANY_TAG,MPI_COMM_WORLD,MPI_STATUS_IGNORE); // get from worker the results
		}


		//Write results to file
		write_output_file(resultToFile, num_pictures,RESULT_COLUMN);
		printf( "DONE writing results\n");	

		clock_t end = clock();

		// Calculate elapsed time in seconds
		double time = (double)(end - start) / CLOCKS_PER_SEC;
		printf("Execution time: %f seconds\n", time);
		//Free memory
		for (int i = 0; i < num_pictures; i++) 
		{
			free(resultToFile[i]);
		}
		free(resultToFile);
		free(fileReadData->objects);
		free(fileReadData->pictures);
		free(fileReadData);

	}
	if (rank == 1)
	{
		// Receive number of objects and pictures
		int objNum, picNum;
		int workingInProcc = 1;
	  	float matching_value;
		int sizePerProc;
		Object* objects = MPIrecvData(0, &objNum, &matching_value, &picNum);

		int** resultToFile = (int**)malloc(picNum * sizeof(int*));
		MALLOC_SUCCSSED(resultToFile);
		for (int i = 0; i < picNum; i++) 
		{
			resultToFile[i] = (int *) malloc(RESULT_COLUMN * sizeof(int));
			MALLOC_SUCCSSED(resultToFile[i]);
		}
		printf("There is %d objects,\nmatching value is %f,\nnumber of pictures are %d\n",objNum, matching_value, picNum);
	  	
      	MPI_Recv(&sizePerProc ,1 , MPI_INT, 0,MPI_ANY_TAG ,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		Picture* pictures = (Picture*)malloc(sizeof(Picture) * sizePerProc); 
	  	MALLOC_SUCCSSED(pictures);
		picRecv(pictures, sizePerProc);
		
		for (int i = 0 ; i < sizePerProc; i ++ ) 
		{
			mainSearchFunc( resultToFile[i],pictures[i],objects,objNum, matching_value, rank); //  search all objects inside the image		
		}

		for(int i = 0; i < picNum; i++)
		{
			MPI_Send(resultToFile[i],RESULT_COLUMN,MPI_INT, 0, 0,MPI_COMM_WORLD);
		}
			
		printf( "DONE sending results\n");
		for (int i = 0; i < picNum; i++) 
		{
			free(resultToFile[i]);
		}
		free(resultToFile);
		free(pictures);
		free(objects);	
	}
	MPI_Finalize();
	return 0;
}



