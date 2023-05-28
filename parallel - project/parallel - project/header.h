#pragma once
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <omp.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"

#define PICTURE_SIZE 400
#define OBJECT_SIZE 300				
#define RESULT_COLUMN 10 // structure example for row 0(pic1) : [num of objects] [obj id][position x] [position y] [obj id] ...

#define MALLOC_SUCCSSED(ptr) if (!ptr) { \
                              fprintf(stderr, "Memory allocation failed\n"); \
                              exit(EXIT_FAILURE); \
                          }


typedef struct {
    int id;
    int size;
    int pixels[PICTURE_SIZE][PICTURE_SIZE];
} Picture;

typedef struct {
    int id;
    int size;
    int pixels[OBJECT_SIZE][OBJECT_SIZE];
} Object;

typedef struct FileData{
    float matching_value;
    int num_pictures;
    Picture* pictures;
    int num_objects;
    Object* objects;
} FileData;

// ########### files handler functions ##########
int readInputFile(const char* filename, FileData* data);
int readPictures( FILE* fp,const char* filename, FileData* data);
int readObjects( FILE* fp,const char* filename, FileData* data);
void write_output_file(int** resultsMatrix, int rows, int cols);



// ########### openMP functions ##########
int mainSearchFunc(int* resToFile,Picture pic, Object* obj,int sizeOfObj, float match, int rank);
int isObjInPosition(Object obj,Picture pic, int len, float matching_value);
void write_output_file(int** resultOutput, int rows, int cols);



// ########### MPI functions ##########
void transferSendInput(float match,int num_objects,int num_pictures,Object* objects, int rankDest);
void sendPicToSlave(Picture* picture, int size);
Object* MPIrecvData(int sourceRank,int* numObjs, float* matching_value, int* numPics);
void picRecv(Picture* picture, int numPerProc);
void savePixelsAs2DArray_Object(int* pixels1D, int size, Object* obj);
void savePixelsAs2DArray_Picture(int* pixels1D, int size, Picture* pic);
void obj_pixels_ToSave(Object *obj, int *arr);
void save_pixels_Picture(Picture* pic, int *pixels_array);





