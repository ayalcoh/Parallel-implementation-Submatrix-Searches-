#include "header.h"

int readInputFile(const char* filename, FileData* data)
{
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "Error: could not open file '%s'\n", filename);
        return 0;
    }
    // read matching value
    if (fscanf(fp, "%f", &data->matching_value) != 1) {
        fprintf(stderr, "Error: could not read matching value from file '%s'\n", filename);
        fclose(fp);
        return 0;
    }
    
	if (readPictures(fp,filename, data) == 1) printf("Saving pictures from file\n");
	if (readObjects(fp,filename, data) == 1) printf("Saving objects from file\n");
	
	return 1;
 }

 

 void write_output_file(int** resultOutput, int rows, int cols) 
{
     FILE *fp;
    fp = fopen("output.txt", "w");
     if (fp != NULL) 
     {
        printf("Starting to write the output file\n");
        for (int i = 0; i < rows; i++) {
            int numOfObjects = resultOutput[i][0];
     
            // write results to file
            fprintf(fp, "Picture %d: ", i+1);
            if (numOfObjects == 3) {
                fprintf(fp, "found Objects: %d at Position(%d,%d) - %d Position(%d,%d) - %d Position(%d,%d)\n",
                    resultOutput[i][1], resultOutput[i][2], resultOutput[i][3],
                    resultOutput[i][4], resultOutput[i][5], resultOutput[i][6],
                    resultOutput[i][7], resultOutput[i][8], resultOutput[i][9]);
            } else {
                fprintf(fp, "No three different Objects were found\n");          
            }
        }
        printf("Done writing the output file\n");
        fclose(fp);
    } else {
        printf("Error opening file\n");
    }
}
 
 int readPictures( FILE* fp,const char* filename ,FileData* data)
 {
    // read number of pictures
    if (fscanf(fp, "%d", &data->num_pictures) != 1) {
        fprintf(stderr, "Error: could not read number of pictures from file '%s'\n", filename);
        fclose(fp);
        return 0;
    }
    // allocate memory for pictures
    data->pictures = (Picture*) malloc(data->num_pictures * sizeof(Picture));
    if (data->pictures == NULL) {
        fprintf(stderr, "Error: could not allocate memory for pictures\n");
        fclose(fp);
        return 0;
    }
    
    // read pictures

    for (int i = 0; i < data->num_pictures; i++) {
        Picture* pic = &data->pictures[i];
        if (fscanf(fp, "%d", &pic->id) != 1) {
            fprintf(stderr, "Error: could not read picture ID from file '%s'\n", filename);
            fclose(fp);
            return 0;
        }
        if (fscanf(fp, "%d", &pic->size) != 1) {
            fprintf(stderr, "Error: could not read picture size from file '%s'\n", filename);
            fclose(fp);
            return 0;
        }
        if (pic->size > PICTURE_SIZE) {
            fprintf(stderr, "Error: picture size is too large in file '%s'\n", filename);
            fclose(fp);
            return 0;
        }
       
        for (int y = 0; y < pic->size; y++) {
            for (int x = 0; x < pic->size; x++) {
                if (fscanf(fp, "%d", &pic->pixels[y][x]) != 1) {
                    fprintf(stderr, "Error: could not read pixel value from file '%s'\n", filename);
                    fclose(fp);
            		return 0;
                }
            }
        }
 	}   
 	return 1;
}

int readObjects( FILE* fp,const char* filename, FileData* data)
{
// read number of objects
    if (fscanf(fp, "%d", &data->num_objects) != 1) {
        fprintf(stderr, "Error: could not read number of objects from file '%s'\n", filename);
        fclose(fp);
        return 0;
    }
    // allocate memory for objects
    data->objects = (Object*) malloc(data->num_objects * sizeof(Object));
    if (data->objects == NULL) {
        fprintf(stderr, "Error: could not allocate memory for objects\n");
        fclose(fp);
        return 0;
 	}
 	
 	// read objects
    for (int i = 0; i < data->num_objects; i++) {
        Object* obj = &data->objects[i];
        if (fscanf(fp, "%d", &obj->id) != 1) {
            fprintf(stderr, "Error: could not read object ID from file '%s'\n", filename);
            fclose(fp);
            return 0;
        }
        if (fscanf(fp, "%d", &obj->size) != 1) {
            fprintf(stderr, "Error: could not read object size from file '%s'\n", filename);
            fclose(fp);
            return 0;
        }
        if (obj->size > OBJECT_SIZE) {
            fprintf(stderr, "Error: object size is too large in file '%s'\n", filename);
            fclose(fp);
            return 0;
        }
        for (int y = 0; y < obj->size; y++) {
            for (int x = 0; x < obj->size; x++) {
                if (fscanf(fp, "%d", &obj->pixels[y][x]) != 1) {
                    fprintf(stderr, "Error: could not read pixel value from file '%s'\n", filename);
                    fclose(fp);
                    return 0;
                }
            }
        }
    }
    return 1;
}
