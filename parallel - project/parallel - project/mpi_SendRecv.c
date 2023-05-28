#include "header.h"
#include "mpi_datatypes.h"

void save_pixels_Picture(Picture* pic, int *pixels_array) {
    int i, j;
    for (i = 0; i < pic->size; i++) {
        for (j = 0; j < pic->size; j++) {
            *pixels_array++ = pic->pixels[i][j];
        }
    }
}

void obj_pixels_ToSave(Object *obj, int *arr) {
    int i, j;
    for (i = 0; i < obj->size; i++) {
        for (j = 0; j < obj->size; j++) {
            *arr++ = obj->pixels[i][j];
        }
    }
}

void transferSendInput(float match,int num_objects,int num_pictures,Object* objects, int rankDest)
{
    MPI_Send(&num_objects, 1, MPI_INT, rankDest, 0,MPI_COMM_WORLD);
    int* pixels = (int*)malloc(sizeof(int) * OBJECT_SIZE * OBJECT_SIZE); // cant send 2d array
    MALLOC_SUCCSSED(pixels);
    for (int i = 0 ; i < num_objects; i++) // saving pixels , sending each object id, size, and pixels as size of current obj
    {
        obj_pixels_ToSave(&objects[i], pixels);
        int sizeOfObj = objects[i].size;
        MPI_Send(&objects[i].id ,1 , MPI_INT, rankDest,0 ,MPI_COMM_WORLD);  
		MPI_Send(&sizeOfObj ,1 , MPI_INT, rankDest,0 ,MPI_COMM_WORLD); 
		MPI_Send(pixels , sizeOfObj* sizeOfObj , MPI_INT, rankDest,0 ,MPI_COMM_WORLD); 
    }
    MPI_Send(&match ,1 , MPI_FLOAT, rankDest,0 ,MPI_COMM_WORLD); 
	MPI_Send(&num_pictures ,1 , MPI_INT, rankDest,0 ,MPI_COMM_WORLD);
    free(pixels);
}



void sendPicToSlave(Picture* picture, int size)
{
    int* pixels = (int*)malloc(sizeof(int) * PICTURE_SIZE * PICTURE_SIZE); // convert to 1D array
    MALLOC_SUCCSSED(pixels);
    int sizeOfPic;
    for (int i = 0 ; i < size; i++) // saving each pictrue pixels, sending pic i and pixels as size of the pic
    {
        save_pixels_Picture(&picture[i], pixels);
        sizeOfPic = picture[i].size;
        MPI_Send(&picture[i].id ,1 , MPI_INT, 1,0 ,MPI_COMM_WORLD);  
		MPI_Send(&sizeOfPic ,1 , MPI_INT, 1,0 ,MPI_COMM_WORLD); 
		MPI_Send(pixels , sizeOfPic*sizeOfPic , MPI_INT, 1,0 ,MPI_COMM_WORLD); 
    }
    free(pixels);
}



void savePixelsAs2DArray_Object(int* pixels1D, int size, Object* obj) {
    // Iterate over each row in the 2D array
    for (int i = 0; i < size; i++) {
        // Iterate over each column in the current row
        for (int j = 0; j < size; j++) {
            // Compute the 1D index for the current pixel
            int index = i * size + j;
            // Save the pixel in the corresponding location in the 2D array
            obj->pixels[i][j] = pixels1D[index];
        }
    }
}

Object* MPIrecvData(int sourceRank,int* numObjs, float* matching_value, int* numPics)
{
	
	MPI_Recv(numObjs ,1 , MPI_INT, sourceRank,0 ,MPI_COMM_WORLD, MPI_STATUS_IGNORE);  
	int* pixels = (int*)malloc(sizeof(int) * OBJECT_SIZE * OBJECT_SIZE);
    MALLOC_SUCCSSED(pixels);
	Object* objects = (Object*)malloc(sizeof(Object)* (*numObjs));
	int objSize;
	for (int i =0; i < *numObjs; i++) //reciving objects, match, how many pics
	{
		MPI_Recv(&objects[i].id ,1 , MPI_INT, sourceRank,0 ,MPI_COMM_WORLD, MPI_STATUS_IGNORE);  
		MPI_Recv(&objSize ,1 , MPI_INT, sourceRank,0 ,MPI_COMM_WORLD, MPI_STATUS_IGNORE); 
		objects[i].size=objSize;
		MPI_Recv(pixels , objSize* objSize , MPI_INT, sourceRank,0 ,MPI_COMM_WORLD, MPI_STATUS_IGNORE); 
		savePixelsAs2DArray_Object(pixels, objects[i].size,&objects[i]);
	}
	free(pixels);
	MPI_Recv(matching_value ,1 , MPI_FLOAT, sourceRank,0 ,MPI_COMM_WORLD, MPI_STATUS_IGNORE);  
	MPI_Recv(numPics ,1 , MPI_INT, sourceRank,0 ,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	  
	return objects;
}



void savePixelsAs2DArray_Picture(int* pixels1D, int size, Picture* pic) {
    // Iterate over each row in the 2D array
    for (int i = 0; i < size; i++) {
        // Iterate over each column in the current row
        for (int j = 0; j < size; j++) {
            // Compute the 1D index for the current pixel
            int index = i * size + j;
            // Save the pixel in the corresponding location in the 2D array
            pic->pixels[i][j] = pixels1D[index];
        }
    }
}

void picRecv(Picture* picture, int numPerProc) 
{
    int sizeOfPic;
	MPI_Status status;
	int* pixels = (int*)malloc(sizeof(int) * PICTURE_SIZE * PICTURE_SIZE); // convert to 1D array
	MALLOC_SUCCSSED(pixels);
    for (int i = 0 ; i < numPerProc; i++) // reciving workers work for the number of work per processes
    {
        MPI_Recv(&picture[i].id ,1 , MPI_INT, 0,MPI_ANY_TAG  ,MPI_COMM_WORLD, &status); 
		MPI_Recv(&sizeOfPic ,1 , MPI_INT, 0,MPI_ANY_TAG  ,MPI_COMM_WORLD, &status); 
        picture[i].size = sizeOfPic;
		MPI_Recv(pixels , sizeOfPic*sizeOfPic , MPI_INT, 0,MPI_ANY_TAG  ,MPI_COMM_WORLD, &status);
        savePixelsAs2DArray_Picture(pixels, picture[i].size, &picture[i]); 
    }
	free(pixels);
}



