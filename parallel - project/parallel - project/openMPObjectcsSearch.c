#include "header.h"

int mainSearchFunc(int* resToFile,Picture pic, Object* obj,int sizeOfObj, float match, int rank)
{
// stop searching picture using array of 0 and store 1 if found pic
	int foundTempArr[sizeOfObj];
	for (int i = 0; i < sizeOfObj; i++) 
	{
		foundTempArr[i] = 0;
	}

	int objCounter = 0;
	int indexObjSize, foundObj = 0;
	
    omp_set_num_threads(4);
	#pragma omp parallel
	{
	#pragma omp for 
	for (int k =0 ; k < sizeOfObj; k ++)
		{
			for (int j=0; j < pic.size; j ++)
			{
				if (foundTempArr[k] == 1 ) continue; // check if object has been found, if so, continue to the next object
				// check picture borders limits
				indexObjSize = obj[k].size; 
				if (((j + indexObjSize) <= pic.size ))
				{
					foundObj = isObjInPosition(obj[k],pic,j,match);
					if (foundObj) 
					{	           
						printf("\tWe found Object #%d lurking in picture #%d at (%d,%d)! by Process #%d Awesome!\n", obj[k].id, pic.id, j,j, rank);
						resToFile[1 + 3*objCounter] = obj[k].id;
						resToFile[1 + 3*objCounter+ 1] = j;
						resToFile[1 + 3*objCounter + 2] = j;
						#pragma omp critical	// need 1 thread to keep eye on		
						{
							objCounter++;
							foundTempArr[k] = 1;
							resToFile[0] = objCounter;
							if (objCounter == 3){ return 1;}
						} 
						break;
					}
				}
			}	
		}
	}
	
	return 1;
}

int isObjInPosition(Object obj,Picture pic, int len, float matching_value)
{
	float difference = 0.0, found;
    int obj_size = obj.size;
    int row_end = len + obj_size;
    int col_end = len + obj_size;
	for (int i = len, k = 0; i < row_end; i++, k++)
    {
        for (int j = len, l = 0; j < col_end; j++, l++)
        {
            float pixel_diff = fabs((pic.pixels[i][j] - obj.pixels[k][l]) / pic.pixels[i][j]);
            difference += pixel_diff;
        }
    }
    difference /= obj_size * obj_size;
    found = difference <= matching_value ? 1 : 0;
    return found;
}	


