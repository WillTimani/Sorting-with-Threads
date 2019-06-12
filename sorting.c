
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>


#define SIZE (sizeof(list)/sizeof(*list))
int list[] = {7, 12, 19, 3, 18, 4, 2, -5, 6, 15, 8}; 	//Starting list of numbers
int result[SIZE] = {0};								 	//Empty array of same size that will hold the sorted array

typedef struct SortingThreadParameters{					//Struct that will be passed into the sort function by the pthread
		int * subArray;									//Subarray that will contain the numbers to be sorted 
		unsigned int size;								//Size of the subarray
	} SortingThreadParameters;

typedef struct MergingThreadParameters{					//Struct that will be passed into the merge function by the pthread
		SortingThreadParameters left;					//Left sorting parameters containing the sorted subarray to be merged
		SortingThreadParameters right;					//Right sorting parameters containing the sorted subarray to be merged
	} MergingThreadParameters;

void *sort(void *inputparam){							//Void pointer input of the SortingThreadParameters
	struct SortingThreadParameters * params = (struct SortingThreadParameters*)inputparam; 	//Converting the void pointer to struct type SortingThreadParameters
		for(int i = 0; i < params->size; i++){							//Double nested for loops to loop through the array twice for bubble sort
			for(int j = 0; j < params->size -1; j++){
				if(params->subArray[j] > params->subArray[j+1]){		//If element of the array is larger than the next element
					int temp = params->subArray[j];						//Temporary variable used to swap elements						
					params->subArray[j] = params->subArray[j+1];		//Stores next element in the current elements position
					params->subArray[j+1] = temp;						//Stores the temporary variable's value containing the current element's value in the next elements position
				}
			}
		}

	pthread_exit(0);
}

void *merge(void *inputparam){							//Void pointer input of the MergingThreadParameters
	struct MergingThreadParameters * params = (struct MergingThreadParameters*)inputparam;	//Converting the void pointer to struct type MergingThreadParameters
	int resultpos = 0;									//The position of the sorted array as numbers are merged into it
	int pos1 = 0;										//Position of the left sorted subarray
	int pos2 = 0;										//Position of the right sorted subarray
	while(pos1 < params->left.size && pos2 < params->right.size){			//While neither positions of the subarrays are at the end
		if(params->left.subArray[pos1] < params->right.subArray[pos2]){		//If the left subarray's value at pos1 is less than the right subarray's value at pos2
			result[resultpos] = params->left.subArray[pos1];				//Sorted array at resultpos becomes left subarray's value at pos1
			pos1++;															
			resultpos++;													
		} else {
			result[resultpos] = params->right.subArray[pos2];				//Sorted array at resultpos becomes right subarray's value at pos2
			pos2++;
			resultpos++;
		}
	}
	while(pos1 < params->left.size){							//Places any remaining values into the sorted array
		result[resultpos] = params->left.subArray[pos1];
		pos1++;
		resultpos++;
	}
	while(pos2 < params->right.size){
		result[resultpos] = params->right.subArray[pos2];
		pos2++;
		resultpos++;
	}

	pthread_exit(0);
}

int main(int argc, char const *argv[])
{
	printf("Initial Array: {");			//For loop for printing the initial array values
	for(int i = 0; i < SIZE; i++){
		printf("%d", list[i]);
		if (i == SIZE - 1)
		{
			break;
		}
		printf(", ");
	}
	printf("}\n");

	SortingThreadParameters * paramsLeft = malloc( sizeof(SortingThreadParameters));	//Allocation of SortingThreadParemeters of the left subarray
	paramsLeft->subArray = list;
	paramsLeft->size = SIZE/2;

	pthread_t tid1;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_create(&tid1, &attr, sort, (void *)paramsLeft);

	SortingThreadParameters * paramsRight = malloc( sizeof(SortingThreadParameters));	//Allocation of SortingThreadParameters of the right subarray
	paramsRight->subArray = list + paramsLeft->size;
	paramsRight->size = SIZE - paramsLeft->size;

	pthread_t tid2;
	pthread_attr_init(&attr);
	pthread_create(&tid2, &attr, sort, (void *)paramsRight);

	pthread_join(tid1,NULL);
	pthread_join(tid2,NULL);

	MergingThreadParameters * paramsMerge = malloc( sizeof(MergingThreadParameters)); 	//Allocation of MergingThreadParameters 
	paramsMerge->left = *paramsLeft;
	paramsMerge->right = *paramsRight;

	pthread_t tid3;
	pthread_attr_init(&attr);
	pthread_create(&tid3, &attr, merge, (void *)paramsMerge);

	pthread_join(tid3,NULL);


	printf("Sorted Array: {");			//For loop printing out the values in the sorted array
	for(int i = 0; i < SIZE; i++){
		printf("%d", result[i]);
		if (i == SIZE - 1)
		{
			break;
		}
		printf(", ");
	}
	printf("}\n");

	return 0;
}