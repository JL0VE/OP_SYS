#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include <pthread.h>
#include <assert.h>

 int** firstMatrix;
 int** secondMatrix;
 int** finalMatrix;
 double boundSize;
 int matrixSize;


 int** createMatrix(int size);
 void* multiplyMatrix (int size);
 void printMatrix(int**, int);
 int** createFinalMatrix(int size);

 int main(int argc, char** argv) {
	 setvbuf (stdout, NULL, _IONBF, 0);
   //Create the instance for the Rand()
   srand(time(NULL));


   int threadCount = 0;
   matrixSize  = 4;

   printf("How many threads do you want?(1-16)");
   scanf("%d",&threadCount);

   boundSize = matrixSize/threadCount;

   firstMatrix = createMatrix(matrixSize);
   secondMatrix = createMatrix(matrixSize);
   printMatrix(firstMatrix, 0);
   printMatrix(secondMatrix, 0);
   printf("\n\n");

   //Set both matrices with random numbers 0-99


/**
 * -------------------------------------------------------------
 */
//   pthread_t p1, p2, p3, p4, p5, p6, p7;
//   pthread_t matrixTwo;
//   int rc;
//   rc = pthread_create(&p1, NULL, multiplyMatrix, &args);
//
//   pthread_create( pthread_t * thread,
//		   const pthread_attr_t * attr,
//		   void * (*start_routine)(void*),
//		   void * arg);

   clock_t begin, end;
    long time_spent;




//   thread test---------------------------------------------!!
   int i;
   finalMatrix = createFinalMatrix(matrixSize);
   int section = 0;
   //pthread_t* threads = malloc(sizeof(pthread_t*) * threadCount);
   //pthread_t* originalThread = threads;

   pthread_t p1,p2,p3,p4;


   begin = clock();
      /* here, do your time-consuming job */
//   if (threadCount == 1) {
//	   finalMatrix = multiplyMatrix(section);
//   } else {
	  // for (i = 0; i < threadCount; i++) {
	   	   pthread_create(&p1, NULL, &multiplyMatrix, section++);
	   	   pthread_create(&p2, NULL, &multiplyMatrix, section++);
	   	   pthread_create(&p3, NULL, &multiplyMatrix, section++);
	   	   pthread_create(&p4, NULL, &multiplyMatrix, section++);
	   	   pthread_join(p1, NULL);
	   	   pthread_join(p2, NULL);
	   	   pthread_join(p3, NULL);
	   	   pthread_join(p4, NULL);

   	  // }
//
//   	   for (i = 0; i < threadCount; i++) {
//	   	   pthread_join(*originalThread++, NULL);
//   	   }
  // }
   end = clock();
   time_spent = (end - begin) / CLOCKS_PER_SEC;

   //Print the elapsed time
   printf("Begin time: %d", begin / CLOCKS_PER_SEC);
   printf("\nEnd time: %d", end / CLOCKS_PER_SEC);
   printf("\nTime To Completion: %d\n", time_spent);
//   printf("PROCESS TIME: %d milliseconds\n", msec%1000);
//   printMatrix(finalMatrix, 1);
   pthread_exit(NULL);
   return 0;
 }

 int** createFinalMatrix(int size) {
 	int** matrix = malloc(size * sizeof(int *));


 	int i, j;

 	for (i = 0; i < size; i++) {
 		matrix[i] = malloc(sizeof(int));
 	}

 	for (i = 0; i < size; i++) {
 		for (j = 0; j < size; j++) {
 			matrix[i][j] = 0;
 		}
 	}
 	return matrix;
 }


 /**
  * This methods creates an array with random numbers and returns it
  * @param int size of matrix
  * @return int** 2D array of a matrix
  */
 int** createMatrix(int size) {
 	int** matrix = malloc(size * sizeof(int *));


 	int i, j;

 	for (i = 0; i < size; i++) {
 		matrix[i] = malloc(sizeof(int));
 	}

 	for (i = 0; i < size; i++) {
 		for (j = 0; j < size; j++) {
 			matrix[i][j] = rand() % 2;
 		}
 	}
 	return matrix;
 }

 /**
  * This method multiplies the first matrix with the second matrix
  * and returns a int** product matrix
  * @param int** first matrix
  * @param int** second matrix
  * @param int size of the matrices
  * @return int** product matrix
  */
 void* multiplyMatrix (int boundSection) {
 	int i, j, z, sum = 0;
 	int firstBound = boundSection * boundSize;
 	int lastBound = (boundSection+1 * boundSize);
//I J Z SUM
//

 	for (i = firstBound; i < lastBound; i++) {
 	      for (j = firstBound; j < lastBound; j++) {
 	        for (z = firstBound; z < lastBound; z++) {
 	          sum = sum + firstMatrix[i][z]*secondMatrix[z][j];
 	        }


 	        finalMatrix[i][j] = sum;
 	        sum = 0;
 	      }
 	 }

 	printf("\nAfter Bound #%d\n", boundSection);
 	printMatrix(finalMatrix, 0);
 	printf("\n");

	return finalMatrix;
 }

 /**
  * This method prints out the array
  * @param int** matrix to print
  * @param int size of the array
  */
 void printMatrix(int **matrix, int size) {
 	int i, j;
 	for (i = 0; i < matrixSize; i++) {
 		printf("[ ");
 		for (j = 0; j < matrixSize; j++) {
 			printf("%d ", matrix[i][j]);
 		}
 		printf("]\n");
 	}
 }
